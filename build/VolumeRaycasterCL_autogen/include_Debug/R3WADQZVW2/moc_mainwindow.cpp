/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/qt/mainwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.10.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[19];
    char stringdata0[277];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "MainWindow"
QT_MOC_LITERAL(1, 11, 14), // "openVolumeFile"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 11), // "addProgress"
QT_MOC_LITERAL(4, 39, 15), // "finishedLoading"
QT_MOC_LITERAL(5, 55, 7), // "loadTff"
QT_MOC_LITERAL(6, 63, 10), // "loadRawTff"
QT_MOC_LITERAL(7, 74, 7), // "saveTff"
QT_MOC_LITERAL(8, 82, 10), // "saveRawTff"
QT_MOC_LITERAL(9, 93, 26), // "loadIndex_and_Sampling_Map"
QT_MOC_LITERAL(10, 120, 21), // "chooseBackgroundColor"
QT_MOC_LITERAL(11, 142, 12), // "saveCamState"
QT_MOC_LITERAL(12, 155, 12), // "loadCamState"
QT_MOC_LITERAL(13, 168, 15), // "showAboutDialog"
QT_MOC_LITERAL(14, 184, 39), // "updateTransferFunctionFromGra..."
QT_MOC_LITERAL(15, 224, 16), // "setLoopTimesteps"
QT_MOC_LITERAL(16, 241, 12), // "nextTimestep"
QT_MOC_LITERAL(17, 254, 16), // "setPlaybackSpeed"
QT_MOC_LITERAL(18, 271, 5) // "speed"

    },
    "MainWindow\0openVolumeFile\0\0addProgress\0"
    "finishedLoading\0loadTff\0loadRawTff\0"
    "saveTff\0saveRawTff\0loadIndex_and_Sampling_Map\0"
    "chooseBackgroundColor\0saveCamState\0"
    "loadCamState\0showAboutDialog\0"
    "updateTransferFunctionFromGradientStops\0"
    "setLoopTimesteps\0nextTimestep\0"
    "setPlaybackSpeed\0speed"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      16,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   94,    2, 0x09 /* Protected */,
       3,    0,   95,    2, 0x09 /* Protected */,
       4,    0,   96,    2, 0x09 /* Protected */,
       5,    0,   97,    2, 0x09 /* Protected */,
       6,    0,   98,    2, 0x09 /* Protected */,
       7,    0,   99,    2, 0x09 /* Protected */,
       8,    0,  100,    2, 0x09 /* Protected */,
       9,    0,  101,    2, 0x09 /* Protected */,
      10,    0,  102,    2, 0x09 /* Protected */,
      11,    0,  103,    2, 0x09 /* Protected */,
      12,    0,  104,    2, 0x09 /* Protected */,
      13,    0,  105,    2, 0x09 /* Protected */,
      14,    0,  106,    2, 0x09 /* Protected */,
      15,    0,  107,    2, 0x09 /* Protected */,
      16,    0,  108,    2, 0x09 /* Protected */,
      17,    1,  109,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   18,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MainWindow *_t = static_cast<MainWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->openVolumeFile(); break;
        case 1: _t->addProgress(); break;
        case 2: _t->finishedLoading(); break;
        case 3: _t->loadTff(); break;
        case 4: _t->loadRawTff(); break;
        case 5: _t->saveTff(); break;
        case 6: _t->saveRawTff(); break;
        case 7: _t->loadIndex_and_Sampling_Map(); break;
        case 8: _t->chooseBackgroundColor(); break;
        case 9: _t->saveCamState(); break;
        case 10: _t->loadCamState(); break;
        case 11: _t->showAboutDialog(); break;
        case 12: _t->updateTransferFunctionFromGradientStops(); break;
        case 13: _t->setLoopTimesteps(); break;
        case 14: _t->nextTimestep(); break;
        case 15: _t->setPlaybackSpeed((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow.data,
      qt_meta_data_MainWindow,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 16)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 16;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
