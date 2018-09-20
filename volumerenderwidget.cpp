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

#include "volumerenderwidget.h"

#include <QPainter>
#include <QGradient>
#include <QImage>
#include <QCoreApplication>
#include <QScreen>
#include <QInputDialog>
#include <QJsonObject>
#include <QErrorMessage>
#include <QLoggingCategory>
#include <QMessageBox>

const static double Z_NEAR = 1.0;
const static double Z_FAR = 500.0;

/**
 * @brief VolumeRenderWidget::VolumeRenderWidget
 * @param parent
 */
VolumeRenderWidget::VolumeRenderWidget(QWidget *parent)
	: QOpenGLWidget(parent)
	, _tffRange(QPoint(0, 255))
	, _timestep(0)
	, _lastLocalCursorPos(QPoint(0, 0))
	, _rotQuat(QQuaternion(1, 0, 0, 0))
	, _translation(QVector3D(0, 0, 2.0))
	, _noUpdate(true)
	, _loadingFinished(false)
	, _writeImage(false)
	, _recordVideo(false)
	, _imgCount(0)
	, _imgSamplingRate(1)
	, _useGL(true)
	, _showOverlay(true)
	, _renderingMethod(STANDARD)
	, _rect_extends({ 350, 350 })
	, _g_values({ 4.0f, 2.0f, 1.0f })
	, _innerEllipse({ 0.3, 0.2 })
	, _outerEllipse({ 0.5, 0.4 })
	, _circle_radiuses({ 100.0f, 400.0f })
	, _eyetracker(nullptr)
	, _useEyetracking(false)
	, _measurement_is_active(false)
	, _single_measurement(false)
{
	this->grabKeyboard();
    this->setMouseTracking(true);
}


/**
 * @brief VolumeRenderWidget::~VolumeRenderWidget
 */
VolumeRenderWidget::~VolumeRenderWidget()
{
	this->releaseKeyboard();
}


/**
 * @brief VolumeRenderWidget::paintOrientationAxis
 */
void VolumeRenderWidget::paintOrientationAxis(QPainter &p)
{
    QMatrix4x4 proj;
    proj.perspective(53.14f, 1.0f, 0.1f, 1.0);
    QMatrix4x4 viewProj(proj * _coordViewMX);
    QVector4D x =         viewProj * QVector4D( 20,  0,  0, 0);
    QVector4D xArrLeft =  viewProj * QVector4D( 16, -2,  0, 0);
    QVector4D xArrRight = viewProj * QVector4D( 16, +2,  0, 0);
    QVector4D y =         viewProj * QVector4D(  0, 20,  0, 0);
    QVector4D yArrLeft =  viewProj * QVector4D( -2, 16,  0, 0);
    QVector4D yArrRight = viewProj * QVector4D( +2, 16,  0, 0);
    QVector4D z =         viewProj * QVector4D(  0,  0, 20, 0);
    QVector4D zArrLeft =  viewProj * QVector4D( -2,  0, 16, 0);
    QVector4D zArrRight = viewProj * QVector4D( +2,  0, 16, 0);

    p.resetTransform();
    p.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    p.translate(66, height() - 66);
    int textOffset = 5;
    // x axis
    p.setPen(Qt::red);
    p.drawLine(0, 0, x.x(), x.y());
    p.drawLine(xArrLeft.x(), xArrLeft.y(), x.x(), x.y());
    p.drawLine(xArrRight.x(), xArrRight.y(), x.x(), x.y());
    p.drawText(x.x() + textOffset, x.y() + textOffset, "x");
    // y axis
    p.setPen(Qt::green);
    p.drawLine(0, 0, y.x(), y.y());
    p.drawLine(yArrLeft.x(), yArrLeft.y(), y.x(), y.y());
    p.drawLine(yArrRight.x(), yArrRight.y(), y.x(), y.y());
    p.drawText(y.x() + textOffset, y.y() + textOffset, "y");
    // z axis
    p.setPen(Qt::blue);
    p.drawLine(0, 0, z.x(), z.y());
    p.drawLine(zArrLeft.x(), zArrLeft.y(), z.x(), z.y());
    p.drawLine(zArrRight.x(), zArrRight.y(), z.x(), z.y());
    p.drawText(z.x() + textOffset, z.y() + textOffset, "z");
}


/**
 * @brief paintFPS
 */
void VolumeRenderWidget::paintFPS(QPainter &p, const double fps, const double lastTime)
{
    p.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    p.setPen(Qt::darkGreen);
    p.setFont(QFont("Helvetica", 11));
    QString s = "FPS: " + QString::number(fps);
    p.drawText(10, 20, s);
    s = "Last: " + QString::number(lastTime);
    p.drawText(10, 36, s);
}


/**
 * @brief VolumeRenderWidget::initializeGL
 */
void VolumeRenderWidget::initializeGL()
{
    connect( context(), &QOpenGLContext::aboutToBeDestroyed, this,
             &VolumeRenderWidget::cleanup );

    initializeOpenGLFunctions();
    makeCurrent();

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    _spScreenQuad.addShaderFromSourceCode(QOpenGLShader::Vertex, VolumeRenderWidget::ReadFile("shaders/vertex_shader.glsl").c_str());
    _spScreenQuad.addShaderFromSourceCode(QOpenGLShader::Fragment, VolumeRenderWidget::ReadFile("shaders/fragment_shader.glsl").c_str());
    _spScreenQuad.bindAttributeLocation("vertex", 0);
    _spScreenQuad.link();

    _spScreenQuad.bind();
    _screenQuadVao.create();
    _screenQuadVao.bind();

    const int numQuadVertices = 8;
    GLfloat quadVertices[numQuadVertices] =
    {
        -1.0f, -1.0f,
        +1.0f, -1.0f,
        -1.0f, +1.0f,
        +1.0f, +1.0f
    };

    // Setup vertex buffer object.
    _quadVbo.create();
    _quadVbo.bind();
    _quadVbo.allocate( quadVertices, numQuadVertices * sizeof( GLfloat ) );
    _quadVbo.release();
    // Store the vertex attribute bindings for the program.
    setupVertexAttribs();
    _viewMX.setToIdentity();
    _viewMX.translate( 0.0f, 0.0f, -1.0f );
    // set quad model matrix
    _modelMX.setToIdentity();
    _modelMX.rotate( 180.0f, 1, 0, 0 );
    _spScreenQuad.release();
    _screenQuadVao.release();

	// create GL output Texture Object
	glGenTextures(1, &_outTexId0);
	glGenTextures(1, &_outTexId1);

    try
    {
        _volumerender.initialize(true, false);
    }
    catch (std::invalid_argument e)
    {
        qCritical() << e.what();
    }
    catch (std::runtime_error e)
    {
        qCritical() << e.what() << "\nSwitching to CPU fallback mode.";
        _useGL = false;
        _volumerender.initialize(false, true);
    }
    catch (...)
    {
        qCritical() << "An unknown error occured initializing OpenCL/OpenGL.";
    }
}

/*
	sets g_values for area c, b and a but each at least 1.0f.
*/
void VolumeRenderWidget::setGValues(float c, float b, float a)
{
	_g_values.x = c >= 1.0f ? c : 1.0f;
	_g_values.y = b >= 1.0f ? b : 1.0f;
	_g_values.z = a >= 1.0f ? a : 1.0f;
}

void VolumeRenderWidget::setInnerEllipseExtends(float rx, float ry)
{
	_innerEllipse = { rx, ry };
}

void VolumeRenderWidget::setOuterEllipseExtends(float rx, float ry)
{
	_outerEllipse = { rx, ry };
}

void VolumeRenderWidget::showGValuesSettings()
{
	bool ok;
	bool ok2;
	int g_c = QInputDialog::getInt(this, tr("Set G_Values:"), tr("Area C:"), 4, 1, std::numeric_limits<int>::max(), 1, &ok);
	int g_b = QInputDialog::getInt(this, tr("Set G_Values:"), tr("Area B:"), 2, 1, std::numeric_limits<int>::max(), 1, &ok2);

	if (ok && ok2) {
		setGValues(static_cast<float>(g_c), static_cast<float>(g_b));
	}
	else {
		qCritical() << "Failed to set G_Values!";
	}

}

void VolumeRenderWidget::showinnerEllipseSettings()
{
	bool ok;
	bool ok2;
	double rx = QInputDialog::getDouble(this, tr("Set Inner Ellipse:"), tr("rx in percent:"), 30, 0.0, 100.0, 2, &ok) * 0.01;
	double ry = QInputDialog::getDouble(this, tr("Set Inner Ellipse:"), tr("ry in percent:"), 20, 0.0, 100.0, 2, &ok2) * 0.01;

	if (ok && ok2) {
		setInnerEllipseExtends(static_cast<float>(rx), static_cast<float>(ry));
	}
	else {
		qCritical() << "Failed to set inner Ellipse extends!";
	}
}

void VolumeRenderWidget::showouterEllipseSettings()
{
	bool ok;
	bool ok2;
	double rx = QInputDialog::getDouble(this, tr("Set Outer Ellipse:"), tr("rx in percent:"), 50, 0.0, 100.0, 2, &ok) * 0.01;
	double ry = QInputDialog::getDouble(this, tr("Set Outer Ellipse:"), tr("ry in percent:"), 40, 0.0, 100.0, 2, &ok2) * 0.01;

	if (ok && ok2) {
		setOuterEllipseExtends(static_cast<float>(rx), static_cast<float>(ry));
	}
	else {
		qCritical() << "Failed to set outer Ellipse extends!";
	}
}


void VolumeRenderWidget::setRectExtends(int width, int height)
{
	_rect_extends = { width, height };
}

void VolumeRenderWidget::showRectExtendsSettings()
{
	bool ok;
	bool ok2;
	int width = QInputDialog::getInt(this, tr("Set Rectangle Extends:"), tr("width:"), 350, 0, std::numeric_limits<int>::max(), 1, &ok);
	int height = QInputDialog::getInt(this, tr("Set Rectangle Extends:"), tr("height:"), 350, 0, std::numeric_limits<int>::max(), 1, &ok2);

	if (ok && ok2) {
		setRectExtends(width, height);
	}
	else {
		qCritical() << "Failed to set Rectangle Extends!";
	}
}


/**
 * @brief VolumeRenderWidget::saveFrame
 */
void VolumeRenderWidget::saveFrame()
{
    _writeImage = true;
    update();
}

/**
 * @brief VolumeRenderWidget::toggleVideoRecording
 */
void VolumeRenderWidget::toggleVideoRecording()
{
    qInfo() << (_recordVideo ? "Stopped recording." : "Started recording.");

    _recordVideo = !_recordVideo;
    _writeImage = true;
    update();
}

/**
 * @brief VolumeRenderWidget::setTimeStep
 * @param timestep
 */
void VolumeRenderWidget::setTimeStep(int timestep)
{
    _timestep = timestep;
    update();
}

/**
 * @brief VolumeRenderWidget::setImageSamplingRate
 * @param samplingRate
 */
void VolumeRenderWidget::setImageSamplingRate(const double samplingRate)
{
    _imgSamplingRate = samplingRate;
    this->resizeGL(this->width(), this->height());
}

/**
 * @brief VolumeRenderWidget::paintGL
 */
void VolumeRenderWidget::paintGL()
{
	// sets an uniform for the fragment shader to distinguish the rendering methods
	
	{
		// collect mouse data
		if (_collect_mouse_movement) {
			// don't update
			return;
		}
	}

	{
		// replay mouse data
		if (_measure_with_collected_mouse_movement_data) {
			if (_mouse_mv_data.size() > _mouse_mv_data_index) {
				_lastLocalCursorPos = _mouse_mv_data.at(_mouse_mv_data_index).mouse_pos;
				_mouse_mv_data_index++;	// increase the index so that the next frame is computed with the location of the next measurment
				_take_measurement = true; // take a measurement for this frame
				// std::cout << "currently replaying." << std::endl;
			}
			else {
				// all measurments were taken, stop updating
				save_measurements(); // save measurements
				_measured_data.clear();
				_mouse_mv_data_index = 0; // reset index
				_measure_with_collected_mouse_movement_data = false; // stops the loop and enables mouse and other key events. has to be after save_measurements() because its value is used there.
				std::cout << "mouse replay finished." << std::endl;
			}
			
		}
	}

	{	// measurements
		_time = QTime::currentTime();
		_tmp_ms.system_time = _time.toString(Qt::ISODateWithMs);
	}

	{
		_spScreenQuad.bind();
		_spScreenQuad.setUniformValue(_spScreenQuad.uniformLocation("mode"), static_cast<GLint>(_renderingMethod));
		_spScreenQuad.release();
	}
		_volumerender.setMode(static_cast<unsigned int>(_renderingMethod));
	// std::cout << "paintGL(): " << "_renderingMethod: " << static_cast<GLint>(_renderingMethod) << std::endl;

	switch (_renderingMethod) {
	case DISTANCE_DC:
		paintGL_distance_dc();
		break;
	case SQUARE_DC:
		paintGL_square_dc();
		break;
	case TRI:
		paintGL_ThreeRenderInvocations();
		break;
	default: // case STANDARD:
		_volumerender.setCursorPos(0.f, 0.f);
		_volumerender.setRectangleExtends(0.f, 0.f);
		_volumerender.setInvert(true);
		paintGL_standard();
		break;
	}

	// std::cout << "paintGL got called.\n";

	{
		// measurments
		_tmp_ms.elapsed_millisecond_during_paintGL = _time.elapsed();
		if (_take_measurement) {
			_measured_data.push_back(_tmp_ms);
		}

		_take_measurement = false;
	}
	
	{
		// mouse replay
		if (_measure_with_collected_mouse_movement_data) {
			if (_mouse_mv_data.size() >= _mouse_mv_data_index) { // >= because after the last one it still needs to do one more update to set the values back for normal rendering
				update();
				return;
			}
		}
	}


	if (_useEyetracking) {
		// permanently update the screen
		update();
	}
	
}

/**
 * @brief VolumeRenderWidget::resizeGL
 * @param w
 * @param h
 */
void VolumeRenderWidget::resizeGL(int w, int h)
{
	_screenQuadProjMX.setToIdentity();
    _screenQuadProjMX.perspective(53.14f, 1.0f, Z_NEAR, Z_FAR);

    _overlayProjMX.setToIdentity();
    _overlayProjMX.perspective(53.14f, qreal(w)/qreal(h ? h : 1), Z_NEAR, Z_FAR);

	// std::cout << "resizeGL(): " << "(" << std::to_string(w) << ", " << std::to_string(h) << ")" << std::endl;

    try
    {
        setOutputTextures(floor(w*_imgSamplingRate), floor(h*_imgSamplingRate), _outTexId0, GL_TEXTURE0);
		updateView(0, 0);
    }
    catch (std::runtime_error e)
    {
        qCritical() << "An error occured while generating output texture." << e.what();
    }

    emit frameSizeChanged(this->size());
}

/**
 * @brief VolumeRenderWidget::setOutputTextures
 * works on Texture tex_unit and type GL_TEXTURE_2D
 */
void VolumeRenderWidget::setOutputTextures(int width, int height, GLuint texture, GLuint tex_unit, bool recreateTexture)
{
	// if(texture == _outTexId0)
	
	/* std::cout << "setOutputTextures(): " << "(" << std::to_string(width) << ", " << std::to_string(height) << ")" 
		<< ", texture: " << std::to_string(texture) << ", tex_unit: " << std::to_string(tex_unit) 
		<< ", _volumerender.hasData(): " << _volumerender.hasData() <<  std::endl;*/
	std::string s = std::string("Drop your volume data file here.").append(" width: ").append(std::to_string(width))
		.append(", height: ").append(std::to_string(height));
	
	if(recreateTexture){
		// recreate the texture
		glDeleteTextures(1, &texture);
		glGenTextures(1, &texture);
	}

	glActiveTexture(tex_unit);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if (!_volumerender.hasData())
    {
        QImage img(width, height, QImage::Format_RGBA8888);
        img.fill(Qt::white);
        QPainter p(&img);
        p.setFont(QFont("Helvetia", 12));
		// std::cout << "p.drawText: " << s << std::endl;
        p.drawText(width/2 - 110, height/2, QString::fromStdString(s));
        p.end();
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
                     width, height, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE,
                     img.bits());
    }
    else
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
                     width, height, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE,
                     NULL);
	}
    glGenerateMipmap(GL_TEXTURE_2D);
	
	_volumerender.updateOutputImg(static_cast<size_t>(width), static_cast<size_t>(height), texture);
}

void VolumeRenderWidget::paintGL_standard()
{
	setOutputTextures(floor(this->size().width() * _imgSamplingRate),

		floor(this->size().height()*_imgSamplingRate), _outTexId0, GL_TEXTURE0);

	if (this->_loadingFinished && _volumerender.hasData() && !_noUpdate)
	{
		// OpenCL raycast
		try
		{
			if (_useGL)
				_volumerender.runRaycast(floor(this->size().width() * _imgSamplingRate),
					floor(this->size().height()* _imgSamplingRate), _timestep);
			else
			{
				std::vector<float> d;
				_volumerender.runRaycastNoGL(floor(this->size().width() * _imgSamplingRate),
					floor(this->size().height()* _imgSamplingRate),
					_timestep, d);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F,
					floor(this->size().width() * _imgSamplingRate),
					floor(this->size().height()* _imgSamplingRate),
					0, GL_RGBA, GL_FLOAT,
					d.data());
				glGenerateMipmap(GL_TEXTURE_2D);
				_volumerender.updateOutputImg(static_cast<size_t>(width()),
					static_cast<size_t>(height()), _outTexId0);
			}

			{
				// measurements
				_tmp_ms.frame_coordinates = { -1, -1 };
				_tmp_ms.position_in_Grid = { -1, -1 };
				_tmp_ms.kernel_milliseconds = _volumerender.getLastExecTime() * 1000.0;
				if (_single_measurement) {
					_tmp_ms.manual_measurement = true;
					_take_measurement = true;
				}
			}
			
		}
		catch (std::runtime_error e)
		{
			qCritical() << e.what();
		}
		
	}
	QPainter p(this);
	p.beginNativePainting();
	{
		// render the ray casting output
		// clear to white to avoid getting colored borders outside the quad
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// draw screen quad
		//
		_screenQuadVao.bind();
		_quadVbo.bind();
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

		// render screen quad
		//
		_spScreenQuad.bind();
		_spScreenQuad.setUniformValue(_spScreenQuad.uniformLocation("projMatrix"),
			_screenQuadProjMX);
		_spScreenQuad.setUniformValue(_spScreenQuad.uniformLocation("mvMatrix"),
			_viewMX * _modelMX);

		QVector2D cursorPos(0, 0);
		QVector2D rectExt(0, 0);

		_spScreenQuad.setUniformValue(_spScreenQuad.uniformLocation("cursorPos"), cursorPos);
		_spScreenQuad.setUniformValue(_spScreenQuad.uniformLocation("rectExt"), rectExt);
		

		// _spScreenQuad.setUniformValue(_spScreenQuad.uniformLocation("outTex0"), GL_TEXTURE0); // irelevant because the shader get's its data per layout binding
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		_screenQuadVao.release();
		_quadVbo.release();
		_spScreenQuad.release();

		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);

		if (_volumerender.hasData() && _writeImage)
		{
			QImage img = this->grabFramebuffer();
			QString number = QString("%1").arg(_imgCount++, 6, 10, QChar('0'));
			if (!_recordVideo)
			{
				QLoggingCategory category("screenshot");
				qCInfo(category, "Writing current frame img/frame_%s.png", number.toStdString().c_str());
				_writeImage = false;
			}
			if (!QDir("img").exists())
				QDir().mkdir("img");
			img.save("img/frame_" + number + ".png");
		}
	}
	p.endNativePainting();
	
	double fps = calcFPS();
	
	// render overlays
	if (_showOverlay)
	{
		paintFPS(p, fps, _volumerender.getLastExecTime());
		paintOrientationAxis(p);
	}

	//recover opengl texture
	p.beginNativePainting();
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _outTexId0);
	}
	p.endNativePainting();
	p.end();
}

/*
// discard invocations based on distance to gaze point with only one raycast
* The image is calculated with the full resolution but more and more invocations are discarded
* depending on the distance their fragment / texture-position would be to the gaze position.
*/
void VolumeRenderWidget::paintGL_distance_dc()
{
	double texture_width = floor(this->size().width() * _imgSamplingRate);
	double texture_height = floor(this->size().height() * _imgSamplingRate);
	double execution_time = 0.0;


	std::tuple<float, float> ell1(std::get<0>(_innerEllipse) * texture_width, std::get<1>(_innerEllipse) * texture_height); // Area A
	std::tuple<float, float> ell2(std::get<0>(_outerEllipse) * texture_width, std::get<1>(_outerEllipse) * texture_height);	// Area B

	std::tuple<float, float> cursorPos;

	// cursor position or eyetracking position is not normalized in this rendering case! cursor pos is center of ellipse 1 and ellipse 2.
	if (_useEyetracking) {
		smoothed_nmlzd_coords(); // updates moving average
		std::tuple<float, float> nlzd = _moving_average_gaze_data_nmlz;
		cursorPos = std::tuple<float, float>(std::get<0>(nlzd) * texture_width, std::get<1>(nlzd) * texture_height);
	}
	else {
		cursorPos = std::tuple<float, float>(_lastLocalCursorPos.x() * _imgSamplingRate, _lastLocalCursorPos.y() * _imgSamplingRate);
	}

	
	// -- begin
	
	if (this->_loadingFinished && _volumerender.hasData() && !_noUpdate)
	{
		// OpenCL raycast
		try
		{
			if (_useGL) {

				// raycast
				{
					cl_float2 x_y_dimension_C = { (texture_width / _g_values.x) + 1, (texture_height / _g_values.x) + 1};
					cl_float2 x_y_dimension_B = { (std::get<0>(ell2) / _g_values.y) + 1 , (std::get<1>(ell2) / _g_values.y) + 1 };
					cl_float2 x_y_dimension_A = { (std::get<0>(ell1) / _g_values.z) + 1 , (std::get<1>(ell1) / _g_values.z) + 1 };

					cl_float area_c = x_y_dimension_C.x * x_y_dimension_C.y;
					cl_float area_b = x_y_dimension_B.x * x_y_dimension_B.y + area_c;
					cl_float area_a = area_b + x_y_dimension_A.x * x_y_dimension_A.y;

					cl_uint3 inverts = { cl_uint(area_c), cl_uint(area_b), cl_uint(area_a)};
					_volumerender.setInverts(inverts);

					float x_y_dimension_total = area_a;

					// std::cout << "total: " << x_y_dimension_total << ", C: " << area_c << ", B: " << area_b << ", A: " << area_a << std::endl;
					// std::cout << "tex_width: " << texture_width << ", tex_height: " << texture_height << std::endl;

					_volumerender.setCursorPos(std::get<0>(cursorPos), std::get<1>(cursorPos));

					// rx and ry for ellipse 1. Here not normalized (in pixel)!
					_volumerender.setRectangleExtends(std::get<0>(ell1), std::get<1>(ell1)); // Area A

																							 // rx and ry for ellipse 2. Here not normalized (in pixel)!
					_volumerender.setEllipse2(std::get<0>(ell2), std::get<1>(ell2));	// Area B

					_volumerender.setResolutionFactors(_g_values);

					// std::cout << "tex0: " << _outTexId0 << ", tex1: " << _outTexId1 << std::endl;

					setOutputTextures(texture_width,

						texture_height, _outTexId1, GL_TEXTURE1);


					_volumerender.runRaycast(std::ceilf(std::sqrtf(x_y_dimension_total)) + 1, std::ceil(std::sqrtf(x_y_dimension_total)) + 1);
					execution_time += _volumerender.getLastExecTime();
				}

				// interpolate and combine them
				{
					setOutputTextures(texture_width,

						texture_height, _outTexId0, GL_TEXTURE0);

					_volumerender.setInterpolationParametersForDDC(_g_values, cl_float2{ std::get<0>(cursorPos),std::get<1>(cursorPos) }, cl_float2{ std::get<0>(ell1),std::get<1>(ell1) }, cl_float2{ std::get<0>(ell2),std::get<1>(ell2) });
					_volumerender.runInterpolationForDDC(texture_width, texture_height, _outTexId1, _outTexId0);
					// don't need to add last exec time because of the construction of calcFPS()
				}

				{
					// measurements
					_tmp_ms.kernel_milliseconds = (execution_time + _volumerender.getLastExecTime()) * 1000.0;
					_tmp_ms.frame_coordinates = _lastLocalCursorPos;
					_tmp_ms.position_in_Grid = _tmp_ms.frame_coordinates / _ms_area;
					if (_measure_with_collected_mouse_movement_data || _measurement_is_active && (_measured_data.size() == 0 || _measured_data.back().position_in_Grid != _tmp_ms.position_in_Grid)) {
						_take_measurement = true;
					}
					else {
						if (_single_measurement) {
							_single_measurement = false;
							_take_measurement = true;
						}
						else {
							_take_measurement = false;
						}
					}
				}

			}
			else
			{
				qCritical() << "Distance dependent discarding only available if _useGL == true!\n";
			}
		}
		catch (std::runtime_error e)
		{
			qCritical() << e.what();
		}

	}
	QPainter p(this);
	p.beginNativePainting();
	{
		// render the ray casting output
		// clear to white to avoid getting colored borders outside the quad
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// draw screen quad
		//
		_screenQuadVao.bind();
		_quadVbo.bind();
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

		// render screen quad
		//
		_spScreenQuad.bind();
		_spScreenQuad.setUniformValue(_spScreenQuad.uniformLocation("projMatrix"),
			_screenQuadProjMX);
		_spScreenQuad.setUniformValue(_spScreenQuad.uniformLocation("mvMatrix"),
			_viewMX * _modelMX);

		QVector2D cursorPos(0, 0);
		QVector2D rectExt(0, 0);

		_spScreenQuad.setUniformValue(_spScreenQuad.uniformLocation("cursorPos"), cursorPos);
		_spScreenQuad.setUniformValue(_spScreenQuad.uniformLocation("rectExt"), rectExt);


		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		_screenQuadVao.release();
		_quadVbo.release();
		_spScreenQuad.release();

		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);

		if (_volumerender.hasData() && _writeImage)
		{
			QImage img = this->grabFramebuffer();
			QString number = QString("%1").arg(_imgCount++, 6, 10, QChar('0'));
			if (!_recordVideo)
			{
				QLoggingCategory category("screenshot");
				qCInfo(category, "Writing current frame img/frame_%s.png", number.toStdString().c_str());
				_writeImage = false;
			}
			if (!QDir("img").exists())
				QDir().mkdir("img");
			img.save("img/frame_" + number + ".png");
		}
	}
	p.endNativePainting();

	double fps = calcFPS(execution_time);

	// render overlays
	if (_showOverlay)
	{
		paintFPS(p, fps, _volumerender.getLastExecTime());
		paintOrientationAxis(p);
	}

	//recover opengl texture
	p.beginNativePainting();
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _outTexId0);
	}
	p.endNativePainting();
	p.end();

	// -- end
}

/* The image is created in two render calls. The first one is a low resolution render call, the second is a high resolution
	render call. Each render calls result is stored in a seperate opengl texture. the fragment shader does merge them to the final image.
*/
void VolumeRenderWidget::paintGL_square_dc()
{
	if (_useGL) {
		float width_renderer = static_cast<float>(this->size().width());
		float height_renderer = static_cast<float>(this->size().height());
		float xPos_nlzd = 0.f;
		float yPos_nlzd = 0.f;
		float rect_width_nlzd = 0.f;
		float rect_height_nlzd = 0.f;
		double fps = 0.0;
		double firstExecTime = 0.0;
		if (this->_loadingFinished && _volumerender.hasData() && !_noUpdate)
		{
			// OpenCL raycast
			try
			{
				if (width_renderer > 1.f && height_renderer > 1.f) {
					if (_useEyetracking) {
						smoothed_nmlzd_coords(); // updates moving average
						std::tuple<float, float> nlzd = _moving_average_gaze_data_nmlz;
						xPos_nlzd = std::get<0>(nlzd);
						yPos_nlzd = std::get<1>(nlzd);
					}
					else {
						xPos_nlzd = static_cast<float>(_lastLocalCursorPos.x()) / width_renderer;
						yPos_nlzd = static_cast<float>(_lastLocalCursorPos.y()) / height_renderer;
					}
					rect_width_nlzd = _rect_extends[0] / width_renderer;
					rect_height_nlzd = _rect_extends[1] / height_renderer;
					
					// std::cout << "Eytracking: " << _useEyetracking << "; " << xPos_nlzd << ", " << yPos_nlzd << "   " << rect_width_nlzd << ", " << rect_height_nlzd << std::endl;
				}


				// first render pass: render with low res full image
				{
					double tmp_sampling_rate = _imgSamplingRate;
					_imgSamplingRate *= 0.5;
					// use _outTexId0 as texture for the first image

					setOutputTextures(floor(this->size().width() * _imgSamplingRate),


						floor(this->size().height()*_imgSamplingRate), _outTexId0, GL_TEXTURE0);

					_volumerender.setCursorPos(0.0, 0.0);
					_volumerender.setRectangleExtends(0.0, 0.0);

					_volumerender.setInvert(true);

					_volumerender.runRaycast(floor(this->size().width() * _imgSamplingRate),
						floor(this->size().height()*_imgSamplingRate), _timestep);

					_imgSamplingRate = tmp_sampling_rate;

					firstExecTime = _volumerender.getLastExecTime();
				}

				

				// second render pass: render with normal res a small area
				{

					// use _outTexId1 as texture for the second image
					double img_width = this->size().width() * _imgSamplingRate;
					double img_height = this->size().height()*_imgSamplingRate;
					setOutputTextures(floor(img_width),

						floor(img_height), _outTexId1, GL_TEXTURE1);

					_volumerender.setCursorPos(xPos_nlzd, yPos_nlzd);
					_volumerender.setRectangleExtends(rect_width_nlzd + (16.0 / img_width), rect_height_nlzd + (16.0 / img_height));

					_volumerender.setInvert(false);

					// std::cout << "xpos: " << xPos_nlzd << ", ypos: " << yPos_nlzd << std::endl;

					_volumerender.runRaycast(floor(this->size().width() * _imgSamplingRate),
						floor(this->size().height() * _imgSamplingRate), _timestep);

					{
						// measurements
						_tmp_ms.kernel_milliseconds = (firstExecTime + _volumerender.getLastExecTime()) * 1000.0;
						_tmp_ms.frame_coordinates = _lastLocalCursorPos;
						_tmp_ms.position_in_Grid = _tmp_ms.frame_coordinates / _ms_area;
						if (_measure_with_collected_mouse_movement_data || _measurement_is_active && (_measured_data.size() == 0 || _measured_data.back().position_in_Grid != _tmp_ms.position_in_Grid)) {
							_take_measurement = true;
						}
						else {
							if (_single_measurement) {
								_single_measurement = false;
								_take_measurement = true;
							}
							else {
								_take_measurement = false;
							}
						}
					}

					fps = calcFPS(firstExecTime);
				}
			}
			catch (std::runtime_error e)
			{
				qCritical() << e.what();
			}

		}
		// draw output and overlays
		{
			QPainter p(this);
			p.beginNativePainting();
			{
				// render the ray casting output
				// clear to white to avoid getting colored borders outside the quad
				glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				// draw screen quad
				//
				_screenQuadVao.bind();
				_quadVbo.bind();
				glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

				// render screen quad
				//
				_spScreenQuad.bind();
				_spScreenQuad.setUniformValue(_spScreenQuad.uniformLocation("projMatrix"),
					_screenQuadProjMX);
				_spScreenQuad.setUniformValue(_spScreenQuad.uniformLocation("mvMatrix"),
					_viewMX * _modelMX);

				QVector2D cursorPos(xPos_nlzd, yPos_nlzd);
				QVector2D rectExt(rect_width_nlzd, rect_height_nlzd);

				_spScreenQuad.setUniformValue(_spScreenQuad.uniformLocation("cursorPos"), cursorPos);
				_spScreenQuad.setUniformValue(_spScreenQuad.uniformLocation("rectExt"), rectExt);

				glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

				_screenQuadVao.release();
				_quadVbo.release();
				_spScreenQuad.release();

				glDisable(GL_CULL_FACE);
				glDisable(GL_DEPTH_TEST);

				if (_volumerender.hasData() && _writeImage)
				{
					QImage img = this->grabFramebuffer();
					QString number = QString("%1").arg(_imgCount++, 6, 10, QChar('0'));
					if (!_recordVideo)
					{
						QLoggingCategory category("screenshot");
						qCInfo(category, "Writing current frame img/frame_%s.png", number.toStdString().c_str());
						_writeImage = false;
					}
					if (!QDir("img").exists())
						QDir().mkdir("img");
					img.save("img/frame_" + number + ".png");
				}
			}
			p.endNativePainting();

			// render overlays
			if (_showOverlay)
			{
				paintFPS(p, fps, _volumerender.getLastExecTime());
				paintOrientationAxis(p);
			}

			// recover opengl texture
			p.beginNativePainting();
			{
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, _outTexId0);
			}
			p.endNativePainting();

			// printQPoint(gaze_data_to_global());

			// p.fillRect(QRect(-QPoint(10 + std::get<0>(_rect_extends) / 2, std::get<1>(_rect_extends) / 2 + 10 + this->size().height()) + mapFromGlobal(gaze_data_to_global()), QSize(20, 20)), Qt::red);

			p.end();
		}

	}
	else {
		qDebug() << "mouse square is only available if useGL is enabled.\n";
	}
}

void VolumeRenderWidget::paintGL_ThreeRenderInvocations() {
	double texture_width = floor(this->size().width() * _imgSamplingRate);
	double texture_height = floor(this->size().height() * _imgSamplingRate);
	double execution_time = 0.0;

	std::tuple<float, float> circles_radiuses = _circle_radiuses; // Circle radiuses for inner circle (first) and outer cirlce (second) in pixel

	std::tuple<float, float> cursorPos;

	// cursor position or eyetracking position is not normalized in this rendering case! cursor pos is center of ellipse 1 and ellipse 2.
	if (_useEyetracking) {
		smoothed_nmlzd_coords(); // updates moving average
		std::tuple<float, float> nlzd = _moving_average_gaze_data_nmlz;
		cursorPos = std::tuple<float, float>(std::get<0>(nlzd) * texture_width, std::get<1>(nlzd) * texture_height);
	}
	else {
		cursorPos = std::tuple<float, float>(_lastLocalCursorPos.x() * _imgSamplingRate, _lastLocalCursorPos.y() * _imgSamplingRate);
	}


	// -- begin
	GLuint texLo;
	GLuint texUnitLo = GL_TEXTURE2;

	GLuint texLm;
	GLuint texUnitLm = GL_TEXTURE3;

	GLuint texLi;
	GLuint texUnitLi = GL_TEXTURE4;

	GLuint texIpLo;
	GLuint texUnitIpLo = GL_TEXTURE5;

	GLuint texIpLm;
	GLuint texUnitIpLm = GL_TEXTURE6;


	if (this->_loadingFinished && _volumerender.hasData() && !_noUpdate)
	{
		// OpenCL raycast
		try
		{
			if (_useGL) {
				// _volumerender.setCursorPos(std::get<0>(cursorPos), std::get<1>(cursorPos));
				// _volumerender.setResolutionFactors(_g_values); // gaps for the different layers.
				
				// _volumerender.setRectangleExtends(r1_adjusted_to_sr, r2_adjusted_to_sr); // set circle radiuses without offset alpha

				// _volumerender.setEllipse2(mr, ir); // set circle radiuses with offset alpha

				{ // set up textures for use only in runTRIMethod
					{ // Lo
						glGenTextures(1, &texLo);
						glActiveTexture(texUnitLo);
						glBindTexture(GL_TEXTURE_2D, texLo);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
						glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
							texture_width, texture_height, 0,
							GL_RGBA, GL_UNSIGNED_BYTE,
							NULL);
						glGenerateMipmap(GL_TEXTURE_2D);
					}
					
					{ // Lm
						glGenTextures(1, &texLm);
						glActiveTexture(texUnitLm);
						glBindTexture(GL_TEXTURE_2D, texLm);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
						glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
							texture_width, texture_height, 0,
							GL_RGBA, GL_UNSIGNED_BYTE,
							NULL);
						glGenerateMipmap(GL_TEXTURE_2D);
					}

					{ // Li
						glGenTextures(1, &texLi);
						glActiveTexture(texUnitLi);
						glBindTexture(GL_TEXTURE_2D, texLi);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
						glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
							texture_width, texture_height, 0,
							GL_RGBA, GL_UNSIGNED_BYTE,
							NULL);
						glGenerateMipmap(GL_TEXTURE_2D);
					}

					{ // Lo Ip
						glGenTextures(1, &texIpLo);
						glActiveTexture(texUnitIpLo);
						glBindTexture(GL_TEXTURE_2D, texIpLo);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
						glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
							texture_width, texture_height, 0,
							GL_RGBA, GL_UNSIGNED_BYTE,
							NULL);
						glGenerateMipmap(GL_TEXTURE_2D);
					}

					{ // Lm Ip
						glGenTextures(1, &texIpLm);
						glActiveTexture(texUnitIpLm);
						glBindTexture(GL_TEXTURE_2D, texIpLm);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
						glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
							texture_width, texture_height, 0,
							GL_RGBA, GL_UNSIGNED_BYTE,
							NULL);
						glGenerateMipmap(GL_TEXTURE_2D);
					}
				}


				setOutputTextures(texture_width,
					texture_height, _outTexId0, GL_TEXTURE0);	// set output texture to tex0 which will have the final image. images used temporarily are created and handled by runTRIMethod()
				_volumerender.runTRIMethod(texture_width, texture_height, 0, { std::get<0>(cursorPos) , std::get<1>(cursorPos) }, _g_values, { std::get<1>(circles_radiuses),  std::get<0>(circles_radiuses) }, _imgSamplingRate, texLo, texLm, texLi, texIpLo, texIpLm);

				/*{ // Outer Layer Lo

					{ // raycast
						_volumerender.setInvert(0);

						setOutputTextures(texture_width,
							texture_height, _outTexId1, GL_TEXTURE1);

						_volumerender.runRaycast(texture_width / _g_values.x + 1, texture_height / _g_values.x + 1); // run raycast on tex 1

						execution_time += _volumerender.getLastExecTime();
					}

					{ // interpolation
						setOutputTextures(texture_width,
							texture_height, _outTexId0, GL_TEXTURE0);

						_volumerender.setInterpolationParametersForTRI(_g_values, cursorPos_atsr, cl_float2{ r1_adjusted_to_sr, r2_adjusted_to_sr }, cl_float2{ ir, mr }, 0);
						_volumerender.runInterpolationForTRI(texture_width, texture_height, _outTexId1, _outTexId0); // run interpolation with output to tex 0
						execution_time += _volumerender.getLastExecTime();
					}
				}*/

				/*{ // Middle Layer Lm

					{ // raycast
						_volumerender.setInvert(1);

						setOutputTextures(texture_width,
							texture_height, _outTexId1, GL_TEXTURE1, true);

						_volumerender.runRaycast(2 * mr / _g_values.y, 2 * mr / _g_values.y); // run raycast on tex 0
						execution_time += _volumerender.getLastExecTime();
					}

					{ // interpolation and then blending with outer layer
						setOutputTextures(texture_width,
							texture_height, _outTexId0, GL_TEXTURE0, true);

						_volumerender.setInterpolationParametersForTRI(_g_values, cursorPos_atsr, cl_float2{ r1_adjusted_to_sr, r2_adjusted_to_sr }, cl_float2{ ir, mr }, 1);
						_volumerender.runInterpolationForTRI(2 * mr, 2 * mr, _outTexId1, _outTexId0); // run interpolation with output to tex 0
						execution_time += _volumerender.getLastExecTime();
					}
				}*/

				/*{ // Inner Layer Li

					{ // raycast
						_volumerender.setInvert(2);

						setOutputTextures(texture_width,
							texture_height, _outTexId1, GL_TEXTURE1, false);

						_volumerender.runRaycast(ir, ir); // run raycast on tex 1
						execution_time += _volumerender.getLastExecTime();
					}

					{ // interpolation and then blending with middle layer
						setOutputTextures(texture_width,
							texture_height, _outTexId0, GL_TEXTURE0, false);

						_volumerender.setInterpolationParametersForTRI(_g_values, cursorPos_atsr, cl_float2{ r1_adjusted_to_sr, r2_adjusted_to_sr }, cl_float2{ ir, mr }, 2);
						_volumerender.runInterpolationForTRI(texture_width, texture_height, _outTexId1, _outTexId0); // run interpolation with output to tex 0
					}
				}*/

			}
			else
			{
				qCritical() << "Distance dependent discarding only available if _useGL == true!\n";
			}
		}
		catch (std::runtime_error e)
		{
			qCritical() << e.what();
		}

	}
	QPainter p(this);
	p.beginNativePainting();
	{
		// render the ray casting output
		// clear to white to avoid getting colored borders outside the quad
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// draw screen quad
		//
		_screenQuadVao.bind();
		_quadVbo.bind();
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

		// render screen quad
		//
		_spScreenQuad.bind();
		_spScreenQuad.setUniformValue(_spScreenQuad.uniformLocation("projMatrix"),
			_screenQuadProjMX);
		_spScreenQuad.setUniformValue(_spScreenQuad.uniformLocation("mvMatrix"),
			_viewMX * _modelMX);

		QVector2D cursorPos(0, 0);
		QVector2D rectExt(0, 0);

		_spScreenQuad.setUniformValue(_spScreenQuad.uniformLocation("cursorPos"), cursorPos);
		_spScreenQuad.setUniformValue(_spScreenQuad.uniformLocation("rectExt"), rectExt);


		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		_screenQuadVao.release();
		_quadVbo.release();
		_spScreenQuad.release();

		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);

		if (_volumerender.hasData() && _writeImage)
		{
			QImage img = this->grabFramebuffer();
			QString number = QString("%1").arg(_imgCount++, 6, 10, QChar('0'));
			if (!_recordVideo)
			{
				QLoggingCategory category("screenshot");
				qCInfo(category, "Writing current frame img/frame_%s.png", number.toStdString().c_str());
				_writeImage = false;
			}
			if (!QDir("img").exists())
				QDir().mkdir("img");
			img.save("img/frame_" + number + ".png");
		}
	}
	p.endNativePainting();

	double fps = calcFPS(execution_time);

	// render overlays
	if (_showOverlay)
	{
		paintFPS(p, fps, _volumerender.getLastExecTime());
		paintOrientationAxis(p);
	}

	//recover opengl texture
	p.beginNativePainting();
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _outTexId0);
	}
	p.endNativePainting();
	p.end();

	{ // Delete Textures only used in TRI
		glDeleteTextures(1, &texLo);
		glDeleteTextures(1, &texLm);
		glDeleteTextures(1, &texLi);
		glDeleteTextures(1, &texIpLo);
		glDeleteTextures(1, &texIpLm);
	}

	// -- end
}

void VolumeRenderWidget::setShowOverlay(bool showOverlay)
{
    _showOverlay = showOverlay;
    updateView();
}

void VolumeRenderWidget::setEyetracking(bool eyetracking)
{
	if (check_eyetracker_availability()) {
		if (eyetracking) {
			// start using eyetracking: subscribe to data
			TobiiResearchStatus status = tobii_research_subscribe_to_gaze_data(_eyetracker, &VolumeRenderWidget::gaze_data_callback, &_gaze_data);
			if (status != TOBII_RESEARCH_STATUS_OK) {
				qCritical() << "Something went wrong when trying to subscribe to eyetracker data.\n";
			}
		}
		else {
			// stop using eyetracking: unsubscribe to data
			TobiiResearchStatus status = tobii_research_unsubscribe_from_gaze_data(_eyetracker, &VolumeRenderWidget::gaze_data_callback);
			if (status != TOBII_RESEARCH_STATUS_OK) {
				qCritical() << "Something went wrong when trying to unsubscribe to eyetracker data.\n";
			}
				
		}
		_useEyetracking = eyetracking;
	}
	else {
		_useEyetracking = false;
		QWidget* ppW = parentWidget()->parentWidget();
		QCheckBox* eyeCB = ppW->findChild<QCheckBox*>("chbEyetracking");
		eyeCB->setChecked(false);
	}
	update();
	// std::cout << "use eyetracking: " << _useEyetracking << std::endl;
}

QQuaternion VolumeRenderWidget::getCamRotation() const
{
    return _rotQuat;
}

void VolumeRenderWidget::setCamRotation(const QQuaternion &rotQuat)
{
    _rotQuat = rotQuat;
}

void VolumeRenderWidget::setRenderingMethod(int rm)
{
	this->_renderingMethod = static_cast<VolumeRenderWidget::RenderingMethod>(rm);
	update();
}

QVector3D VolumeRenderWidget::getCamTranslation() const
{
    return _translation;
}

void VolumeRenderWidget::setCamTranslation(const QVector3D &translation)
{
    _translation = translation;
}

/**
 * @brief VolumeRenderWidget::showSelectOpenCL
 */
void VolumeRenderWidget::showSelectOpenCL()
{
    std::vector<std::string> names;
    try
    {
        names = _volumerender.getPlatformNames();
    }
    catch (std::runtime_error e)
    {
        qCritical() << "An error occured while trying to retrieve OpenCL platform information."
                    << e.what();
        return;
    }

    QStringList platforms;
    for (std::string &s : names)
        platforms.append(QString::fromStdString(s));

    bool ok;
    QString platform = QInputDialog::getItem(this, tr("Select platform"),
                                             tr("Select OpenCL platform:"),
                                             platforms, 0, false, &ok);
    if (ok && !platform.isEmpty())
    {
        cl_vendor vendor = VENDOR_ANY;
        QString type = "GPU";
        _useGL = false;

        // filter GPU only platforms
        if (platform.contains("NVIDIA"))
            vendor = VENDOR_NVIDIA;
        else
        {
            if (!platform.contains("Graphics"))
            {
                QStringList types;
                types << "GPU" << "CPU";
                type = QInputDialog::getItem(this, tr("Select type"),
                                             tr("Select device type:"), types, 0, false, &ok);
            }
            if (platform.contains("Advanced Micro Devices", Qt::CaseInsensitive))
                vendor = VENDOR_AMD;
            else if (platform.contains("Intel", Qt::CaseInsensitive))
                vendor = VENDOR_INTEL;

        }
        if (!type.isEmpty())
        {
            if (type == "GPU")
            {
                QMessageBox msgBox;
                msgBox.setText("Do you wish to try OpenGL context sharing using this platform?");
                msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                msgBox.setDefaultButton(QMessageBox::Yes);
                int ret = msgBox.exec();
                _useGL = ret == QMessageBox::Yes;
            }

            int platformId = platforms.indexOf(platform);
            try
            {
                names = _volumerender.getDeviceNames(platformId, type.toStdString());
            }
            catch (std::runtime_error e)
            {
                qCritical() << "No capable device found on using the selected platform and type."
                            << e.what();
                return;
            }
            QStringList devices;
            for (std::string &s : names)
                devices.append(QString::fromStdString(s));

            QString device;
            if (devices.empty())
                throw std::runtime_error("No capable device found on the selected platform.");
            else if (devices.size() == 1)
                device = devices.front();
            else
                device = QInputDialog::getItem(this, tr("Select device"),
                                                   tr("Select OpenCL device:"),
                                                   devices, 0, false, &ok);
            if (!device.isEmpty())
            {
                try {
                    _volumerender.initialize(_useGL, type == "CPU", vendor);
                } catch (std::runtime_error e) {
                    qCritical() << e.what() << "\nSwitching to CPU fallback mode.";
                    _useGL = false;
                    _volumerender.initialize(false, true);
                }
                catch (...) {
                    qCritical() << "An unknown error occured initializing OpenCL/OpenGL.";
                }

                try {
                    setOutputTextures(floor(width()*_imgSamplingRate),


                                           floor(height()*_imgSamplingRate), _outTexId0, GL_TEXTURE0);
					updateView(0, 0);
                } catch (std::runtime_error e) {
                    qCritical() << "An error occured while generating output texture." << e.what();
                }
            }
        }
    }
}

void VolumeRenderWidget::showSelectEyetrackingDevice()
{
	// qDebug() << "show Select Eyetracking Device.\n";

	TobiiResearchEyeTrackers* eyetrackers = NULL;
	std::vector<std::string> eyetracker_device_names;

	TobiiResearchStatus result;
	size_t i = 0;
	result = tobii_research_find_all_eyetrackers(&eyetrackers);
	if (result != TOBII_RESEARCH_STATUS_OK) {
		qCritical() << "Finding trackers failed. Error: " << result << "\n";
		return;
	}
	else {
		if (eyetrackers->count == 0) {
			qDebug() << "No Eyetrackers found!\n";
		}
	}

	

	for (i = 0; i < eyetrackers->count; i++) {
		TobiiResearchEyeTracker* eyetracker = eyetrackers->eyetrackers[i];
		char* device_name;
		char* serial_number;
		tobii_research_get_device_name(eyetracker, &device_name);
		tobii_research_get_serial_number(eyetracker, &serial_number);
		eyetracker_device_names.push_back(std::string(device_name).append(", serial number: ").append(std::string(serial_number)));
		tobii_research_free_string(device_name);
		tobii_research_free_string(serial_number);
	}

	// ---- upper bound



	QStringList platforms;
	bool ok;
	bool only_one = false;
	QString platform;
	int eyetracker_index;

	for (std::string &s : eyetracker_device_names)
		platforms.append(QString::fromStdString(s));
	
	if (eyetracker_device_names.size() > 1) {
		platform = QInputDialog::getItem(this, tr("Select Eyetracker"),
			tr("Select Eyetracking Device:"),
			platforms, 0, false, &ok);
		eyetracker_index = platforms.indexOf(platform);
	}
	else {
		only_one = true;
		eyetracker_index = 0;
	}
	if (ok && !platform.isEmpty() || only_one && eyetrackers->count > 0)
	{
		// qDebug() << "selected platform: " << platform << "\nindex: " << eyetracker_index << "\n";
		_eyetracker = eyetrackers->eyetrackers[eyetracker_index];
	}
	// ---- lower bound
	tobii_research_free_eyetrackers(eyetrackers);
}

bool VolumeRenderWidget::MonitorEnumProc(HMONITOR monitor, HDC hdcMnitor, LPRECT rect, LPARAM param)
{
	struct return_data_struct {
		bool success;
		long left;
		long top;
		long right;
		long bottom;
		std::wstring device_name;
	};
	
	return_data_struct* return_data_ptr = reinterpret_cast<return_data_struct*>(param);

	MONITORINFOEX qmi;
	qmi.cbSize = sizeof(MONITORINFOEX);

	bool success = GetMonitorInfo(monitor, &qmi);

	if (success) {
		/*std::wcout << std::wstring(qmi.szDevice) << ", " << (*return_data_ptr).device_name << "." << std::endl;
		std::wcout << return_data_ptr->success << ", " << std::endl;*/
		if (std::wstring(qmi.szDevice).compare(return_data_ptr->device_name) == 0) {
			return_data_ptr->success = true;
			return_data_ptr->bottom = qmi.rcMonitor.bottom;
			return_data_ptr->left = qmi.rcMonitor.left;
			return_data_ptr->right = qmi.rcMonitor.right;
			return_data_ptr->top = qmi.rcMonitor.top;
		}
	}

	return success;
}

void VolumeRenderWidget::actionSelectMonitor()
{
	// qDebug() << "calibrate eyetracking device.\n";

	// enumerate display devices and get names
	DISPLAY_DEVICE dd;
	dd.cb = sizeof(dd);
	
	std::vector<std::wstring> device_names;
	std::vector<std::wstring> device_strings;

	for (int deviceIndex = 0; EnumDisplayDevices(0, deviceIndex, &dd, 0); deviceIndex++) {
		std::wstring dn = dd.DeviceName;
		for (int monitorIndex = 0; EnumDisplayDevices(dn.c_str(), monitorIndex, &dd, 0); monitorIndex++) {
			device_names.push_back(dd.DeviceName);
			device_strings.push_back(dd.DeviceString);
		}
	}

	std::wstring result;
	
	for (int i = 0; i < device_names.size(); i++) {
		result.append(L"Device ").append(std::to_wstring(i)).append(L": ").append(device_names[i]).append(L", ").append(device_strings[i]).append(L"\n");
		
		// only need the substring of th device name to be able to compare it with the name returned by GetMonitorInfo
		device_names[i] = device_names[i].substr(0, 12);
	}
	
	std::wcout << result;
	
	
	// QPoint wpos = mapToGlobal(QPoint(0, 0));

	// qDebug() << "Volumerenderwidet position: " << wpos << "\n";

	if (device_names.size() != device_strings.size()) {
		qCritical() << "Something went wrong! device_names and device_strings have different sizes!\n";
		return;
	}

	// ---- upper bound
	QStringList platforms;
	for (int i = 0; i < device_names.size(); i++)
		platforms.append(QString::fromStdWString(device_names[i]).append(QString::fromStdString(", ")).append(QString::fromStdWString(device_strings[i])));

	bool ok;
	bool only_one = false;
	QString platform;
	int monitor_index;

	if (device_names.size() > 1) {
		platform = QInputDialog::getItem(this, tr("Select Monitor"),
			tr("Select the Monitor the Eyetracker is calibrated to:"),
			platforms, 0, false, &ok);
		monitor_index = platforms.indexOf(platform);
	}
	else {
		only_one = true;
		monitor_index = 0;
	}

	if (ok && !platform.isEmpty() || only_one)
	{
		struct return_data_struct {
			bool success;
			long left;
			long top;
			long right;
			long bottom;
			std::wstring device_name;
		} return_data;

		return_data.success = false;
		return_data.device_name = device_names[monitor_index];

		// enumerate display monitors to retrive handles and information
		EnumDisplayMonitors(NULL, NULL, reinterpret_cast<MONITORENUMPROC>(&VolumeRenderWidget::MonitorEnumProc), reinterpret_cast<LPARAM>(&return_data));
		
		if (return_data.success) {
			_monitor_offset = QPoint(return_data.left, return_data.top);
			_curr_monitor_width = return_data.right - return_data.left;
			_curr_monitor_height = return_data.bottom - return_data.top;
			// qDebug() << "Selected: " << QString::fromStdWString(return_data.device_name) << "\n"
			//	<< "mon_off_x: " << _monitor_offset.x() << ", mon_off_y: " << _monitor_offset.y() << ", width: " << _curr_monitor_width << ", height: " << _curr_monitor_height << "\n";
		}
		else {
			qCritical() << "Could not retrive data for selected Monitor!\n";
		}


		// qDebug() << "selected platform: " << platform << "\nindex: " << eyetracker_index << "\n";
		//_eyetracker = eyetrackers->eyetrackers[eyetracker_index];
	}
	// ---- lower bound
}

/**
 * @brief VolumeRenderWidget::setupVertexAttribs
 */
void VolumeRenderWidget::setupVertexAttribs()
{
    // screen quad
    _quadVbo.bind();
    glEnableVertexAttribArray( 0 );
    glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 0, 0 );
    _quadVbo.release();
}


/**
 * @brief VolumeRenderWidget::setVolumeData
 * @param fileName
 */
void VolumeRenderWidget::setVolumeData(const QString &fileName)
{
    this->_noUpdate = true;
    int timesteps = 0;
    try
    {
        timesteps = _volumerender.loadVolumeData(fileName.toStdString());
    }
    catch (std::invalid_argument e)
    {
        qCritical() << e.what();
    }
    catch (std::runtime_error e)
    {
        qCritical() << e.what();
    }
    emit timeSeriesLoaded(timesteps - 1);

    _overlayModelMX.setToIdentity();
    QVector3D res = getVolumeResolution();
    _overlayModelMX.scale(res / qMax(res.x(), qMax(res.y(), res.z())));
    this->_noUpdate = false;
    update();
}


/**
 * @brief VolumeRenderWidget::hasData
 * @return
 */
bool VolumeRenderWidget::hasData()
{
    return _volumerender.hasData();
}


/**
 * @brief VolumeRenderWidget::getVolumeResolution
 * @return
 */
const QVector3D VolumeRenderWidget::getVolumeResolution()
{
    if (_volumerender.hasData() == false)
        return QVector3D();

    return QVector3D(_volumerender.getResolution().at(0),
                     _volumerender.getResolution().at(1),
                     _volumerender.getResolution().at(2));
}


/**
 * @brief VolumeRenderWidget::updateStepSize
 * @param stepSize
 */
void VolumeRenderWidget::updateSamplingRate(double samplingRate)
{
    _volumerender.updateSamplingRate(samplingRate);
    update();
}


/**
 * @brief VolumeRenderWidget::setInterpolation
 * @param method
 */
void VolumeRenderWidget::setTffInterpolation(const QString method)
{
    if (method.contains("Quad"))
        _tffInterpol = QEasingCurve::InOutQuad;
    else if (method.contains("Linear"))
        _tffInterpol = QEasingCurve::Linear;
}

/**
 * @brief VolumeRenderWidget::setRawTransferFunction
 * @param tff
 */
void VolumeRenderWidget::setRawTransferFunction(std::vector<unsigned char> tff)
{
    try
    {
        _volumerender.setTransferFunction(tff);
        update();
    }
    catch (std::runtime_error e)
    {
        qCritical() << e.what();
    }
}

/**
 * @brief VolumeRenderWidget::updateTransferFunction
 * @param stops
 */
void VolumeRenderWidget::updateTransferFunction(QGradientStops stops)
{
    const int tffSize = 256;
    const qreal granularity = 4096.0;
    std::vector<uchar> tff(tffSize*4);
    std::vector<unsigned int> prefixSum(tffSize);

    QPropertyAnimation interpolator;
    interpolator.setEasingCurve(_tffInterpol);
    interpolator.setDuration(granularity);
    foreach (QGradientStop stop, stops)
    {
        interpolator.setKeyValueAt(stop.first, stop.second);
    }
//    tff.at(0) = (uchar)0;
//    tff.at(1) = (uchar)0;
//    tff.at(2) = (uchar)0;
//    tff.at(3) = (uchar)0;
#pragma omp for
    for (int i = 0; i < tffSize; ++i)
    {
        interpolator.setCurrentTime((i/static_cast<double>(tffSize)) * granularity);
        tff.at(i*4 + 0) = (uchar)qMax(0, interpolator.currentValue().value<QColor>().red()   - 3);
        tff.at(i*4 + 1) = (uchar)qMax(0, interpolator.currentValue().value<QColor>().green() - 3);
        tff.at(i*4 + 2) = (uchar)qMax(0, interpolator.currentValue().value<QColor>().blue()  - 3);
        tff.at(i*4 + 3) = (uchar)qMax(0, interpolator.currentValue().value<QColor>().alpha() - 3);
        prefixSum.at(i) = tff.at(i*4 + 3);
    }
    try
    {
        _volumerender.setTransferFunction(tff);
        // TODO: replace with std::exclusicve_scan(std::par, ... )
        std::partial_sum(prefixSum.begin(), prefixSum.end(), prefixSum.begin());
        _volumerender.setTffPrefixSum(prefixSum);
    }
    catch (std::runtime_error e)
    {
        qCritical() << e.what();
    }

    update();
}

std::vector<unsigned char> VolumeRenderWidget::getRawTransferFunction(QGradientStops stops) const
{
    const size_t tffSize = 256;
    const qreal granularity = 4096.0;
    std::vector<uchar> tff(tffSize*4);

    QPropertyAnimation interpolator;
    interpolator.setEasingCurve(_tffInterpol);
    interpolator.setDuration(granularity);
    foreach (QGradientStop stop, stops)
    {
        interpolator.setKeyValueAt(stop.first, stop.second);
    }

    for (size_t i = 0; i < tffSize; ++i)
    {
        interpolator.setCurrentTime((i/static_cast<double>(tffSize)) * granularity);
        tff.at(i*4 + 0) = (uchar)qMax(0, interpolator.currentValue().value<QColor>().red()   - 3);
        tff.at(i*4 + 1) = (uchar)qMax(0, interpolator.currentValue().value<QColor>().green() - 3);
        tff.at(i*4 + 2) = (uchar)qMax(0, interpolator.currentValue().value<QColor>().blue()  - 3);
        tff.at(i*4 + 3) = (uchar)qMax(0, interpolator.currentValue().value<QColor>().alpha() - 3);
    }
    return tff;
}

/**
 * @brief VolumeRenderWidget::cleanup
 */
void VolumeRenderWidget::cleanup()
{
//    makeCurrent();
//    if (_quadVbo.isCreated())
//        _quadVbo.destroy();
}

/**
 * @brief VolumeRenderWidget::mousePressEvent
 * @param event
 */
void VolumeRenderWidget::mousePressEvent(QMouseEvent *event)
{
    // nothing yet
    _lastLocalCursorPos = event->pos();
}


/**
 * @brief VolumeRenderWidget::mouseReleaseEvent
 * @param event
 */
void VolumeRenderWidget::mouseReleaseEvent(QMouseEvent *event)
{
    event->accept();
}

/**
 * @brief VolumeRenderWidget::resetCam
 */
void VolumeRenderWidget::resetCam()
{
    _rotQuat = QQuaternion(1, 0, 0, 0);
    _translation = QVector3D(0, 0, 2.0);
    updateView();
}

/**
 * @brief update camera view
 * @param dx
 * @param dy
 */
void VolumeRenderWidget::updateView(float dx, float dy)
{
    QVector3D rotAxis = QVector3D(dy, dx, 0.0f).normalized();
    double angle = QVector2D(dx, dy).length()*500;
    _rotQuat = _rotQuat * QQuaternion::fromAxisAndAngle(rotAxis, -angle);

    QMatrix4x4 viewMat;
//    viewMat.scale(QVector3D(1,1,1./_zScale));
    viewMat.rotate(_rotQuat);

    _coordViewMX.setToIdentity();
    _coordViewMX.scale(1, -1, 1);
    _coordViewMX.translate(_translation * -1.0);
    _coordViewMX *= QMatrix4x4(_rotQuat.toRotationMatrix().transposed());

    viewMat.translate(_translation);
    viewMat.scale(_translation.z());

    std::array<float, 16> viewArray;
    for (size_t i = 0; i < viewArray.size(); ++i)
    {
        viewArray.at(i) = viewMat.transposed().constData()[i];
    }

    try
    {
        _volumerender.updateView(viewArray);
    }
    catch (std::runtime_error e)
    {
        qCritical() << e.what();
    }
    update();
}


/**
 * @brief VolumeRenderWidget::mouseMoveEvent
 * @param event
 */
void VolumeRenderWidget::mouseMoveEvent(QMouseEvent *event)
{
	if (_measure_with_collected_mouse_movement_data) {
		// if data is currently measured, don't handle mouse movement events at all.
		event->accept();
		return;
	}

	if (_collect_mouse_movement) {
		// only collect mouse movement data but do not anything else with the mouse event especially not updating the screen.
		_tmp_mp.mouse_pos = event->pos();
		
		_tmp_mp.grid_location = _tmp_mp.mouse_pos / _ms_area;

		if (_mouse_mv_data.size() == 0 || _mouse_mv_data.back().grid_location != _tmp_mp.grid_location) {
			_mouse_mv_data.push_back(_tmp_mp);
		}

		event->accept();
		return;
	}

    float dx = (float)(event->pos().x() - _lastLocalCursorPos.x()) / width();
    float dy = (float)(event->pos().y() - _lastLocalCursorPos.y()) / height();

    // rotate object
    if (event->buttons() & Qt::LeftButton)
    {
        if (event->modifiers() & Qt::ShiftModifier)
        {
            dx *= 0.1f;
            dy *= 0.1f;
        }
        updateView(dx, dy);
    }
    // translate object
    if (event->buttons() & Qt::MiddleButton)
    {
        int sensitivity = 6;
        if (event->modifiers() & Qt::ShiftModifier)
            sensitivity = 1;

        _translation.setX(_translation.x() - dx*sensitivity);
        _translation.setY(_translation.y() + dy*sensitivity);
        updateView();
    }

	switch (_renderingMethod) {
	case RenderingMethod::SQUARE_DC:
		update();
		break;
	case RenderingMethod::DISTANCE_DC:
		update();
		break;
	case RenderingMethod::TRI:
		update();
		break;
	default:
		break;
	}

    _lastLocalCursorPos = event->pos();
    event->accept();
}


/**
 * @brief VolumeRenderWidget::wheelEvent
 * @param event
 */
void VolumeRenderWidget::wheelEvent(QWheelEvent *event)
{
    double t = 1600.0;
    if (event->modifiers() & Qt::ShiftModifier)
        t *= 6.0;

    // limit translation to origin, otherwise camera setup breaks (flips)
    _translation.setZ(qMax(0.01, _translation.z() - event->angleDelta().y() / t));
    updateView();
    event->accept();
}


/**
 * @brief VolumeRenderWidget::mouseDoubleClickEvent
 * @param event
 */
void VolumeRenderWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    // nothing yet
    event->accept();
}


bool VolumeRenderWidget::check_eyetracker_availability()
{
	if (_eyetracker == nullptr) {
		return false;
	}
	else {
		TobiiResearchEyeTrackers* eyetrackers = NULL;

		TobiiResearchStatus result;
		result = tobii_research_find_all_eyetrackers(&eyetrackers);
		if (result != TOBII_RESEARCH_STATUS_OK) {
			qCritical() << "Finding trackers to check status failed. Error: " << result << "\n";
			return false;
		}

		bool eyetracker_exists = false;
		for (int i = 0; i < eyetrackers->count; i++) {
			if (_eyetracker == eyetrackers->eyetrackers[i]) {
				eyetracker_exists = true; 
			}
		}
		tobii_research_free_eyetrackers(eyetrackers);
		return eyetracker_exists;
	}
	
}


void VolumeRenderWidget::gaze_data_callback(TobiiResearchGazeData * gaze_data, void * user_data)
{
	memcpy(user_data, gaze_data, sizeof(*gaze_data));
}

QPoint VolumeRenderWidget::gaze_data_to_global()
{
	float gaze_point_x = (_gaze_data.left_eye.gaze_point.position_on_display_area.x + _gaze_data.right_eye.gaze_point.position_on_display_area.x) / 2;
	float gaze_point_y = (_gaze_data.left_eye.gaze_point.position_on_display_area.y + _gaze_data.right_eye.gaze_point.position_on_display_area.y) / 2;

	QPoint gaze_monitor_local = QPoint(gaze_point_x * static_cast<float>(_curr_monitor_width), gaze_point_y * static_cast<float>(_curr_monitor_height));
	
	QPoint gaze_global = _monitor_offset + gaze_monitor_local;

	/*std::cout << "\n\ngaze_point_x: " << gaze_point_x << ", gaze_point_y: " << gaze_point_y
		<< ", \n_curr_monitor_width: " << _curr_monitor_width << ", _curr_monitor_height: " << _curr_monitor_height
		<< ", \n gaze_monitor_local.x(): " << gaze_monitor_local.x() << ", gaze_monitor_local.y(): " << gaze_monitor_local.y()
		<< ", \n _monitor_offset.x(): " << _monitor_offset.x() << ", _monitor_offset.y(): " << _monitor_offset.y()
		<< ", \n gaze_global.x(): " << gaze_global.x() << ", gaze_global.y(): " << gaze_global.y() 
		<< ", \n this widgets global position x: " << mapToGlobal(QPoint(0, 0)).x() << ", y: " << mapToGlobal(QPoint(0, 0)).y() 
		<< std::endl;*/
	
	return gaze_global;
}

std::tuple<float, float> VolumeRenderWidget::normalized_ogl_widget_coords()
{
	QPoint ogl_w_c = mapFromGlobal(gaze_data_to_global());
	return std::tuple<float, float>(static_cast<float>(ogl_w_c.x()) / static_cast<float>(this->size().width()), static_cast<float>(ogl_w_c.y()) / static_cast<float>(this->size().height()));
}

void VolumeRenderWidget::smoothed_nmlzd_coords()
{
	std::tuple<float, float> new_nmlz_data = normalized_ogl_widget_coords();

	/*if (std::get<0>(new_nmlz_data) > 1.2f || std::get<0>(new_nmlz_data) < -1.2f ||
		std::get<1>(new_nmlz_data) > 1.2f || std::get<1>(new_nmlz_data) < -1.2f) {
		return;
	}*/

	if (!_gaze_data.right_eye.gaze_point.validity || !_gaze_data.left_eye.gaze_point.validity) return;

	std::get<0>(_moving_average_gaze_data_nmlz) += (1.0f / _moving_average_values) * std::get<0>(new_nmlz_data);
	std::get<1>(_moving_average_gaze_data_nmlz) += (1.0f / _moving_average_values) * std::get<1>(new_nmlz_data);

	_last_few_gaze_data.push_back(new_nmlz_data);
	if (_last_few_gaze_data.size() > _moving_average_values) {
		std::get<0>(_moving_average_gaze_data_nmlz) -= (1.f / _moving_average_values) * std::get<0>(_last_few_gaze_data.front());
		std::get<1>(_moving_average_gaze_data_nmlz) -= (1.f / _moving_average_values) * std::get<1>(_last_few_gaze_data.front());
		_last_few_gaze_data.erase(_last_few_gaze_data.begin());
	}

}

void VolumeRenderWidget::printFloatTuple(std::tuple<float, float> tp)
{
	std::cout << "(" << std::get<0>(tp) << ", " << std::get<1>(tp) << ")";
}

void VolumeRenderWidget::printQPoint(QPoint p)
{
	printFloatTuple(std::tuple<float, float>(p.x(), p.y()));
}

std::string VolumeRenderWidget::ReadFile(const char * path)
{
	std::ifstream file_stream(path);
	if (!file_stream.is_open()) {
		throw std::runtime_error(std::string("Failed to open") + path);
	}
	return std::string(std::istreambuf_iterator<char>(file_stream), std::istreambuf_iterator<char>());
}

bool VolumeRenderWidget::save_measurements(std::string file_name)
{
	std::cout << "Trying to save_measurements to: " << file_name << std::endl;

	QFile file(file_name.c_str());
	if (file.open(QFile::WriteOnly | QFile::Append)) {
		QTextStream out(&file);
		out << "BOM\n";
		out << "Frame extends: " << size().width() << ", " << size().height() << "\n";
		out << "_ms_area: " << _ms_area << "\n";
		out << "measurements taken: " << _measured_data.size() << "\n";
		out << "current rendering method: " << _renderingMethod << "\n";
		out << "is mouse replay: " << _measure_with_collected_mouse_movement_data << "\n";
		out << "Camera settings: Roation Quaternion: ( " << _rotQuat.toVector4D().x() << ", " << _rotQuat.toVector4D().y() << ", " 
			<< _rotQuat.toVector4D().z() << ", " << _rotQuat.toVector4D().w() << "), Translationsvektor: ( " << _translation.x() 
			<< ", " << _translation.y() << ", " << _translation.z() << ").\n";
		out << "System_Time\tElapsed_Time_PaintGL\tElapsed_Kernel_Time\tFrame_Coordinates\tGrid_Position\tManual_Measurement\n";
		for (auto it = _measured_data.begin(); it != _measured_data.end(); ++it) {
			out << it->system_time << "\t" << it->elapsed_millisecond_during_paintGL << "\t" 
				<< it->kernel_milliseconds << "\t" << "(" << it->frame_coordinates.x() << ", " 
				<< it->frame_coordinates.y() << ")"  << "\t" << "(" << it->position_in_Grid.x()
				<< ", " << it->position_in_Grid.y() << ")" << "\t" << it->manual_measurement 
				// << "; " << it->ms_lid 
				<< "\n";
		}
		out << "EOM\n\n";
		out.flush();
	}
	else {
		return false;
	}

	return true;
}

bool VolumeRenderWidget::save_mouse_movements(std::string file_name)
{
	std::cout << "Trying to save_mouse_movements to: " << file_name << std::endl;
	QFile file(file_name.c_str());
	if (file.open(QFile::WriteOnly)) {
		QTextStream out(&file);
		out << "Frame extends: " << size().width() << ", " << size().height() << "\n";
		out << "_ms_area: " << _ms_area << "\n";
		out << "mouse movement measurements taken: " << _mouse_mv_data.size() << "\n";
		out << "mouse_pos_x\tmouse_pos_y\tgrid_location_x\tgrid_location_y\n";
		out << "Begin\n";
		for (auto it = _mouse_mv_data.begin(); it != _mouse_mv_data.end(); ++it) {
				out << it->mouse_pos.x() << " " << it->mouse_pos.y() << " " << it->grid_location.x() << " " << it->grid_location.y() << "\n";
		}
		out.flush();
	}
	else {
		return false;
	}

	return true;
}

bool VolumeRenderWidget::load_mouse_movement(std::string file_name)
{
	std::cout << "Trying to load_mouse_movement from: " << file_name << std::endl;
	_mouse_mv_data.clear();
	
	QFile file(file_name.c_str());
	if (file.open(QFile::ReadOnly)) {
		QTextStream in(&file);
		in.setIntegerBase(10);
		while (!in.atEnd() && in.readLine() != QString("Begin")) {
			// skip to after "Begin" in File.
		}
		
		while (!in.atEnd()) {
			int mp_x, mp_y, mg_x, mg_y;
			in >> mp_x;
			in >> mp_y;
			in >> mg_x;
			in >> mg_y;

			mouse_position tmp_mp;
			tmp_mp.mouse_pos = QPoint(mp_x, mp_y);
			tmp_mp.grid_location = QPoint(mg_x, mg_y);
			_mouse_mv_data.push_back(tmp_mp);
		}

	}
	else {
		return false;
	}

	return true;
}


void VolumeRenderWidget::keyPressEvent(QKeyEvent *event) {
	// std::cout << "Key pressed: " << event->key() << std::endl;
	if (event->key() == _ms_trigger_key && !_measure_with_collected_mouse_movement_data) {
		_measurement_is_active = !_measurement_is_active;
	}

	if (event->key() == _single_measurement_key && !_measure_with_collected_mouse_movement_data) {
		// set _single_measurement to true to indicate that the next frame is measured independent to the rest of the measurements. also triggers a repaint so that the measurement can take place immediately.
		_single_measurement = true;
		update();
	}

	if (event->key() == _save_measurements_key && !_measure_with_collected_mouse_movement_data) {
		_take_measurement = false;
		_single_measurement = false;
		_measurement_is_active = false;
		save_measurements();
		_measured_data.clear();
	}

	if (event->key() == _save_mouse_movements_key && !_measure_with_collected_mouse_movement_data) {
		_collect_mouse_movement = false;
		save_mouse_movements();
	}

	if (event->key() == _trigger_collect_mouse_movement && !_measure_with_collected_mouse_movement_data) {
		if (_collect_mouse_movement) {
			_collect_mouse_movement = false;
			update();
		}
		else {
			_collect_mouse_movement = true;
		}
	}

	if (event->key() == _trigger_mouse_replay) {
		if (_measure_with_collected_mouse_movement_data) {
			// abort the current measurement by resetting the _mouse_mv_data_index to 0 and setting the _measure_with_collected_mouse_movement_data to false
			_measure_with_collected_mouse_movement_data = false;
			_mouse_mv_data_index = 0;
			_measured_data.clear();
		}
		else {
			// start the measurement. only possible if no mouse data is collected during this time but already collected mouse data exists
			if (!_collect_mouse_movement && _mouse_mv_data.size() > 0) {
				_measure_with_collected_mouse_movement_data = true;
				_mouse_mv_data_index = 0;
				
			}
		}
		update();
	}

	if (event->key() == _load_mouse_movement_data_key && !_measure_with_collected_mouse_movement_data && !_collect_mouse_movement) {
		if (load_mouse_movement()) {
			std::cout << "loaded mouse movement data." << std::endl;
		}
		else {
			std::cout << "loading of mouse movement data failed!" << std::endl;
		}
	}

	std::cout << "Variables:" << std::endl;
	std::cout << "_measurement_is_active: " << _measurement_is_active << " key should be T." << std::endl;
	std::cout << "_single_measurement: " << _single_measurement << " key should be O." << std::endl;
	std::cout << "_collect_mouse_movement: " << _collect_mouse_movement << " key should be M." << std::endl;
	std::cout << "save_measurments key should be S." << std::endl;
	std::cout << "save_mouse_movements key should be P." << std::endl;
	std::cout << "load_mouse_movement key should be L." << std::endl;
	std::cout << "_measure_with_collected_mouse_movement_data: " << _measure_with_collected_mouse_movement_data << " key should be R." << "\n\n" << std::endl;

	event->accept();
}


/**
 * @brief VolumeRenderWidget::keyReleaseEvent
 * @param event
 */
void VolumeRenderWidget::keyReleaseEvent(QKeyEvent *event)
{
	// std::cout << "released\n";
    // nothing yet
    event->accept();
}


/**
 * @brief VolumeRenderWidget::getLoadingFinished
 * @return
 */
bool VolumeRenderWidget::getLoadingFinished() const
{
    return _loadingFinished;
}


/**
 * @brief VolumeRenderWidget::setLoadingFinished
 * @param loadingFinished
 */
void VolumeRenderWidget::setLoadingFinished(bool loadingFinished)
{
    this->setTimeStep(0);
    _loadingFinished = loadingFinished;
}


/**
 * @brief VolumeRenderWidget::setCamOrtho
 * @param camOrtho
 */
void VolumeRenderWidget::setCamOrtho(bool camOrtho)
{
    _volumerender.setCamOrtho(camOrtho);
    _overlayProjMX.setToIdentity();
    if (camOrtho)
        _overlayProjMX.ortho(QRect(0, 0, width(), height()));
    else
        _overlayProjMX.perspective(53.14f, qreal(width())/qreal(height() ? height() : 1), Z_NEAR, Z_FAR);
    this->updateView();
}


/**
 * @brief VolumeRenderWidget::setIllumination
 * @param illum
 */
void VolumeRenderWidget::setIllumination(int illum)
{
    _volumerender.setIllumination(illum);
    this->updateView();
}


/**
 * @brief VolumeRenderWidget::setAmbientOcclusion
 * @param ao
 */
void VolumeRenderWidget::setAmbientOcclusion(bool ao)
{
    _volumerender.setAmbientOcclusion(ao);
    this->updateView();
}

/**
 * @brief VolumeRenderWidget::setLinearInterpolation
 * @param linear
 */
void VolumeRenderWidget::setLinearInterpolation(bool linear)
{
    _volumerender.setLinearInterpolation(linear);
    this->updateView();
}

/**
 * @brief VolumeRenderWidget::setContours
 * @param contours
 */
void VolumeRenderWidget::setContours(bool contours)
{
    _volumerender.setContours(contours);
    this->updateView();
}

/**
 * @brief VolumeRenderWidget::setAerial
 * @param aerial
 */
void VolumeRenderWidget::setAerial(bool aerial)
{
    _volumerender.setAerial(aerial);
    this->updateView();
}


/**
 * @brief VolumeRenderWidget::setDrawBox
 * @param box
 */
void VolumeRenderWidget::setDrawBox(bool box)
{
    _volumerender.setBoundingBox(box);
    this->updateView();
}


/**
 * @brief VolumeRenderWidget::setBackgroundColor
 * @param col
 */
void VolumeRenderWidget::setBackgroundColor(const QColor col)
{
    std::array<float, 4> color = {{(float)col.redF(), (float)col.greenF(),
                                   (float)col.blueF(), (float)col.alphaF()}};
    _volumerender.setBackground(color);
    this->updateView();
}


/**
 * @brief VolumeRenderWidget::calcFPS
 * calculates FPS based on the last ExecTime. If using two ore more executíon calls to OpenCL, all except the last one have to be given as offset.
 * @return
 */
double VolumeRenderWidget::calcFPS(double offset)
{
    _times.push_back(_volumerender.getLastExecTime() + offset);
    if (_times.length() > 60)
        _times.pop_front();

    double sum = 0.0;
#pragma omp parallel for reduction(+:sum)
    for (int i = 0; i < _times.size(); ++i)
        sum += _times.at(i);

    double fps = 1.0/(sum/_times.length());

//    qDebug() << fps;
    return fps;
}


/**
 * @brief VolumeRenderWidget::generateLowResVolume
 * @param factor
 */
void VolumeRenderWidget::generateLowResVolume()
{
    bool ok;
    int factor = QInputDialog::getInt(this, tr("Factor"),
                                         tr("Select downsampling factor:"), 2, 2, 64, 1, &ok);
    if (ok)
    {
        try
        {
            std::string name = _volumerender.volumeDownsampling(_timestep, factor);
            QLoggingCategory category("volumeDownSampling");
            qCInfo(category, "Successfully created down-sampled volume data set: '%s.raw'",
                   name.c_str());
        }
        catch (std::invalid_argument e)
        {
            qCritical() << e.what();
        }
        catch (std::runtime_error e)
        {
            qCritical() << e.what();
        }
    }
}

/**
 * @brief VolumeRenderWidget::read
 * @param json
 */
void VolumeRenderWidget::read(const QJsonObject &json)
{
    if (json.contains("camRotation"))
    {
        QStringList sl = json["camRotation"].toVariant().toString().split(' ');
        if (sl.length() >= 4)
        {
            _rotQuat.setScalar(sl.at(0).toFloat());
            _rotQuat.setX(sl.at(1).toFloat());
            _rotQuat.setY(sl.at(2).toFloat());
            _rotQuat.setZ(sl.at(3).toFloat());
        }
    }
    if (json.contains("camTranslation"))
    {
        QStringList sl = json["camTranslation"].toVariant().toString().split(' ');
        if (sl.length() >= 3)
        {
            _translation.setX(sl.at(0).toFloat());
            _translation.setY(sl.at(1).toFloat());
            _translation.setZ(sl.at(2).toFloat());
        }
    }

    updateView();
}

/**
 * @brief VolumeRenderWidget::write
 * @param json
 */
void VolumeRenderWidget::write(QJsonObject &json) const
{
    QString sTmp = QString::number(_rotQuat.scalar()) + " " + QString::number(_rotQuat.x())
                   + " " + QString::number(_rotQuat.y()) + " " + QString::number(_rotQuat.z());
    json["camRotation"] = sTmp;
    sTmp = QString::number(_translation.x()) + " " + QString::number(_translation.y())
            + " " + QString::number(_translation.z());
    json["camTranslation"] = sTmp;
}