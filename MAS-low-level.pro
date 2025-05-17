QT += core network
QT += serialport

CONFIG += c++17 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        cs_rov.cpp \
        kx_pult/configdata.cpp \
        kx_pult/kx_protocol.cpp \
        kx_pult/qkx_coeffs.cpp \
        kx_pult/qpiconfig.cpp \
        main.cpp \
        protocol-BW-AH127C/AH127Cprotocol.cpp \
        protocol_PS_MS5837/ms5837.cpp \
        protocol_VMA/vma_controller.cpp \
        protocol_manipulator/manipulator.cpp


LIBS += -L"/home/hydronautics/rpi/sysroot/usr/lib"
LIBS += -lwiringPi
LIBS += -L"/home/hydronautics/rpi/sysroot/usr/lib/arm-linux-gnueabihf"
LIBS += -li2c

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    protocol-BW-AH127C/AH127Cprotocol.h \
    cs_rov.h \
    kx_pult/configdata.h \
    kx_pult/kx_protocol.h \
    kx_pult/qkx_coeffs.h \
    kx_pult/qpiconfig.h \
    protocol_pult/pc_protocol.h \
    protocol_pult/protocol.h \
    protocol_pult/udp_protocol.h \
    protocol_VMA/vma_controller.h \
    protocol_PS_MS5837/ms5837.h \
    protocol_manipulator/manipulator.h
