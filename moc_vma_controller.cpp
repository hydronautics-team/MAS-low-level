/****************************************************************************
** Meta object code from reading C++ file 'vma_controller.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "NIR/VMA_controller/vma_controller.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'vma_controller.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_VMA_controller_t {
    QByteArrayData data[14];
    char stringdata0[83];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_VMA_controller_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_VMA_controller_t qt_meta_stringdata_VMA_controller = {
    {
QT_MOC_LITERAL(0, 0, 14), // "VMA_controller"
QT_MOC_LITERAL(1, 15, 7), // "started"
QT_MOC_LITERAL(2, 23, 0), // ""
QT_MOC_LITERAL(3, 24, 8), // "finished"
QT_MOC_LITERAL(4, 33, 5), // "start"
QT_MOC_LITERAL(5, 39, 4), // "stop"
QT_MOC_LITERAL(6, 44, 9), // "setValues"
QT_MOC_LITERAL(7, 54, 3), // "Upl"
QT_MOC_LITERAL(8, 58, 3), // "Upp"
QT_MOC_LITERAL(9, 62, 3), // "Ucl"
QT_MOC_LITERAL(10, 66, 3), // "Ucp"
QT_MOC_LITERAL(11, 70, 3), // "Uzl"
QT_MOC_LITERAL(12, 74, 3), // "Uzp"
QT_MOC_LITERAL(13, 78, 4) // "tick"

    },
    "VMA_controller\0started\0\0finished\0start\0"
    "stop\0setValues\0Upl\0Upp\0Ucl\0Ucp\0Uzl\0"
    "Uzp\0tick"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_VMA_controller[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   44,    2, 0x06 /* Public */,
       3,    0,   45,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    0,   46,    2, 0x0a /* Public */,
       5,    0,   47,    2, 0x0a /* Public */,
       6,    6,   48,    2, 0x0a /* Public */,
      13,    0,   61,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Float, QMetaType::Float, QMetaType::Float, QMetaType::Float, QMetaType::Float, QMetaType::Float,    7,    8,    9,   10,   11,   12,
    QMetaType::Void,

       0        // eod
};

void VMA_controller::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<VMA_controller *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->started(); break;
        case 1: _t->finished(); break;
        case 2: _t->start(); break;
        case 3: _t->stop(); break;
        case 4: _t->setValues((*reinterpret_cast< const float(*)>(_a[1])),(*reinterpret_cast< const float(*)>(_a[2])),(*reinterpret_cast< const float(*)>(_a[3])),(*reinterpret_cast< const float(*)>(_a[4])),(*reinterpret_cast< const float(*)>(_a[5])),(*reinterpret_cast< const float(*)>(_a[6]))); break;
        case 5: _t->tick(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (VMA_controller::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&VMA_controller::started)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (VMA_controller::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&VMA_controller::finished)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject VMA_controller::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_VMA_controller.data,
    qt_meta_data_VMA_controller,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *VMA_controller::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *VMA_controller::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_VMA_controller.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int VMA_controller::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void VMA_controller::started()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void VMA_controller::finished()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
