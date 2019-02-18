/****************************************************************************
** Meta object code from reading C++ file 'volumerenderwidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/qt/volumerenderwidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'volumerenderwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.10.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_VolumeRenderWidget_t {
    QByteArrayData data[54];
    char stringdata0[704];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_VolumeRenderWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_VolumeRenderWidget_t qt_meta_stringdata_VolumeRenderWidget = {
    {
QT_MOC_LITERAL(0, 0, 18), // "VolumeRenderWidget"
QT_MOC_LITERAL(1, 19, 10), // "fpsChanged"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 16), // "frameSizeChanged"
QT_MOC_LITERAL(4, 48, 16), // "timeSeriesLoaded"
QT_MOC_LITERAL(5, 65, 7), // "cleanup"
QT_MOC_LITERAL(6, 73, 8), // "resetCam"
QT_MOC_LITERAL(7, 82, 18), // "updateSamplingRate"
QT_MOC_LITERAL(8, 101, 12), // "samplingRate"
QT_MOC_LITERAL(9, 114, 22), // "updateTransferFunction"
QT_MOC_LITERAL(10, 137, 14), // "QGradientStops"
QT_MOC_LITERAL(11, 152, 5), // "stops"
QT_MOC_LITERAL(12, 158, 22), // "getRawTransferFunction"
QT_MOC_LITERAL(13, 181, 26), // "std::vector<unsigned char>"
QT_MOC_LITERAL(14, 208, 22), // "setRawTransferFunction"
QT_MOC_LITERAL(15, 231, 3), // "tff"
QT_MOC_LITERAL(16, 235, 19), // "setTffInterpolation"
QT_MOC_LITERAL(17, 255, 6), // "method"
QT_MOC_LITERAL(18, 262, 11), // "setCamOrtho"
QT_MOC_LITERAL(19, 274, 8), // "camOrtho"
QT_MOC_LITERAL(20, 283, 16), // "setContRendering"
QT_MOC_LITERAL(21, 300, 15), // "setIllumination"
QT_MOC_LITERAL(22, 316, 5), // "illum"
QT_MOC_LITERAL(23, 322, 22), // "setLinearInterpolation"
QT_MOC_LITERAL(24, 345, 6), // "linear"
QT_MOC_LITERAL(25, 352, 11), // "setContours"
QT_MOC_LITERAL(26, 364, 8), // "contours"
QT_MOC_LITERAL(27, 373, 9), // "setAerial"
QT_MOC_LITERAL(28, 383, 6), // "aerial"
QT_MOC_LITERAL(29, 390, 9), // "setImgEss"
QT_MOC_LITERAL(30, 400, 6), // "useEss"
QT_MOC_LITERAL(31, 407, 9), // "setObjEss"
QT_MOC_LITERAL(32, 417, 10), // "setDrawBox"
QT_MOC_LITERAL(33, 428, 3), // "box"
QT_MOC_LITERAL(34, 432, 18), // "setBackgroundColor"
QT_MOC_LITERAL(35, 451, 3), // "col"
QT_MOC_LITERAL(36, 455, 20), // "setImageSamplingRate"
QT_MOC_LITERAL(37, 476, 14), // "setShowOverlay"
QT_MOC_LITERAL(38, 491, 11), // "showOverlay"
QT_MOC_LITERAL(39, 503, 9), // "saveFrame"
QT_MOC_LITERAL(40, 513, 20), // "toggleVideoRecording"
QT_MOC_LITERAL(41, 534, 19), // "toggleViewRecording"
QT_MOC_LITERAL(42, 554, 24), // "toggleInteractionLogging"
QT_MOC_LITERAL(43, 579, 11), // "setTimeStep"
QT_MOC_LITERAL(44, 591, 8), // "timestep"
QT_MOC_LITERAL(45, 600, 19), // "setAmbientOcclusion"
QT_MOC_LITERAL(46, 620, 2), // "ao"
QT_MOC_LITERAL(47, 623, 20), // "generateLowResVolume"
QT_MOC_LITERAL(48, 644, 4), // "read"
QT_MOC_LITERAL(49, 649, 4), // "json"
QT_MOC_LITERAL(50, 654, 5), // "write"
QT_MOC_LITERAL(51, 660, 12), // "QJsonObject&"
QT_MOC_LITERAL(52, 673, 16), // "showSelectOpenCL"
QT_MOC_LITERAL(53, 690, 13) // "reloadKernels"

    },
    "VolumeRenderWidget\0fpsChanged\0\0"
    "frameSizeChanged\0timeSeriesLoaded\0"
    "cleanup\0resetCam\0updateSamplingRate\0"
    "samplingRate\0updateTransferFunction\0"
    "QGradientStops\0stops\0getRawTransferFunction\0"
    "std::vector<unsigned char>\0"
    "setRawTransferFunction\0tff\0"
    "setTffInterpolation\0method\0setCamOrtho\0"
    "camOrtho\0setContRendering\0setIllumination\0"
    "illum\0setLinearInterpolation\0linear\0"
    "setContours\0contours\0setAerial\0aerial\0"
    "setImgEss\0useEss\0setObjEss\0setDrawBox\0"
    "box\0setBackgroundColor\0col\0"
    "setImageSamplingRate\0setShowOverlay\0"
    "showOverlay\0saveFrame\0toggleVideoRecording\0"
    "toggleViewRecording\0toggleInteractionLogging\0"
    "setTimeStep\0timestep\0setAmbientOcclusion\0"
    "ao\0generateLowResVolume\0read\0json\0"
    "write\0QJsonObject&\0showSelectOpenCL\0"
    "reloadKernels"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_VolumeRenderWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      33,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,  179,    2, 0x06 /* Public */,
       3,    1,  182,    2, 0x06 /* Public */,
       4,    1,  185,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    0,  188,    2, 0x0a /* Public */,
       6,    0,  189,    2, 0x0a /* Public */,
       7,    1,  190,    2, 0x0a /* Public */,
       9,    1,  193,    2, 0x0a /* Public */,
      12,    1,  196,    2, 0x0a /* Public */,
      14,    1,  199,    2, 0x0a /* Public */,
      16,    1,  202,    2, 0x0a /* Public */,
      18,    1,  205,    2, 0x0a /* Public */,
      20,    1,  208,    2, 0x0a /* Public */,
      21,    1,  211,    2, 0x0a /* Public */,
      23,    1,  214,    2, 0x0a /* Public */,
      25,    1,  217,    2, 0x0a /* Public */,
      27,    1,  220,    2, 0x0a /* Public */,
      29,    1,  223,    2, 0x0a /* Public */,
      31,    1,  226,    2, 0x0a /* Public */,
      32,    1,  229,    2, 0x0a /* Public */,
      34,    1,  232,    2, 0x0a /* Public */,
      36,    1,  235,    2, 0x0a /* Public */,
      37,    1,  238,    2, 0x0a /* Public */,
      39,    0,  241,    2, 0x0a /* Public */,
      40,    0,  242,    2, 0x0a /* Public */,
      41,    0,  243,    2, 0x0a /* Public */,
      42,    0,  244,    2, 0x0a /* Public */,
      43,    1,  245,    2, 0x0a /* Public */,
      45,    1,  248,    2, 0x0a /* Public */,
      47,    0,  251,    2, 0x0a /* Public */,
      48,    1,  252,    2, 0x0a /* Public */,
      50,    1,  255,    2, 0x0a /* Public */,
      52,    0,  258,    2, 0x0a /* Public */,
      53,    0,  259,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void, QMetaType::QSize,    2,
    QMetaType::Void, QMetaType::Int,    2,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Double,    8,
    QMetaType::Void, 0x80000000 | 10,   11,
    0x80000000 | 13, 0x80000000 | 10,   11,
    QMetaType::Void, 0x80000000 | 13,   15,
    QMetaType::Void, QMetaType::QString,   17,
    QMetaType::Void, QMetaType::Bool,   19,
    QMetaType::Void, QMetaType::Bool,   20,
    QMetaType::Void, QMetaType::Int,   22,
    QMetaType::Void, QMetaType::Bool,   24,
    QMetaType::Void, QMetaType::Bool,   26,
    QMetaType::Void, QMetaType::Bool,   28,
    QMetaType::Void, QMetaType::Bool,   30,
    QMetaType::Void, QMetaType::Bool,   30,
    QMetaType::Void, QMetaType::Bool,   33,
    QMetaType::Void, QMetaType::QColor,   35,
    QMetaType::Void, QMetaType::Double,    8,
    QMetaType::Void, QMetaType::Bool,   38,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   44,
    QMetaType::Void, QMetaType::Bool,   46,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QJsonObject,   49,
    QMetaType::Void, 0x80000000 | 51,   49,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void VolumeRenderWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        VolumeRenderWidget *_t = static_cast<VolumeRenderWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->fpsChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 1: _t->frameSizeChanged((*reinterpret_cast< QSize(*)>(_a[1]))); break;
        case 2: _t->timeSeriesLoaded((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->cleanup(); break;
        case 4: _t->resetCam(); break;
        case 5: _t->updateSamplingRate((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 6: _t->updateTransferFunction((*reinterpret_cast< QGradientStops(*)>(_a[1]))); break;
        case 7: { std::vector<unsigned char> _r = _t->getRawTransferFunction((*reinterpret_cast< QGradientStops(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< std::vector<unsigned char>*>(_a[0]) = std::move(_r); }  break;
        case 8: _t->setRawTransferFunction((*reinterpret_cast< std::vector<unsigned char>(*)>(_a[1]))); break;
        case 9: _t->setTffInterpolation((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 10: _t->setCamOrtho((*reinterpret_cast< const bool(*)>(_a[1]))); break;
        case 11: _t->setContRendering((*reinterpret_cast< const bool(*)>(_a[1]))); break;
        case 12: _t->setIllumination((*reinterpret_cast< const int(*)>(_a[1]))); break;
        case 13: _t->setLinearInterpolation((*reinterpret_cast< const bool(*)>(_a[1]))); break;
        case 14: _t->setContours((*reinterpret_cast< const bool(*)>(_a[1]))); break;
        case 15: _t->setAerial((*reinterpret_cast< const bool(*)>(_a[1]))); break;
        case 16: _t->setImgEss((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 17: _t->setObjEss((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 18: _t->setDrawBox((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 19: _t->setBackgroundColor((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 20: _t->setImageSamplingRate((*reinterpret_cast< const double(*)>(_a[1]))); break;
        case 21: _t->setShowOverlay((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 22: _t->saveFrame(); break;
        case 23: _t->toggleVideoRecording(); break;
        case 24: _t->toggleViewRecording(); break;
        case 25: _t->toggleInteractionLogging(); break;
        case 26: _t->setTimeStep((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 27: _t->setAmbientOcclusion((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 28: _t->generateLowResVolume(); break;
        case 29: _t->read((*reinterpret_cast< const QJsonObject(*)>(_a[1]))); break;
        case 30: _t->write((*reinterpret_cast< QJsonObject(*)>(_a[1]))); break;
        case 31: _t->showSelectOpenCL(); break;
        case 32: _t->reloadKernels(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (VolumeRenderWidget::*_t)(double );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&VolumeRenderWidget::fpsChanged)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (VolumeRenderWidget::*_t)(QSize );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&VolumeRenderWidget::frameSizeChanged)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (VolumeRenderWidget::*_t)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&VolumeRenderWidget::timeSeriesLoaded)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject VolumeRenderWidget::staticMetaObject = {
    { &QOpenGLWidget::staticMetaObject, qt_meta_stringdata_VolumeRenderWidget.data,
      qt_meta_data_VolumeRenderWidget,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *VolumeRenderWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *VolumeRenderWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_VolumeRenderWidget.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "QOpenGLFunctions_4_3_Core"))
        return static_cast< QOpenGLFunctions_4_3_Core*>(this);
    return QOpenGLWidget::qt_metacast(_clname);
}

int VolumeRenderWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QOpenGLWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 33)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 33;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 33)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 33;
    }
    return _id;
}

// SIGNAL 0
void VolumeRenderWidget::fpsChanged(double _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void VolumeRenderWidget::frameSizeChanged(QSize _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void VolumeRenderWidget::timeSeriesLoaded(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
