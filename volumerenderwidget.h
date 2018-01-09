#ifndef VOLUMERENDERWIDGET_H
#define VOLUMERENDERWIDGET_H

#include <QObject>
#include <QWidget>
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

    void updateView(float dx = 0, float dy = 0);

    bool getLoadingFinished() const;
    void setLoadingFinished(bool loadingFinished);

public slots:
    void cleanup();
    void resetCam();

    void updateSamplingRate(double samplingRate);
    void updateTransferFunction(QGradientStops stops);
    std::vector<unsigned char> getRawTransferFunction(QGradientStops stops) const;

#undef Bool
    void setTffInterpolation(const QString method);
    void setCamOrtho(bool camOrtho);
    void setIllumination(bool illum);
    void setLinearInterpolation(bool linear);
    void setDrawBox(bool box);
    void setBackgroundColor(const QColor col);
    void setImageSamplingRate(const double samplingRate);

    void saveFrame();
    void setTimeStep(int timestep);
    void setAmbientOcclusion(bool ao);
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
    double calcFPS();

    void generateOutputTextures(int width, int height);

    // -------Members--------
    //
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
    GLuint _outTexId;
    VolumeRenderCL _volumerender;
    QEasingCurve _tffInterpol;
    int _timestep;

    // global rendering flags
    QPoint _lastLocalCursorPos;
    QQuaternion _rotQuat;
    QVector3D _translation;

    bool _noUpdate;
    bool _loadingFinished;
    bool _writeImage;
    qint64 _imgCount;
    QVector<double> _times;
    double _imgSamplingRate;       // image oversampling rate
};

#endif // VOLUMERENDERWIDGET_H
