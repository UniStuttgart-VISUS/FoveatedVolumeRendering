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
#include <QTime>


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

	enum RenderingMethod { STANDARD, DISTANCE_DC, SQUARE_DC, TRI};
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

    void setOutputTextures(int width, int height, GLuint texture, GLuint tex_unit, bool recreateTexture = true);

	void paintGL_standard();
	void paintGL_distance_dc(); // elliptical discards in multiple rendering calls
	void paintGL_square_dc(); // square with discard
	void paintGL_ThreeRenderInvocations(); // not implemented

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

	// Functions and Datastructures to mesure performance
	struct ms_data {
		// struct to hold the measured data
		QString system_time; // the system time when the measurement of this data begin i.e. the paint method related to this measurement was triggered.
		int elapsed_millisecond_during_paintGL; // the amount of milliseconds the method paintGL() did need for computation
		double kernel_milliseconds; // the amount of milliseconds the profiling of the kernel returned. Those are also used for the computation of the fps drawn on screen. They are stored as double because on good hardware they can have small values which are not good to be represented in int.
		QPoint frame_coordinates; // the coordinates of the cursor for inside the opengl widget for which the frame was computed
		bool manual_measurement; // indicates whether this measurement was triggered manually.
		QPoint position_in_Grid; // the position in the grid which has been divided by the value of _ms_area; this value is elementwise intgerdivision of frame_coordinates by _ms_area.
		// int ms_lid; // lokal id of measurement
	};
	std::vector<ms_data> _measured_data; // vector to store the data of the last measurement. it's data will be cleared when a new measurement is started or its data has been written to a file
	
	ms_data _tmp_ms; // A temporary measurement to store temporary data of a measurement.
	
	QTime _time; // a QTime object which will be set to QTime::currentTime() at construction and can be used to measure elapsed times.
	bool _measurement_is_active; // holds the state whether measurements are to be taken or not. true <=> yes, false otherwise. it is set false to begin of the application.
	bool _single_measurement; // tells that the next frame will be measured as single measurement and will not count as frame that has been measured inside a specific grid.
	const Qt::Key _ms_trigger_key = Qt::Key_M; // the key to be pressed to trigger the value of: _measurement_is_active
	const Qt::Key _single_measurement_key = Qt::Key_K; // the key to be pressed to trigger a single measurement.
	const Qt::Key _save_measurements_key = Qt::Key_P; // pressing this key will save the measurements by calling save_measurements() and cleans the to this moment collected data. also disables all vlaues indicating, that a measurement has to be taken.
	const int _ms_area = 10; // the extends of one measured area; no measurements are taken if they are in the same area as the measurement taken before
	// int _curr_ms_id = 0; // the current lid of the measurement. used to find the right one.
	bool _take_measurement = false; // is used to indicate between different functions whether to add data to the last object in the measurement or not.

	bool save_measurements(std::string file_name = std::string("ms_data.txt")); // will save the current data in _measured_data to a text file named ms_data.txt if not else specified.

	/*
	* Mouse Movement replaying:
	* if _collect_mouse_movement == true, the frame will not be updated and the user can use the mouse to follow a path on the screen or the currently set image.
	* Therefore, the mouse_movement events have to be disabled during this time.
	* if _collect_mouse_movement is set to false (after it was true), the screen will be updated normally.
	* If _measure_with_collected_mouse_movement_data is true and collected mouse movement data exists, each frame that will be drawn next will have new values for the current mouse_position.
	* during this time, the mouse_event handling method has to be deactivated as well.
	*/

	struct mouse_position {
		// struct to store the mouse position and the grid it is in
		QPoint mouse_pos;
		QPoint grid_location; // position in grid according to _ms_area;
	};

	std::vector<mouse_position> mouse_mv_data; // Data structure to collect multiple mouse_position structs.

	bool _collect_mouse_movement = false; // tells whether mouse movement data is to be collected
	bool _measure_with_collected_mouse_movement_data = false; // tells whether the next measurements are done with the collected mouse data
	const Qt::Key _trigger_collect_mouse_movement;

    // -------Members--------
    //
	// Eyetracker
	TobiiResearchEyeTracker* _eyetracker;
	TobiiResearchGazeData _gaze_data;
	std::vector<std::tuple<float, float>> _last_few_gaze_data;
	std::tuple<float, float> _moving_average_gaze_data_nmlz;
	const int _moving_average_values = 3;
	QPoint _monitor_offset;
	int _curr_monitor_width;
	int _curr_monitor_height;

	// distance dc and partly TRI
	cl_float3 _g_values; // TRI, first is lowest resolution (highest g), third is normally always 1 (native resolution)
	std::tuple<float, float> _innerEllipse;
	std::tuple<float, float> _outerEllipse;

	// square dc
	std::array<int, 2> _rect_extends;

	// TRI
	std::tuple<float, float> _circle_radiuses; // Circle radiuses for inner circle (first) and outer cirlce (second) in pixel

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