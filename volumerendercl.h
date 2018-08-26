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
#pragma once

#define CL_QUEUE_PROFILING_ENABLE
#define CL_HPP_ENABLE_EXCEPTIONS
#include "openclglutilities.h"
#include "openclutilities.h"

#include "datrawreader.h"

#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <qopenglfunctions_4_3_core.h>

#include <valarray>

class VolumeRenderCL
{
public:
    enum kernel_arg
    {
          VOLUME     = 0 // volume data set                          image3d_t
        , BRICKS     = 1 // low resolution brick volume              image3d_t
        , TFF        = 2 // transfer function array                  image1d_t
        , OUTPUT         // output image                             image2d_t
        , SAMPLING_RATE  // step size factor                         cl_float
        , VIEW           // view matrix                              float16
        , ORTHO          // use orthographic camera                  cl_uint (bool)
        , ILLUMINATION   // use illumination                         cl_uint
        , BOX            // show bounding box aroud volume           cl_uint (bool)
        , LINEAR         // use linear interpolation, not nearest    cl_uint (bool)
        , BACKGROUND     // background color RGBA                    cl_float4
        , TFF_PREFIX     // prefix sum of transfer function          image1d_t
        , AO             // use ambient occlusion                    cl_uint (bool)
        , MODEL_SCALE    // model scaling factor                     cl_float3
        , CONTOURS       // show contour lines                       cl_uint (bool)
        , AERIAL         // use aerial perspective                   cl_uint (bool)
		, CURSOR_POS	 // cursor pos								 cl_float2
		, RECTANGLE_EXTS // rectangle extends, also used for ell1	 cl_float2
		, ELLIPSE_2		 // ellipse 2								 cl_float2
		, INVERT		 // invert									 cl_uint3 (bool), max 1d-index of each area in mode 1
		, RESOLUTIONFACTOR // resolution factor						 cl_float3
		, MODE			// modus									 cl_uint
	};

    // mipmap down-scaling metric
    enum scaling_metric
    {
        MIN = 0,
        MAX,
        AVG,
        DENSITY,
    };

    VolumeRenderCL();

    ~VolumeRenderCL();

    /**
     * @brief Initialize the volume raycaster, i.e. the OpenCL context, queue and kernel.
     */
    void initialize(bool useGL = false, bool useCPU = false, cl_vendor vendor = VENDOR_ANY);

    /**
     * @brief Update the view matrix argument for the raycasting kernel.
     * @param viewMat the 4x4 transposed view matrix.
     */
    void updateView(const std::array<float, 16> viewMat);

    /**
     * @brief Update the integration step size factor kernel argument for the volume raycast.
     * @param samplingRate the sampling rate relative per voxel.
     */
    void updateSamplingRate(const double samplingRate);

    /**
     * @brief Update the output image kernel argument and vector size.
     * @param width The image width in pixels.
     * @param height The image height in pixels.
     */
    void updateOutputImg(const size_t width, const size_t height, cl_GLuint texId, cl_mem_flags flags = CL_MEM_WRITE_ONLY);

    /**
     * @brief Run the actual OpenCL volume raycasting kernel.
     * @param width The image width in pixels, used as one dimension of the global thread size.
     * @param height The image height in pixels, used as one dimension of the global thread size.
     * @param t time series id, defaults to 0 if no time series
     */
     void runRaycast(const size_t width, const size_t height, const int t = 0);

     /**
      * @brief Run the actual OpenCL volume raycasting kernel without OpenGL context shring.
      * @param width The image width in pixels, used as one dimension of the global thread size.
      * @param height The image height in pixels, used as one dimension of the global thread size.
      * @param t time series id, defaults to 0 if no time series
      * @param pixel color output data of the frame
      */
     void runRaycastNoGL(const size_t width, const size_t height, const int t,
                         std::vector<float> &output);

    /**
     * @brief Load volume data from a given .dat file name.
     * @param fileName The full path to the volume data file.
     * @return number of loaded volume time steps
     */
    int loadVolumeData(const std::string fileName);

    /**
     * @brief Answers if volume data has been loaded.
     * @return true, if volume data has been loaded, false otherwise.
     */
    bool hasData();

    /**
     * @brief getResolution
     * @return
     */
    const std::array<unsigned int, 3> getResolution() const;

    /**
     * @brief Set the transfer function for the volume raycast as a kernel argument.
     * @param tff a vector of the RGBA transfer function values.
     * @param rangeMin clamp range to minimum
     * @param rangeMax clamp range to maximum
     */
    void setTransferFunction(std::vector<unsigned char> &tff);

    /**
     * @brief Set the prefix sum of the transfer function.
     * @param tffPrefixSum The prefix sum as vector of unsigned ints.
     */
    void setTffPrefixSum(std::vector<unsigned int> &tffPrefixSum);

    /**
     * @brief VolumeRenderCL::scaleVolume
     * @param scale
     */
    void scaleVolume(std::valarray<float> scale);

    /**
     * @brief buildScaledVol
     * @param factor
     * @param metric
     * @param useTff
     * @return
     */
    cl::Image3D buildScaledVol(std::array<unsigned int, 3> factor,
                               scaling_metric metric, bool useTff, cl::Image3D volData);

    // TODO: set background / toggle black/white

    /**
     * @brief setCamOrtho
     * @param setCamOrtho
     */
    void setCamOrtho(bool setCamOrtho);
    /**
     * @brief setIllumination
     * @param illum
     */
    void setIllumination(unsigned int illum);
    /**
     * @brief setBoundingBox
     * @param boundingBox
     */
    void setBoundingBox(bool boundingBox);
    /**
     * @brief setLinearSampling
     * @param linearSampling
     */
    void setLinearInterpolation(bool linearSampling);
    /**
     * @brief setContours
     * @param contours
     */
    void setContours(bool contours);
    /**
     * @brief setAerial
     * @param aerial
     */
    void setAerial(bool aerial);
    /**
     * @brief setBackground
     * @param color
     */
    void setBackground(std::array<float, 4> color);
	/**
	* @brief setCursorPos normalized
	* @param xPos
	* @param yPos
	*/
	void setCursorPos(float xPos, float yPos);
	/**
	* @brief setRectangleExtends normalized
	* @param color
	*/
	void setRectangleExtends(float width, float height);
	/**
	* @brief set ellipse 2
	* @param color
	*/
	void setEllipse2(float width, float height);
	/**
	* @brief setInvert
	* @param color
	*/
	void setInvert(unsigned int inv);
	/*
	sets inverts for areas max index
	*/
	void setInverts(cl_uint3 inverts);
	/**
	* @brief setResolutionFactor
	* @param factor
	*/
	void setResolutionFactors(cl_float3 factor);
	/**
	* @brief setMode, 0 = Standard, 1 = dc with rect, 2 = sin res, else = standard
	* @param mode
	*/
	void setMode(unsigned int mode);

	/*
	* Sets interpolation parameters for mode 1.
	*/
	void setInterpolationParametersForDDC(cl_float3 g_values, cl_float2 cursorPos, cl_float2 ell1, cl_float2 ell2);

	/*
	* Sets interpolation parameters for mode 3.
	*/
	void setInterpolationParametersForTRI(cl_float3 g_values, cl_float2 cursorPos, cl_float2 r1r2, cl_float2 irmr, int run);

    /**
     * @brief Get the execution time of the last kernel run.
     * @return The kernel runtime in seconds.
     */
    double getLastExecTime();

	/*
	*  Runs the TRI Method which create three images and interpolates all of them to the native display resolution.
	*  Then it blends those three images together to create a smooth full resolution image
	*/
	void runTRIMethod(const size_t texture_width, const size_t texture_height, const int t, cl_float2 cursorPos, cl_float3 go_gm_gi, cl_float2 r2r1, double imgSamplingRate, GLuint texUnitLo, GLuint texUnitLm, GLuint texUnitLi);

	/**
	*  Run the kernel to interpolate the values from distance_dc
	*/
	void runInterpolationForDDC(const size_t width, const size_t height, GLuint inTexId, GLuint outTexId);

	/**
	*  Run the kernel to interpolate the values from TRI
	*/
	void runInterpolationForTRI(const size_t width, const size_t height, GLuint inTexId, GLuint outTexId);

    /**
     * @brief getPlatformNames
     * @return platform names
     */
    const std::vector<std::string> getPlatformNames();

    /**
     * @brief getDeviceNames
     * @param platformId OpenCL platform id
     * @param type string describing the OpenCL device type ('GPU' or 'CPU')
     * @return device names
     */
    const std::vector<std::string> getDeviceNames(int platformId, const std::string &type);

    /**
     * @brief setAmbientOcclusion
     * @param ao
     */
    void setAmbientOcclusion(bool ao);

    /**
     * @brief Generate a downsampling of the currently loaded volume file.
     * @param t Timestep
     * @param factor downsampling factor, uniform for all 3 dimensions
     */
    const std::string volumeDownsampling(const int t, const int factor);

	cl::ImageGL getOutputMemGL(); // Returns a copy of the local _outputMem using the copy constructor

private:
    /**
     * @brief Generate coarse grained volume bricks that can be used for ESS.
     * @param volumeData
     */
    void generateBricks();

    /**
     * @brief Calculate the scaling vector for the volume data.
     */
    void calcScaling();

    /**
     * @brief volDataToCLmem
     * @param volumeData
     */
    void volDataToCLmem(const std::vector<std::vector<char> > &volumeData);

    /**
     *
     */
    template<class T>
    void volDataToCLmem(const std::vector<char> &volumeData)
    {
        // reinterpret raw data (char) to input format
        auto s = reinterpret_cast<const T *>(volumeData.data());
        auto e = reinterpret_cast<const T *>(volumeData.data() + volumeData.size());
        // convert imput vector to the desired output precision
        std::vector<unsigned char> convertedData(s, e);
        try
        {
            cl::ImageFormat format;
            format.image_channel_order = CL_R;
            format.image_channel_data_type = CL_UNORM_INT8;

            _volumesMem = cl::Image3D(_contextCL,
                                     CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                     format,
                                     _dr.properties().volume_res[0],
                                     _dr.properties().volume_res[1],
                                     _dr.properties().volume_res[2],
                                     0, 0,
                                     (void *)convertedData.data());
        }
        catch (cl::Error err)
        {
            throw std::runtime_error( "ERROR: " + std::string(err.what()) + "("
                                      + getCLErrorString(err.err()) + ")");
        }
    }

    void setMemObjectsRaycast(const int t);
    void setMemObjectsBrickGen(const int t);
	void setMemObjectsInterpolation(GLuint inTexId, GLuint outTexId);

    void initKernel(const std::string fileName, const std::string buildFlags = "");

    void logCLerror(cl::Error err);

    cl::Context _contextCL;
    cl::CommandQueue _queueCL;
    cl::Kernel _raycastKernel;
    cl::Kernel _genBricksKernel;
    cl::Kernel _downsamplingKernel;
	cl::Kernel _interpolationKernelForDDC;
	cl::Kernel _interpolationKernelForTRI;

    std::vector<cl::Image3D> _volumesMem;
    std::vector<cl::Image3D> _bricksMem;
    cl::ImageGL _outputMem;
	cl::ImageGL _inputMem; // used for mode 1.
    cl::ImageGL _overlayMem;
    cl::Image1D _tffMem;
    cl::Image1D _tffPrefixMem;
    cl::Image2D _outputMemNoGL;

    bool _volLoaded;
    double _lastExecTime;
    std::valarray<float> _modelScale;
    bool _useGL;

    DatRawReader _dr;
};
