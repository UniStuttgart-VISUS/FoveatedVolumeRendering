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

#include <QObject>
#include <qcheckbox.h>
#include <QWidget>
#include <QDir>
#include <QPointer>
#include <QQuaternion>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QOpenGLTexture>
#include <QPropertyAnimation>
#include <qopenglfunctions_4_3_core.h>
#include <QPainter>
#include <qnumeric.h>


#include <TOBIIRESEARCH\tobii_research.h>
#include <TOBIIRESEARCH\tobii_research_eyetracker.h>
#include <TOBIIRESEARCH\tobii_research_streams.h>
#include <TOBIIRESEARCH\tobii_research_calibration.h>

#include "volumerendercl.h"

class VolumeRenderWidget : public QOpenGLWidget, protected QOpenGLFunctions_4_3_Core
{
    Q_OBJECT

public:
    explicit VolumeRenderWidget(QWidget *parent = 0);
    virtual ~VolumeRenderWidget();

    void setupVertexAttribs();

    void setVolumeData(const QString &fileName);

    bool hasData();

    const QVector3D getVolumeResolution();

    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;

    void mouseDoubleClickEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

    void keyReleaseEvent(QKeyEvent *event) Q_DECL_OVERRIDE;

	void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;

    void updateView(float dx = 0, float dy = 0);


    bool getLoadingFinished() const;
    void setLoadingFinished(bool loadingFinished);

    QVector3D getCamTranslation() const;
    void setCamTranslation(const QVector3D &translation);

    QQuaternion getCamRotation() const;
    void setCamRotation(const QQuaternion &rotQuat);

	enum RenderingMethod { STANDARD, DISTANCE_DC, SQUARE_DC, SINUS_RESOLUTION};
	void setRenderingMethod(int rm);


public slots:
    void cleanup();
    void resetCam();

    void updateSamplingRate(double samplingRate);
    void updateTransferFunction(QGradientStops stops);
    std::vector<unsigned char> getRawTransferFunction(QGradientStops stops) const;
    void setRawTransferFunction(std::vector<unsigned char> tff);

#undef Bool
    void setTffInterpolation(const QString method);
    void setCamOrtho(bool camOrtho);
    void setIllumination(int illum);
    void setLinearInterpolation(bool linear);
    void setContours(bool contours);
    void setAerial(bool aerial);
    void setDrawBox(bool box);
    void setBackgroundColor(const QColor col);
    void setImageSamplingRate(const double samplingRate);
    void setShowOverlay(bool showOverlay);
	void setEyetracking(bool eyetracking);

	// distance dc
	void setGValues(float c, float b, float a = 1.0f);
	void setInnerEllipseExtends(float rx, float ry);
	void setOuterEllipseExtends(float rx, float ry);
	void showGValuesSettings();
	void showinnerEllipseSettings();
	void showouterEllipseSettings();


	// square dc
	void setRectExtends(int width, int height);
	void showRectExtendsSettings();

    void saveFrame();
    void toggleVideoRecording();
    void setTimeStep(int timestep);
    void setAmbientOcclusion(bool ao);

    void generateLowResVolume();

    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;

    void showSelectOpenCL();
	void showSelectEyetrackingDevice();

	static bool MonitorEnumProc(HMONITOR monitor, HDC hdcMnitor, LPRECT rect, LPARAM param);
	void actionSelectMonitor();
signals:
    void fpsChanged(double);
    void frameSizeChanged(QSize);
    void timeSeriesLoaded(int);

protected:
    // Qt specific QOpenGLWidget methods
    void initializeGL() Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
    void resizeGL(int w, int h) Q_DECL_OVERRIDE;

private:
    void paintOrientationAxis(QPainter &p);
    void paintFPS(QPainter &p, const double fps, const double lastTime);
    double calcFPS(double offset = 0.0);

    void setOutputTextures(int width, int height, GLuint texture, GLuint tex_unit);

	void paintGL_standard();
	void paintGL_distance_dc(); // elliptical discards in multiple rendering calls
	void paintGL_square_dc(); // square with discard
	void paintGL_SinusResolution(); // not implemented

	bool check_eyetracker_availability();
	static void gaze_data_callback(TobiiResearchGazeData *gaze_data, void *user_data);
	QPoint gaze_data_to_global(); // returns a QPoint for the gaze_data in virtual desktop coordinates (global)
	std::tuple<float, float> normalized_ogl_widget_coords(); // returns the gaze_data normalized to the opengl widget
	void smoothed_nmlzd_coords();	// updates the _moving_average_gaze_data_nmlz variable


	// Debug helper functions
	void printFloatTuple(std::tuple<float, float> tp);
	void printQPoint(QPoint p);

	// file loading helper function
	static std::string ReadFile(const char *path); // returns the content of a file


    // -------Members--------
    //
	// Eyetracker
	TobiiResearchEyeTracker* _eyetracker;
	TobiiResearchGazeData _gaze_data;
	std::vector<std::tuple<float, float>> _last_few_gaze_data;
	std::tuple<float, float> _moving_average_gaze_data_nmlz;
	const int _moving_average_values = 1;
	QPoint _monitor_offset;
	int _curr_monitor_width;
	int _curr_monitor_height;

	// distance dc
	cl_float3 _g_values;
	std::tuple<float, float> _innerEllipse;
	std::tuple<float, float> _outerEllipse;

	// square dc
	std::array<int, 2> _rect_extends;

    // OpenGL
    QOpenGLVertexArrayObject _screenQuadVao;
    QOpenGLShaderProgram _spScreenQuad;
    QOpenGLShaderProgram _spOverlaysGL;
    QOpenGLBuffer _quadVbo;
    GLuint _overlayFboId;
    GLuint _overlayTexId;

    QMatrix4x4 _screenQuadProjMX;
    QMatrix4x4 _viewMX;
    QMatrix4x4 _modelMX;
    QMatrix4x4 _coordViewMX;
    QMatrix4x4 _overlayProjMX;
    QMatrix4x4 _overlayModelMX;

    QPoint _tffRange;
    GLuint _outTexId0;	// bound to Texture unit 0
	GLuint _outTexId1;	// bound to texture unit 1
    VolumeRenderCL _volumerender;
    QEasingCurve _tffInterpol;
    int _timestep;

    // global rendering flags
    QPoint _lastLocalCursorPos;
    QQuaternion _rotQuat;
    QVector3D _translation;

	bool _useEyetracking;
    bool _noUpdate;
    bool _loadingFinished;
    bool _writeImage;
    bool _recordVideo;
    qint64 _imgCount;
    QVector<double> _times;
    double _imgSamplingRate;       // image oversampling rate
    bool _useGL;
    bool _showOverlay;
	RenderingMethod _renderingMethod;
};