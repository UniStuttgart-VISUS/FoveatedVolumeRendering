/****************************************************************************
** Meta object code from reading C++ file 'transferfunctionwidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/qt/transferfunctionwidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'transferfunctionwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.10.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ShadeWidget_t {
    QByteArrayData data[5];
    char stringdata0[54];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ShadeWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ShadeWidget_t qt_meta_stringdata_ShadeWidget = {
    {
QT_MOC_LITERAL(0, 0, 11), // "ShadeWidget"
QT_MOC_LITERAL(1, 12, 13), // "colorsChanged"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 20), // "selectedPointChanged"
QT_MOC_LITERAL(4, 48, 5) // "color"

    },
    "ShadeWidget\0colorsChanged\0\0"
    "selectedPointChanged\0color"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ShadeWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   24,    2, 0x06 /* Public */,
       3,    1,   25,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QColor,    4,

       0        // eod
};

void ShadeWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ShadeWidget *_t = static_cast<ShadeWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->colorsChanged(); break;
        case 1: _t->selectedPointChanged((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (ShadeWidget::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ShadeWidget::colorsChanged)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (ShadeWidget::*_t)(const QColor );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ShadeWidget::selectedPointChanged)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject ShadeWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_ShadeWidget.data,
      qt_meta_data_ShadeWidget,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *ShadeWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ShadeWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ShadeWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int ShadeWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void ShadeWidget::colorsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void ShadeWidget::selectedPointChanged(const QColor _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
struct qt_meta_stringdata_TransferFunctionEditor_t {
    QByteArrayData data[9];
    char stringdata0[128];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_TransferFunctionEditor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_TransferFunctionEditor_t qt_meta_stringdata_TransferFunctionEditor = {
    {
QT_MOC_LITERAL(0, 0, 22), // "TransferFunctionEditor"
QT_MOC_LITERAL(1, 23, 20), // "gradientStopsChanged"
QT_MOC_LITERAL(2, 44, 0), // ""
QT_MOC_LITERAL(3, 45, 14), // "QGradientStops"
QT_MOC_LITERAL(4, 60, 5), // "stops"
QT_MOC_LITERAL(5, 66, 20), // "selectedPointChanged"
QT_MOC_LITERAL(6, 87, 5), // "color"
QT_MOC_LITERAL(7, 93, 13), // "pointsUpdated"
QT_MOC_LITERAL(8, 107, 20) // "selectedPointUpdated"

    },
    "TransferFunctionEditor\0gradientStopsChanged\0"
    "\0QGradientStops\0stops\0selectedPointChanged\0"
    "color\0pointsUpdated\0selectedPointUpdated"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TransferFunctionEditor[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x06 /* Public */,
       5,    1,   37,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       7,    0,   40,    2, 0x0a /* Public */,
       8,    1,   41,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, QMetaType::QColor,    6,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QColor,    6,

       0        // eod
};

void TransferFunctionEditor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        TransferFunctionEditor *_t = static_cast<TransferFunctionEditor *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->gradientStopsChanged((*reinterpret_cast< const QGradientStops(*)>(_a[1]))); break;
        case 1: _t->selectedPointChanged((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 2: _t->pointsUpdated(); break;
        case 3: _t->selectedPointUpdated((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (TransferFunctionEditor::*_t)(const QGradientStops & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TransferFunctionEditor::gradientStopsChanged)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (TransferFunctionEditor::*_t)(const QColor );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TransferFunctionEditor::selectedPointChanged)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject TransferFunctionEditor::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_TransferFunctionEditor.data,
      qt_meta_data_TransferFunctionEditor,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *TransferFunctionEditor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TransferFunctionEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_TransferFunctionEditor.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int TransferFunctionEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void TransferFunctionEditor::gradientStopsChanged(const QGradientStops & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void TransferFunctionEditor::selectedPointChanged(const QColor _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
struct qt_meta_stringdata_TransferFunctionWidget_t {
    QByteArrayData data[7];
    char stringdata0[93];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_TransferFunctionWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_TransferFunctionWidget_t qt_meta_stringdata_TransferFunctionWidget = {
    {
QT_MOC_LITERAL(0, 0, 22), // "TransferFunctionWidget"
QT_MOC_LITERAL(1, 23, 21), // "resetTransferFunction"
QT_MOC_LITERAL(2, 45, 0), // ""
QT_MOC_LITERAL(3, 46, 16), // "setInterpolation"
QT_MOC_LITERAL(4, 63, 6), // "method"
QT_MOC_LITERAL(5, 70, 16), // "setColorSelected"
QT_MOC_LITERAL(6, 87, 5) // "color"

    },
    "TransferFunctionWidget\0resetTransferFunction\0"
    "\0setInterpolation\0method\0setColorSelected\0"
    "color"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TransferFunctionWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   29,    2, 0x0a /* Public */,
       3,    1,   30,    2, 0x0a /* Public */,
       5,    1,   33,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    4,
    QMetaType::Void, QMetaType::QColor,    6,

       0        // eod
};

void TransferFunctionWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        TransferFunctionWidget *_t = static_cast<TransferFunctionWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->resetTransferFunction(); break;
        case 1: _t->setInterpolation((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->setColorSelected((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject TransferFunctionWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_TransferFunctionWidget.data,
      qt_meta_data_TransferFunctionWidget,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *TransferFunctionWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TransferFunctionWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_TransferFunctionWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int TransferFunctionWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
