/****************************************************************************
** Meta object code from reading C++ file 'protocoluwb.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "parserUWB/protocoluwb.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'protocoluwb.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_UWB__ProtocolUWB_t {
    QByteArrayData data[14];
    char stringdata0[111];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_UWB__ProtocolUWB_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_UWB__ProtocolUWB_t qt_meta_stringdata_UWB__ProtocolUWB = {
    {
QT_MOC_LITERAL(0, 0, 16), // "UWB::ProtocolUWB"
QT_MOC_LITERAL(1, 17, 7), // "started"
QT_MOC_LITERAL(2, 25, 0), // ""
QT_MOC_LITERAL(3, 26, 8), // "finished"
QT_MOC_LITERAL(4, 35, 8), // "renewMSG"
QT_MOC_LITERAL(5, 44, 10), // "RecDataUWB"
QT_MOC_LITERAL(6, 55, 3), // "msg"
QT_MOC_LITERAL(7, 59, 11), // "renewMapMsg"
QT_MOC_LITERAL(8, 71, 5), // "renew"
QT_MOC_LITERAL(9, 77, 8), // "uint16_t"
QT_MOC_LITERAL(10, 86, 4), // "size"
QT_MOC_LITERAL(11, 91, 8), // "tick_UWB"
QT_MOC_LITERAL(12, 100, 5), // "start"
QT_MOC_LITERAL(13, 106, 4) // "stop"

    },
    "UWB::ProtocolUWB\0started\0\0finished\0"
    "renewMSG\0RecDataUWB\0msg\0renewMapMsg\0"
    "renew\0uint16_t\0size\0tick_UWB\0start\0"
    "stop"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_UWB__ProtocolUWB[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   54,    2, 0x06 /* Public */,
       3,    0,   55,    2, 0x06 /* Public */,
       4,    1,   56,    2, 0x06 /* Public */,
       7,    1,   59,    2, 0x06 /* Public */,
       8,    1,   62,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      11,    0,   65,    2, 0x08 /* Private */,
      12,    0,   66,    2, 0x0a /* Public */,
      13,    0,   67,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 5,    6,
    QMetaType::Void, 0x80000000 | 5,    6,
    QMetaType::Void, 0x80000000 | 9,   10,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void UWB::ProtocolUWB::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ProtocolUWB *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->started(); break;
        case 1: _t->finished(); break;
        case 2: _t->renewMSG((*reinterpret_cast< RecDataUWB(*)>(_a[1]))); break;
        case 3: _t->renewMapMsg((*reinterpret_cast< RecDataUWB(*)>(_a[1]))); break;
        case 4: _t->renew((*reinterpret_cast< uint16_t(*)>(_a[1]))); break;
        case 5: _t->tick_UWB(); break;
        case 6: _t->start(); break;
        case 7: _t->stop(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ProtocolUWB::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ProtocolUWB::started)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ProtocolUWB::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ProtocolUWB::finished)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ProtocolUWB::*)(RecDataUWB );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ProtocolUWB::renewMSG)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (ProtocolUWB::*)(RecDataUWB );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ProtocolUWB::renewMapMsg)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (ProtocolUWB::*)(uint16_t );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ProtocolUWB::renew)) {
                *result = 4;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject UWB::ProtocolUWB::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_UWB__ProtocolUWB.data,
    qt_meta_data_UWB__ProtocolUWB,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *UWB::ProtocolUWB::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *UWB::ProtocolUWB::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_UWB__ProtocolUWB.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int UWB::ProtocolUWB::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void UWB::ProtocolUWB::started()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void UWB::ProtocolUWB::finished()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void UWB::ProtocolUWB::renewMSG(RecDataUWB _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void UWB::ProtocolUWB::renewMapMsg(RecDataUWB _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void UWB::ProtocolUWB::renew(uint16_t _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
