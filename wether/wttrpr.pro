TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.c

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../usr/lib64/release/ -ljson-c
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../usr/lib64/debug/ -ljson-c
else:unix: LIBS += -L$$PWD/../../../../../usr/lib64/ -ljson-c

INCLUDEPATH += $$PWD/../../../../../usr/lib64
DEPENDPATH += $$PWD/../../../../../usr/lib64

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../usr/local/lib/release/ -lcurl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../usr/local/lib/debug/ -lcurl
else:unix: LIBS += -L$$PWD/../../../../../usr/local/lib/ -lcurl

INCLUDEPATH += $$PWD/../../../../../usr/local/include
DEPENDPATH += $$PWD/../../../../../usr/local/include
