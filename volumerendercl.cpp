/**
 * \file
 *
 * \author Valentin Bruder
 *
 * \copyright Copyright (C) 2018 Valentin Bruder
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "volumerendercl.h"

#include <functional>
#include <algorithm>
#include <numeric>
#include <omp.h>

/**
 * @brief RoundPow2
 * @param iNumber
 * @return
 */
static unsigned int RoundPow2(unsigned int n)
{
    // next highest power of 2
    // (cf: http://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2)
    unsigned int val = n - 1u;
    val |= val >> 1;
    val |= val >> 2;
    val |= val >> 4;
    val |= val >> 8;
    val |= val >> 16;
    val++;
    // previous power of 2
    unsigned int x = val >> 1;
    // round to nearest of the two
    return (val - n) > (n - x) ? x : val;
}


/**
 * @brief VolumeRenderCL::VolumeRenderCL
 */
VolumeRenderCL::VolumeRenderCL() :
    _volLoaded(false)
  , _lastExecTime(0.0)
  , _modelScale{1.0, 1.0, 1.0}
  , _useGL(true)
{
}


/**
 * @brief VolumeRenderCL::~VolumeRenderCL
 */
VolumeRenderCL::~VolumeRenderCL()
{
}


/**
 * @brief VolumeRenderCL::logCLerror
 * @param error
 */
void VolumeRenderCL::logCLerror(cl::Error error)
{
    std::cerr << "Error in " << error.what() << ": "
              << getCLErrorString(error.err()) << std::endl;
    throw std::runtime_error( "ERROR: " + std::string(error.what()) + " ("
                              + getCLErrorString(error.err()) + ")");
}


/**
 * @brief VolumeRenderCL::initialize
 */
void VolumeRenderCL::initialize(bool useGL, bool useCPU, cl_vendor vendor)
{
    cl_device_type type = useCPU ? CL_DEVICE_TYPE_CPU : CL_DEVICE_TYPE_GPU;
    try // opencl scope
    {
        // FIXME: Using CPU segfaults on most tff changes
        if (useGL && !useCPU)
        {
            _useGL = useGL;
            _contextCL = createCLGLContext(type, vendor);
        }
        else
        {
            if (useGL)
                std::cout << "Cannot use OpenGL context shring with CPU devices. "
                          << "Falling back to buffer generation." << std::endl;
            _contextCL = createCLContext(type, vendor);
            _useGL = false;
        }

        cl_command_queue_properties cqp = 0;
#ifdef CL_QUEUE_PROFILING_ENABLE
        cqp = CL_QUEUE_PROFILING_ENABLE;
#endif
        _queueCL = cl::CommandQueue(_contextCL, cqp);
    }
    catch (cl::Error err)
    {
        logCLerror(err);
    }

#ifdef _WIN32
    initKernel("kernels//volumeraycast.cl", "-DCL_STD=CL1.2 -DESS");
#else
    initKernel("../RaycastLight/kernels/volumeraycast.cl", "-DCL_STD=CL1.2 -DESS");
#endif // _WIN32
}


/**
 * @brief VolumeRenderCL::initKernel
 * @param fileName
 * @param buildFlags
 */
void VolumeRenderCL::initKernel(const std::string fileName, const std::string buildFlags)
{
    try
    {
        cl::Program program = buildProgramFromSource(_contextCL, fileName, buildFlags);
        _raycastKernel = cl::Kernel(program, "volumeRender");
        cl_float16 view = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
        _raycastKernel.setArg(VIEW, view);
        _raycastKernel.setArg(SAMPLING_RATE, 1.5f);     // default step size 0.5*voxel size
        _raycastKernel.setArg(ORTHO, 0);                // perspective cam by default
        _raycastKernel.setArg(ILLUMINATION, 1);         // illumination on by default
        _raycastKernel.setArg(BOX, 0);
        _raycastKernel.setArg(LINEAR, 1);
        cl_float4 bgColor = {{1.f, 1.f, 1.f, 1.f}};
        _raycastKernel.setArg(BACKGROUND, bgColor);
        _raycastKernel.setArg(AO, 0);                   // ambient occlusion off by default
        _raycastKernel.setArg(CONTOURS, 0);             // contour lines off by default
        _raycastKernel.setArg(AERIAL, 0);               // aerial perspective off by default
		cl_float2 cp = { 0.f,0.f };
		_raycastKernel.setArg(CURSOR_POS, cp);
		_raycastKernel.setArg(RECTANGLE_EXTS, cp);
		_raycastKernel.setArg(ELLIPSE_2, cp);
		cl_uint3 inverts = {1, 0, 0};
		_raycastKernel.setArg(INVERT, inverts);
		cl_float3 rf = { 1.0f, 1.0f, 1.0f };
		_raycastKernel.setArg(RESOLUTIONFACTOR, rf);
		_raycastKernel.setArg(MODE, 0);					// Standard

        _genBricksKernel = cl::Kernel(program, "generateBricks");
        _downsamplingKernel = cl::Kernel(program, "downsampling");
		_interpolationKernelForDDC = cl::Kernel(program, "interpolateTexelsFromDDC");
		_interpolationKernelForTRI = cl::Kernel(program, "interpolateTexelsFromTRI");
    }
    catch (cl::Error err)
    {
        logCLerror(err);
    }
}


/**
 * @brief VolumeRenderCL::setMemObjects
 */
void VolumeRenderCL::setMemObjectsRaycast(const int t)
{
    _raycastKernel.setArg(VOLUME, _volumesMem.at(t));
    _raycastKernel.setArg(BRICKS, _bricksMem.at(t));
    _raycastKernel.setArg(TFF, _tffMem);
    if (_useGL)
        _raycastKernel.setArg(OUTPUT, _outputMem);
    else
        _raycastKernel.setArg(OUTPUT, _outputMemNoGL);
    _raycastKernel.setArg(TFF_PREFIX, _tffPrefixMem);
    cl_float3 modelScale = {_modelScale[0], _modelScale[1], _modelScale[2]};
    _raycastKernel.setArg(MODEL_SCALE, modelScale);
}


/**
 * @brief VolumeRenderCL::setMemObjectsBrickGen
 */
void VolumeRenderCL::setMemObjectsBrickGen(const int t)
{
    if (_volumesMem.size() <= static_cast<size_t>(t) || _bricksMem.size() <= static_cast<size_t>(t))
        return;
    _genBricksKernel.setArg(VOLUME, _volumesMem.at(t));
    _genBricksKernel.setArg(BRICKS, _bricksMem.at(t));
}

void VolumeRenderCL::setMemObjectsInterpolation(GLuint inTexId, GLuint outTexId)
{
	_inputMem = cl::ImageGL(_contextCL, CL_MEM_READ_ONLY, GL_TEXTURE_2D, 0, inTexId);
	_outputMem = cl::ImageGL(_contextCL, CL_MEM_WRITE_ONLY, GL_TEXTURE_2D, 0, outTexId);

	_interpolationKernelForDDC.setArg(0, _inputMem); // in Data
	_interpolationKernelForDDC.setArg(1, _outputMem);	// out Data

	_interpolationKernelForTRI.setArg(0, _inputMem); // in Data
	_interpolationKernelForTRI.setArg(1, _outputMem);	// out Data
}


/**
 * @brief VolumeRenderCL::setMemObjectsDownsampling
 */
const std::string VolumeRenderCL::volumeDownsampling(const int t, const int factor)
{
    if (!_dr.has_data() || factor < 2)
        return std::string("");

    std::array<unsigned int, 3> texSize = {1u, 1u, 1u};
    texSize.at(0) = ceil(_dr.properties().volume_res.at(0)/(double)factor);
    texSize.at(1) = ceil(_dr.properties().volume_res.at(1)/(double)factor);
    texSize.at(2) = ceil(_dr.properties().volume_res.at(2)/(double)factor);

    if (texSize.at(0) < 64)
    {
        std::cerr << "Error: Down sampled volume size would be smaller than 64. Aborting."
                  << std::endl;
        throw std::invalid_argument("Could not create down-sampled volume data set, because \
                                     the resolution would be smaller than the minimum (64x64x64).");
    }

    cl::ImageFormat format;
    format.image_channel_order = CL_R;

    if (_dr.properties().format == "UCHAR")
        format.image_channel_data_type = CL_UNORM_INT8;
    // FIXME: format
    else if (_dr.properties().format == "USHORT")
        format.image_channel_data_type = CL_UNORM_INT8; //CL_UNORM_INT16
    else if (_dr.properties().format == "FLOAT")
        format.image_channel_data_type = CL_FLOAT;
    else
        throw std::invalid_argument("Unknown or invalid volume data format.");

    try
    {
        cl::Image3D lowResVol = cl::Image3D(_contextCL,
                                            CL_MEM_WRITE_ONLY,
                                            format,
                                            texSize.at(0),
                                            texSize.at(1),
                                            texSize.at(2),
                                            0, 0,
                                            NULL);

        _downsamplingKernel.setArg(VOLUME, _volumesMem.at(t));
        _downsamplingKernel.setArg(1, lowResVol);

        cl::NDRange globalThreads(texSize.at(0), texSize.at(1), texSize.at(2));
        cl::Event ndrEvt;
        _queueCL.enqueueNDRangeKernel(_downsamplingKernel, cl::NullRange,
                                      globalThreads, cl::NullRange, NULL, &ndrEvt);
        _queueCL.finish();    // global sync

        // read back volume data
        // TODO: format selection
        std::vector<unsigned char> outputData(texSize.at(0)*texSize.at(1)*texSize.at(2));
        std::array<size_t, 3> origin = {{0, 0, 0}};
        std::array<size_t, 3> region = {{texSize.at(0), texSize.at(1), texSize.at(2)}};
        _queueCL.enqueueReadImage(lowResVol,
                                  CL_TRUE,
                                  origin,
                                  region,
                                  0, 0,
                                  outputData.data());
        _queueCL.flush();    // global sync

        // dump to file
        size_t lastindex = _dr.properties().dat_file_name.find_last_of(".");
        std::string rawname = _dr.properties().dat_file_name.substr(0, lastindex);
        rawname += "_";
        rawname += std::to_string(texSize.at(0));
        std::ofstream file(rawname + ".raw", std::ios::out|std::ios::binary);
        std::cout << "Writing downsampled volume data to "
                  << rawname << "_" << std::to_string(texSize.at(0)) << ".raw ...";
        std::copy(outputData.cbegin(), outputData.cend(),
                  std::ostream_iterator<unsigned char>(file));
        file.close();

        // Generate .dat file and write out
        std::ofstream datFile(rawname + ".dat", std::ios::out);
//        lastindex = _dr.properties().raw_file_names.at(0).find_last_of(".");
        lastindex = rawname.find_last_of(".");
        size_t firstindex = rawname.find_last_of("/\\");
        std::string rawnameShort = rawname.substr(firstindex + 1, lastindex);
        datFile << "ObjectFileName: \t" << rawnameShort << ".raw\n";
        datFile << "Resolution: \t\t" << texSize.at(0) << " " << texSize.at(1) << " "
                                      << texSize.at(2) << "\n";
        datFile << "SliceThickness: \t" << _dr.properties().slice_thickness.at(0) << " "
                << _dr.properties().slice_thickness.at(1) << " "
                << _dr.properties().slice_thickness.at(2)
                << "\n";
        // FIXME: format
        datFile << "Format: \t\t\t" << "UCHAR" << "\n"; //_dr.properties().format << "\n";
        datFile.close();
        std::cout << " Done." << std::endl;
        return rawname;
    }
    catch (cl::Error err)
    {
        logCLerror(err);
    }
    return std::string("");
}

cl::ImageGL VolumeRenderCL::getOutputMemGL()
{
	return cl::ImageGL(_outputMem);
}


/**
 * @brief VolumeRenderCL::calcScaling
 */
void VolumeRenderCL::calcScaling()
{
    if (!_dr.has_data())
        return;

    _modelScale = { static_cast<float>(_dr.properties().volume_res.at(0)),
                    static_cast<float>(_dr.properties().volume_res.at(1)),
                    static_cast<float>(_dr.properties().volume_res.at(2)) };

    std::valarray<float> thickness = { static_cast<float>(_dr.properties().slice_thickness.at(0)),
                                       static_cast<float>(_dr.properties().slice_thickness.at(1)),
                                       static_cast<float>(_dr.properties().slice_thickness.at(2)) };
    _modelScale *= thickness*(1.f/thickness[0]);
#undef max  // error here if we don't undef max
    _modelScale = _modelScale.max() / _modelScale;
}


/**
 * @brief VolumeRenderCL::scaleVolume
 * @param scale
 */
void VolumeRenderCL::scaleVolume(std::valarray<float> scale)
{
    _modelScale *= scale;
}


/**
 * @brief VolumeRenderCL::updateKernelArgs
 * @param viewMat
 */
void VolumeRenderCL::updateView(const std::array<float, 16> viewMat)
{
    if (!_dr.has_data() || _modelScale.size() < 3)
        return;

    cl_float16 view;
    for (size_t i = 0; i < 16; ++i)
        view.s[i] = viewMat[i];
    try{
        _raycastKernel.setArg(VIEW, view);
    } catch (cl::Error err) {
        logCLerror(err);
    }
}


/**
 * @brief VolumeRenderCL::updateStepSize
 * @param stepSize
 */
void VolumeRenderCL::updateSamplingRate(const double samplingRate)
{
    try{
        _raycastKernel.setArg(SAMPLING_RATE, static_cast<cl_float>(samplingRate));
    } catch (cl::Error err) {
        logCLerror(err);
    }
}


/**
 * @brief VolumeRenderCL::updateOutputImg
 * @param width
 * @param height
 */
void VolumeRenderCL::updateOutputImg(const size_t width, const size_t height, GLuint texId, cl_mem_flags flags)
{
	// std::cout << "updateOutputImage: " << std::to_string(width) << ", " << std::to_string(height) << ", texId: " << std::to_string(texId) << std::endl;

    cl::ImageFormat format;
    format.image_channel_order = CL_RGBA;
    format.image_channel_data_type = CL_FLOAT;
    try
    {
        if (_useGL)
            _outputMem = cl::ImageGL(_contextCL, flags, GL_TEXTURE_2D, 0, texId);
        else
        {
            _outputMemNoGL = cl::Image2D(_contextCL, flags, format, width, height);
            _raycastKernel.setArg(OUTPUT, _outputMemNoGL);
        }
    }
    catch (cl::Error err)
    {
        logCLerror(err);
    }
}


/**
 * @brief VolumeRenderCL::runRaycast
 * @param imgSize
 */
void VolumeRenderCL::runRaycast(const size_t width, const size_t height, const int t)
{
    if (!this->_volLoaded)
        return;
    try // opencl scope
    {
        setMemObjectsRaycast(t);
        size_t lDim = 8;    // local work group dimension
        cl::NDRange globalThreads(width + (lDim - width % lDim), height + (lDim - height % lDim));
        cl::NDRange localThreads(lDim, lDim);
        cl::Event ndrEvt;

        std::vector<cl::Memory> memObj;
        memObj.push_back(_outputMem);
        _queueCL.enqueueAcquireGLObjects(&memObj);
        _queueCL.enqueueNDRangeKernel(
                    _raycastKernel, cl::NullRange, globalThreads, localThreads, NULL, &ndrEvt);
        _queueCL.enqueueReleaseGLObjects(&memObj);
        _queueCL.finish();    // global sync

#ifdef CL_QUEUE_PROFILING_ENABLE
        cl_ulong start = 0;
        cl_ulong end = 0;
        ndrEvt.getProfilingInfo(CL_PROFILING_COMMAND_START, &start);
        ndrEvt.getProfilingInfo(CL_PROFILING_COMMAND_END, &end);
        _lastExecTime = static_cast<double>(end - start)*1e-9;
//        std::cout << "Kernel time: " << _lastExecTime << std::endl << std::endl;
#endif
    }
    catch (cl::Error err)
    {
        logCLerror(err);
    }
}

/**
 * @brief VolumeRenderCL::runRaycastNoGL
 * @param width
 * @param height
 * @param t
 * @param output
 */
void VolumeRenderCL::runRaycastNoGL(const size_t width, const size_t height, const int t,
                                    std::vector<float> &output)
{
    if (!this->_volLoaded)
        return;
    try // opencl scope
    {
        setMemObjectsRaycast(t);
        size_t lDim = 8;    // local work group dimension
        cl::NDRange globalThreads(width + (lDim - width % lDim), height + (lDim - height % lDim));
        cl::NDRange localThreads(lDim, lDim);
        cl::Event ndrEvt;

        _queueCL.enqueueNDRangeKernel(
                    _raycastKernel, cl::NullRange, globalThreads, localThreads, NULL, &ndrEvt);
        output.resize(width*height*4);
        cl::Event readEvt;
        std::array<size_t, 3> origin = {{0, 0, 0}};
        std::array<size_t, 3> region = {{width, height, 1}};
        _queueCL.enqueueReadImage(_outputMemNoGL,
                                  CL_TRUE,
                                  origin,
                                  region,
                                  0, 0,
                                  output.data(),
                                  NULL,
                                  &readEvt);
        _queueCL.flush();    // global sync

#ifdef CL_QUEUE_PROFILING_ENABLE
        cl_ulong start = 0;
        cl_ulong end = 0;
        ndrEvt.getProfilingInfo(CL_PROFILING_COMMAND_START, &start);
        ndrEvt.getProfilingInfo(CL_PROFILING_COMMAND_END, &end);
        _lastExecTime = static_cast<double>(end - start)*1e-9;
//        std::cout << "Kernel time: " << _lastExecTime << std::endl << std::endl;
#endif
    }
    catch (cl::Error err)
    {
        logCLerror(err);
    }
}


/**
 * @brief VolumeRenderCL::generateBricks
 * @param volumeData
 */
void VolumeRenderCL::generateBricks()
{
    if (!_dr.has_data())
        return;
    try
    {
        // calculate brick size
        const unsigned int numBricks = 64u;
        std::array<unsigned int, 3> brickRes = {1u, 1u, 1u};
        brickRes.at(0) = RoundPow2(_dr.properties().volume_res.at(0)/numBricks);
        brickRes.at(1) = RoundPow2(_dr.properties().volume_res.at(1)/numBricks);
        brickRes.at(2) = RoundPow2(_dr.properties().volume_res.at(2)/numBricks);
        std::array<unsigned int, 3> bricksTexSize = {1u, 1u, 1u};
        bricksTexSize.at(0) = ceil(_dr.properties().volume_res.at(0)/(double)brickRes.at(0));
        bricksTexSize.at(1) = ceil(_dr.properties().volume_res.at(1)/(double)brickRes.at(1));
        bricksTexSize.at(2) = ceil(_dr.properties().volume_res.at(2)/(double)brickRes.at(2));

        // set memory object
        cl::ImageFormat format;
        format.image_channel_order = CL_RG;  // NOTE: CL_RG for min+max

        if (_dr.properties().format == "UCHAR")
            format.image_channel_data_type = CL_UNORM_INT8;
        else if (_dr.properties().format == "USHORT")
            format.image_channel_data_type = CL_UNORM_INT16;
        else if (_dr.properties().format == "FLOAT")
            format.image_channel_data_type = CL_FLOAT;
        else
            throw std::invalid_argument("Unknown or invalid volume data format.");

        _bricksMem.clear();
        for (size_t i = 0; i < _dr.properties().raw_file_names.size(); ++i)
        {
            _bricksMem.push_back(cl::Image3D(_contextCL,
                                             CL_MEM_READ_WRITE | CL_MEM_HOST_NO_ACCESS,
                                             format,
                                             bricksTexSize.at(0),
                                             bricksTexSize.at(1),
                                             bricksTexSize.at(2)));
            // run aggregation kernel
            setMemObjectsBrickGen(i);
            size_t lDim = 4;    // local work group dimension: 4*4*4=64
            cl::NDRange globalThreads(bricksTexSize.at(0) + (lDim - bricksTexSize.at(0) % lDim),
                                      bricksTexSize.at(1) + (lDim - bricksTexSize.at(1) % lDim),
                                      bricksTexSize.at(2) + (lDim - bricksTexSize.at(2) % lDim));
            cl::NDRange localThreads(lDim, lDim, lDim);
//            cl::Event ndrEvt;
            _queueCL.enqueueNDRangeKernel(_genBricksKernel, cl::NullRange, globalThreads,
                                          localThreads); //, NULL, &ndrEvt);
            _queueCL.finish();
//            cl_ulong start = 0;
//            cl_ulong end = 0;
//            ndrEvt.getProfilingInfo(CL_PROFILING_COMMAND_START, &start);
//            ndrEvt.getProfilingInfo(CL_PROFILING_COMMAND_END, &end);
//            double execTime = static_cast<double>(end - start)*1e-9;
//            std::cout << "Build up time: " << execTime << std::endl;
        }
    }
    catch (cl::Error err)
    {
        throw std::runtime_error( "ERROR: " + std::string(err.what()) + "("
                                  + getCLErrorString(err.err()) + ")");
    }
}

/**
 * @brief VolumeRenderCL::volDataToCLmem
 * @param volumeData
 */
void VolumeRenderCL::volDataToCLmem(const std::vector<std::vector<char>> &volumeData)
{
    if (!_dr.has_data())
        return;
    try
    {
        cl::ImageFormat format;
        format.image_channel_order = CL_R;
        int formatMultiplier = 1;

        if (_dr.properties().format == "UCHAR")
            format.image_channel_data_type = CL_UNORM_INT8;
        else if (_dr.properties().format == "USHORT")
        {
            format.image_channel_data_type = CL_UNORM_INT16;
            formatMultiplier = 2;
        }
        else if (_dr.properties().format == "FLOAT")
        {
            format.image_channel_data_type = CL_FLOAT;
            formatMultiplier = 4;
        }
        else
            throw std::invalid_argument("Unknown or invalid volume data format.");

        _volumesMem.clear();
        for (const auto &v : volumeData)
        {
            if(_dr.properties().volume_res[0]*_dr.properties().volume_res[1]*
                     _dr.properties().volume_res[2]*formatMultiplier > v.size())
            {
                _dr.clearData();
                throw std::runtime_error( "Volume size does not match size specified in dat file.");
            }
            _volumesMem.push_back(cl::Image3D(_contextCL,
                                              CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                              format,
                                              _dr.properties().volume_res[0],
                                              _dr.properties().volume_res[1],
                                              _dr.properties().volume_res[2],
                                              0, 0,
                                              (void *)v.data()));
        }
    }
    catch (cl::Error err)
    {
        throw std::runtime_error( "ERROR: " + std::string(err.what()) + "("
                                  + getCLErrorString(err.err()) + ")");
    }
}

/**
 * @brief VolumeRenderCL::loadVolumeData
 * @param fileName
 */
int VolumeRenderCL::loadVolumeData(const std::string fileName)
{
    this->_volLoaded = false;
    std::cout << "Loading volume data defined in " << fileName << std::endl;
    try
    {
        _dr.read_files(fileName);
        std::cout << _dr.data().front().size()*_dr.data().size() << " bytes have been read from "
                  << _dr.data().size() << " file(s)." << std::endl;
        std::cout << _dr.properties().to_string() << std::endl;
        volDataToCLmem(_dr.data());
        calcScaling();
    }
    catch (std::invalid_argument e)
    {
        throw std::runtime_error(e.what());
    }

    // set initally a simple linear transfer function
    std::vector<unsigned char> tff(256*4, 0);
    std::iota(tff.begin() + 3, tff.end(), 0);
    setTransferFunction(tff);

    std::vector<unsigned int> prefixSum(256, 0);
#pragma omp for
    for (int i = 0; i < (int)prefixSum.size(); ++i)
        prefixSum.at(i) = i*4;

    std::partial_sum(prefixSum.begin(), prefixSum.end(), prefixSum.begin());
    setTffPrefixSum(prefixSum);

    this->_volLoaded = true;
    return _dr.data().size();
}


/**
 * @brief VolumeRenderCL::hasData
 * @return
 */
bool VolumeRenderCL::hasData()
{
    return this->_volLoaded;
}


/**
 * @brief VolumeRenderCL::getResolution
 * @return
 */
const std::array<unsigned int, 3> VolumeRenderCL::getResolution() const
{
    if (!_dr.has_data())
        return std::array<unsigned int, 3> {{0, 0, 0}};
    return _dr.properties().volume_res;
}


/**
 * @brief VolumeRenderCL::setTransferFunction
 * @param tff
 */
void VolumeRenderCL::setTransferFunction(std::vector<unsigned char> &tff)
{
    if (!_dr.has_data())
        return;
    try
    {
        cl::ImageFormat format;
        format.image_channel_order = CL_RGBA;
        format.image_channel_data_type = CL_UNORM_INT8;

        cl_mem_flags flags = CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR;
        // divide size by 4 because of RGBA
        _tffMem = cl::Image1D(_contextCL, flags, format, tff.size() / 4, tff.data());
        generateBricks();

        std::vector<unsigned int> prefixSum;
        // copy only alpha values (every fourth element)
        for (int i = 3; i < static_cast<int>(tff.size()); i += 4)
            prefixSum.push_back(static_cast<unsigned int>(tff.at(i)));
        std::partial_sum(prefixSum.begin(), prefixSum.end(), prefixSum.begin());
        setTffPrefixSum(prefixSum);
    }
    catch (cl::Error err)
    {
        logCLerror(err);
    }
}


/**
 * @brief VolumeRenderCL::setTffPrefixSum
 * @param tffPrefixSum
 */
void VolumeRenderCL::setTffPrefixSum(std::vector<unsigned int> &tffPrefixSum)
{
    if (!_dr.has_data())
        return;
    try
    {
        cl::ImageFormat format;
        format.image_channel_order = CL_R;
        format.image_channel_data_type = CL_UNSIGNED_INT32;

        cl_mem_flags flags = CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR;
        _tffPrefixMem = cl::Image1D(_contextCL, flags, format, tffPrefixSum.size(),
                                    tffPrefixSum.data());
    }
    catch (cl::Error err)
    {
        logCLerror(err);
    }
}

/**
 * @brief VolumeRenderCL::setCamOrtho
 * @param setCamOrtho
 */
void VolumeRenderCL::setCamOrtho(bool setCamOrtho)
{
    if (!this->hasData())
        return;

    try {
        _raycastKernel.setArg(ORTHO, (cl_uint)setCamOrtho);
    } catch (cl::Error err) { logCLerror(err); }
}


/**
 * @brief VolumeRenderCL::setIllumination
 * @param illum
 */
void VolumeRenderCL::setIllumination(unsigned int illum)
{
    try {
        _raycastKernel.setArg(ILLUMINATION, (cl_uint)illum);
    } catch (cl::Error err) { logCLerror(err); }
}


/**
 * @brief VolumeRenderCL::setAmbientOcclusion
 * @param illum
 */
void VolumeRenderCL::setAmbientOcclusion(bool ao)
{
    try {
        _raycastKernel.setArg(AO, (cl_uint)ao);
    } catch (cl::Error err) { logCLerror(err); }
}


/**
 * @brief VolumeRenderCL::setBoundingBox
 * @param boundingBox
 */
void VolumeRenderCL::setBoundingBox(bool boundingBox)
{
    try {
        _raycastKernel.setArg(BOX, (cl_uint)boundingBox);
    } catch (cl::Error err) { logCLerror(err); }
}


/**
 * @brief VolumeRenderCL::setLinearSampling
 * @param linearSampling
 */
void VolumeRenderCL::setLinearInterpolation(bool linearSampling)
{
    try {
        _raycastKernel.setArg(LINEAR, (cl_uint)linearSampling);
    } catch (cl::Error err) { logCLerror(err); }
}

/**
 * @brief VolumeRenderCL::setContours
 * @param contours
 */
void VolumeRenderCL::setContours(bool contours)
{
    try {
        _raycastKernel.setArg(CONTOURS, (cl_uint)contours);
    } catch (cl::Error err) { logCLerror(err); }
}

/**
 * @brief VolumeRenderCL::setAerial
 * @param aerial
 */
void VolumeRenderCL::setAerial(bool aerial)
{
    try {
        _raycastKernel.setArg(AERIAL, (cl_uint)aerial);
    } catch (cl::Error err) { logCLerror(err); }
}


/**
 * @brief VolumeRenderCL::setBackground
 * @param color
 */
void VolumeRenderCL::setBackground(std::array<float, 4> color)
{
    cl_float3 bgColor = {{color[0], color[1], color[2], color[3]}};
    try {
        _raycastKernel.setArg(BACKGROUND, bgColor);
    } catch (cl::Error err) { logCLerror(err); }
}

void VolumeRenderCL::setCursorPos(float xPos, float yPos)
{
	cl_float2 pos = { {xPos, yPos} };
	_raycastKernel.setArg(CURSOR_POS, pos);
}

void VolumeRenderCL::setRectangleExtends(float width, float height)
{
	cl_float2 ext = { {width, height} };
	_raycastKernel.setArg(RECTANGLE_EXTS, ext);
}

void VolumeRenderCL::setEllipse2(float width, float height)
{
	cl_float2 rx_ry = { { width, height } };
	_raycastKernel.setArg(ELLIPSE_2, rx_ry);
}

// False := 0, True := !False
void VolumeRenderCL::setInvert(unsigned int inv)
{
	cl_uint3 inverts = { inv, 0, 0 };
	_raycastKernel.setArg(INVERT, inverts);
}

void VolumeRenderCL::setInverts(cl_uint3 inverts)
{
	_raycastKernel.setArg(INVERT, inverts);
}

void VolumeRenderCL::setResolutionFactors(cl_float3 factor)
{
	_raycastKernel.setArg(RESOLUTIONFACTOR, factor);
}

void VolumeRenderCL::setMode(unsigned int mode)
{
	_raycastKernel.setArg(MODE, mode);
}

void VolumeRenderCL::setORS(unsigned int inv)
{
	_raycastKernel.setArg(ORS, inv);
}

void VolumeRenderCL::setInterpolationParametersForDDC(cl_float3 g_values, cl_float2 cursorPos, cl_float2 ell1, cl_float2 ell2)
{
	_interpolationKernelForDDC.setArg(2, g_values);
	_interpolationKernelForDDC.setArg(3, cursorPos);
	_interpolationKernelForDDC.setArg(4, ell1);
	_interpolationKernelForDDC.setArg(5, ell2);
}

void VolumeRenderCL::setInterpolationParametersForTRI(cl_float3 g_values, cl_float2 cursorPos, cl_float2 r1r2, cl_float2 irmr, int run)
{
	_interpolationKernelForTRI.setArg(2, g_values);
	_interpolationKernelForTRI.setArg(3, cursorPos);
	_interpolationKernelForTRI.setArg(4, r1r2);
	_interpolationKernelForTRI.setArg(5, irmr);
	_interpolationKernelForTRI.setArg(6, run);
}


/**
 * @brief VolumeRenderCL::getLastExecTime
 * @return
 */
double VolumeRenderCL::getLastExecTime()
{
    return _lastExecTime;
}

void VolumeRenderCL::runTRIMethod(const size_t texture_width, const size_t texture_height, const int t, cl_float2 cursorPos, cl_float3 go_gm_gi, cl_float2 r2r1, double imgSamplingRate, GLuint texIdLo, GLuint texIdLm, GLuint texIdLi, GLuint texIdIpLo, GLuint texIdIpLm)
{
	if (!this->_volLoaded)
		return;
	try // opencl scope
	{
		float r1_adjusted_to_sr = r2r1.y * imgSamplingRate;
		float r2_adjusted_to_sr = r2r1.x * imgSamplingRate;

		float mr = r2_adjusted_to_sr + 2 * go_gm_gi.x;
		float ir = r1_adjusted_to_sr + 2 * go_gm_gi.y;

		const size_t invocations_width_Lo = texture_width / go_gm_gi.x + 1;
		const size_t invocations_height_Lo = texture_height / go_gm_gi.x + 1;

		const size_t invocations_width_and_height_Lm = 2 * mr / go_gm_gi.y;

		const size_t invocations_width_and_height_Li = 2 * ir;

		// Images raycast
		cl::ImageGL imgLo; 
		cl::ImageGL imgLm;
		cl::ImageGL imgLi;

		cl::Event ndrEvtr0;
		cl::Event ndrEvtr1;
		cl::Event ndrEvtr2;
		cl::Event ndrEvti0;
		cl::Event ndrEvti1;

		size_t lDim = 8;    // local work group dimension
		
		{	// set Mem Objects and Arguments which are the same for all raycastKernel runs
			_raycastKernel.setArg(VOLUME, _volumesMem.at(t));
			_raycastKernel.setArg(BRICKS, _bricksMem.at(t));
			_raycastKernel.setArg(TFF, _tffMem);
			_raycastKernel.setArg(TFF_PREFIX, _tffPrefixMem);
			cl_float3 modelScale = { _modelScale[0], _modelScale[1], _modelScale[2] };
			_raycastKernel.setArg(MODEL_SCALE, modelScale);
			
			_raycastKernel.setArg(CURSOR_POS, cursorPos);
			_raycastKernel.setArg(RESOLUTIONFACTOR, go_gm_gi);
			_raycastKernel.setArg(RECTANGLE_EXTS, cl_float2{ r1_adjusted_to_sr , r2_adjusted_to_sr });
			_raycastKernel.setArg(ELLIPSE_2, cl_float2{mr, ir});
		
			{ // images
				imgLo = cl::ImageGL(_contextCL, CL_MEM_WRITE_ONLY, GL_TEXTURE_2D, 0, texIdLo);
				imgLm = cl::ImageGL(_contextCL, CL_MEM_WRITE_ONLY, GL_TEXTURE_2D, 0, texIdLm);
				imgLi = cl::ImageGL(_contextCL, CL_MEM_WRITE_ONLY, GL_TEXTURE_2D, 0, texIdLi);
			}
		}
		
		{ // raycasts for outer, middle and inner layer on different images
			{ // Lo
				cl::NDRange globalThreads_OuterLayer(invocations_width_Lo + (lDim - invocations_width_Lo % lDim), invocations_height_Lo + (lDim - invocations_height_Lo % lDim));
				cl::NDRange localThreads_OuterLayer(lDim, lDim);

				_raycastKernel.setArg(OUTPUT, imgLo);
				_raycastKernel.setArg(INVERT, cl_uint3{ 0, 0, 0 });

				std::vector<cl::Memory> memObj;
				memObj.push_back(imgLo);
				memObj.push_back(_inputMem);
				_queueCL.enqueueAcquireGLObjects(&memObj);
				_queueCL.enqueueNDRangeKernel(
					_interpolationKernelForDDC, cl::NullRange, globalThreads_OuterLayer, localThreads_OuterLayer, NULL, &ndrEvtr0);
				_queueCL.enqueueReleaseGLObjects(&memObj);
			}

			{ // Lm
				cl::NDRange globalThreads_MiddleLayer(invocations_width_and_height_Lm + (lDim - invocations_width_and_height_Lm % lDim), invocations_width_and_height_Lm + (lDim - invocations_width_and_height_Lm % lDim));
				cl::NDRange localThreads_MiddleLayer(lDim, lDim);

				_raycastKernel.setArg(OUTPUT, imgLm);
				_raycastKernel.setArg(INVERT, cl_uint3{ 1, 0, 0 });

				std::vector<cl::Memory> memObj;
				memObj.push_back(imgLm);
				memObj.push_back(_inputMem);
				_queueCL.enqueueAcquireGLObjects(&memObj);
				_queueCL.enqueueNDRangeKernel(
					_interpolationKernelForDDC, cl::NullRange, globalThreads_MiddleLayer, localThreads_MiddleLayer, NULL, &ndrEvtr1);
				_queueCL.enqueueReleaseGLObjects(&memObj);
			}

			{ // Li
				cl::NDRange globalThreads_InnerLayer(invocations_width_and_height_Li + (lDim - invocations_width_and_height_Li % lDim), invocations_width_and_height_Li + (lDim - invocations_width_and_height_Li % lDim));
				cl::NDRange localThreads_InnerLayer(lDim, lDim);

				_raycastKernel.setArg(OUTPUT, imgLi);
				_raycastKernel.setArg(INVERT, cl_uint3{ 2, 0, 0 });

				std::vector<cl::Memory> memObj;
				memObj.push_back(imgLi);
				memObj.push_back(_inputMem);
				_queueCL.enqueueAcquireGLObjects(&memObj);
				_queueCL.enqueueNDRangeKernel(
					_interpolationKernelForDDC, cl::NullRange, globalThreads_InnerLayer, localThreads_InnerLayer, NULL, &ndrEvtr2);
				_queueCL.enqueueReleaseGLObjects(&memObj);
			}
		}

		_queueCL.finish();    // global sync after raycasts

		// Images interpolation
		cl::ImageGL imgIpLo;
		cl::ImageGL imgIpLm;

		{ // set interpolation parameters for all interpolationKernel runs
			_interpolationKernelForTRI.setArg(2, go_gm_gi);
			_interpolationKernelForTRI.setArg(3, cursorPos);
			_interpolationKernelForTRI.setArg(4, cl_float2{ r1_adjusted_to_sr, r2_adjusted_to_sr });
			_interpolationKernelForTRI.setArg(5, cl_float2{ ir, mr });
		}

		{ // interpolation for outer, middle and inner layer on different images
			{ // Lo
				_interpolationKernelForTRI.setArg(6, 0);

				cl::NDRange globalThreads_Ip_Lo(texture_width + (lDim - texture_width % lDim), texture_height + (lDim - texture_height % lDim));
				cl::NDRange localThreads_Ip_Lo(lDim, lDim);

				{ // set Mem Lo
					_inputMem = cl::ImageGL(_contextCL, CL_MEM_READ_ONLY, GL_TEXTURE_2D, 0, texIdLo);
					imgIpLo = cl::ImageGL(_contextCL, CL_MEM_WRITE_ONLY, GL_TEXTURE_2D, 0, texIdIpLo);

					_interpolationKernelForTRI.setArg(0, _inputMem); // in Data
					_interpolationKernelForTRI.setArg(1, imgIpLo);	// out Data
				}

				std::vector<cl::Memory> memObj;
				memObj.push_back(imgIpLo);
				memObj.push_back(_inputMem);
				_queueCL.enqueueAcquireGLObjects(&memObj);
				_queueCL.enqueueNDRangeKernel(
					_interpolationKernelForTRI, cl::NullRange, globalThreads_Ip_Lo, localThreads_Ip_Lo, NULL, &ndrEvti0);
				_queueCL.enqueueReleaseGLObjects(&memObj);
			}

			{ // Lm
				_interpolationKernelForTRI.setArg(6, 1);

				const size_t invocations_ip_width_and_height_lm = 2 * mr;

				cl::NDRange globalThreads_Ip_Lm(invocations_ip_width_and_height_lm + (lDim - invocations_ip_width_and_height_lm % lDim), invocations_ip_width_and_height_lm + (lDim - invocations_ip_width_and_height_lm % lDim));
				cl::NDRange localThreads_Ip_Lm(lDim, lDim);

				{ // set Mem Lm
					_inputMem = cl::ImageGL(_contextCL, CL_MEM_READ_ONLY, GL_TEXTURE_2D, 0, texIdLm);
					imgIpLm = cl::ImageGL(_contextCL, CL_MEM_WRITE_ONLY, GL_TEXTURE_2D, 0, texIdIpLm);

					_interpolationKernelForTRI.setArg(0, _inputMem); // in Data
					_interpolationKernelForTRI.setArg(1, imgIpLm);	// out Data
				}

				std::vector<cl::Memory> memObj;
				memObj.push_back(imgIpLm);
				memObj.push_back(_inputMem);
				_queueCL.enqueueAcquireGLObjects(&memObj);
				_queueCL.enqueueNDRangeKernel(
					_interpolationKernelForTRI, cl::NullRange, globalThreads_Ip_Lm, localThreads_Ip_Lm, NULL, &ndrEvti1);
				_queueCL.enqueueReleaseGLObjects(&memObj);
			}
			
		}

		_queueCL.finish();    // global sync after interpolations

		/*
		Now there are three images, imgIpLo, imgIpLm and ImgLi that have to be blended together.
		*/

		{
			// trying in fragmentshader for now
		}

#ifdef CL_QUEUE_PROFILING_ENABLE
		cl_ulong start = 0;
		cl_ulong end = 0;
		ndrEvtr0.getProfilingInfo(CL_PROFILING_COMMAND_START, &start);
		ndrEvtr0.getProfilingInfo(CL_PROFILING_COMMAND_END, &end);
		_lastExecTime = static_cast<double>(end - start)*1e-9;
		//        std::cout << "Kernel time: " << _lastExecTime << std::endl << std::endl;
#endif
	}
	catch (cl::Error err)
	{
		logCLerror(err);
	}
}

void VolumeRenderCL::runInterpolationForDDC(const size_t width, const size_t height, GLuint inTexId, GLuint outTexId)
{
	if (!this->_volLoaded)
		return;
	try // opencl scope
	{
		setMemObjectsInterpolation(inTexId, outTexId);
		size_t lDim = 8;    // local work group dimension
		cl::NDRange globalThreads(width + (lDim - width % lDim), height + (lDim - height % lDim));
		cl::NDRange localThreads(lDim, lDim);
		cl::Event ndrEvt;

		std::vector<cl::Memory> memObj;
		memObj.push_back(_outputMem);
		memObj.push_back(_inputMem);
		_queueCL.enqueueAcquireGLObjects(&memObj);
		_queueCL.enqueueNDRangeKernel(
			_interpolationKernelForDDC, cl::NullRange, globalThreads, localThreads, NULL, &ndrEvt);
		_queueCL.enqueueReleaseGLObjects(&memObj);
		_queueCL.finish();    // global sync

#ifdef CL_QUEUE_PROFILING_ENABLE
		cl_ulong start = 0;
		cl_ulong end = 0;
		ndrEvt.getProfilingInfo(CL_PROFILING_COMMAND_START, &start);
		ndrEvt.getProfilingInfo(CL_PROFILING_COMMAND_END, &end);
		_lastExecTime = static_cast<double>(end - start)*1e-9;
		//        std::cout << "Kernel time: " << _lastExecTime << std::endl << std::endl;
#endif
	}
	catch (cl::Error err)
	{
		logCLerror(err);
	}
}

void VolumeRenderCL::runInterpolationForTRI(const size_t width, const size_t height, GLuint inTexId, GLuint outTexId)
{
	if (!this->_volLoaded)
		return;
	try // opencl scope
	{
		setMemObjectsInterpolation(inTexId, outTexId);
		size_t lDim = 8;    // local work group dimension
		cl::NDRange globalThreads(width + (lDim - width % lDim), height + (lDim - height % lDim));
		cl::NDRange localThreads(lDim, lDim);
		cl::Event ndrEvt;

		std::vector<cl::Memory> memObj;
		memObj.push_back(_outputMem);
		memObj.push_back(_inputMem);
		_queueCL.enqueueAcquireGLObjects(&memObj);
		_queueCL.enqueueNDRangeKernel(
			_interpolationKernelForTRI, cl::NullRange, globalThreads, localThreads, NULL, &ndrEvt);
		_queueCL.enqueueReleaseGLObjects(&memObj);
		_queueCL.finish();    // global sync

#ifdef CL_QUEUE_PROFILING_ENABLE
		cl_ulong start = 0;
		cl_ulong end = 0;
		ndrEvt.getProfilingInfo(CL_PROFILING_COMMAND_START, &start);
		ndrEvt.getProfilingInfo(CL_PROFILING_COMMAND_END, &end);
		_lastExecTime = static_cast<double>(end - start)*1e-9;
		//        std::cout << "Kernel time: " << _lastExecTime << std::endl << std::endl;
#endif
	}
	catch (cl::Error err)
	{
		logCLerror(err);
	}
}


/**
 * @brief VolumeRenderCL::getPlatformNames
 * @return
 */
const std::vector<std::string> VolumeRenderCL::getPlatformNames()
{
    std::vector<std::string> names;
    try
    {
        std::vector<cl::Platform> platforms;

        cl::Platform::get(&platforms);
        for(unsigned int i = 0; i < platforms.size(); ++i)
            names.push_back(platforms[i].getInfo<CL_PLATFORM_NAME>());
    }
    catch (cl::Error err) {
        logCLerror(err);
    }
    return names;
}

/**
 * @brief VolumeRenderCL::getDeviceNames
 * @param platformId
 * @param type
 * @return
 */
const std::vector<std::string> VolumeRenderCL::getDeviceNames(int platformId,
                                                              const std::string &type)
{
    std::vector<std::string> names;
    cl_device_type t = CL_DEVICE_TYPE_ALL;
    if (type == "GPU") t = CL_DEVICE_TYPE_GPU;
    else if (type == "CPU") t = CL_DEVICE_TYPE_CPU;
    try
    {
        std::vector<cl::Platform> platforms;
        cl::Platform::get(&platforms);
        std::vector<cl::Device> devices;
        platforms[platformId].getDevices(t, &devices);

        for(unsigned int i = 0; i < devices.size(); ++i)
            names.push_back(devices[i].getInfo<CL_DEVICE_NAME>());
    }
    catch (cl::Error err) {
        logCLerror(err);
    }

    return names;
}
