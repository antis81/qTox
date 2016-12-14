#-------------------------------------------------
#
# Project created by QtCreator 2014-06-22T14:07:35
#
#-------------------------------------------------

#    This file is part of qTox, a Qt-based graphical interface for Tox.
#
#    This program is libre software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#
#    See the COPYING file for more details.


QT       += core network xml opengl sql svg qml quick

TARGET    = qtox
TEMPLATE  = app
INCLUDEPATH += src

CONFIG   += c++11
CONFIG   += warn_on exceptions_off rtti_off
CONFIG   += link_pkgconfig
# undocumented, but just works™
CONFIG   += silent

# needed, since `rtti_off` doesn't work
QMAKE_CXXFLAGS += -fno-rtti
QMAKE_RESOURCE_FLAGS += -compress 9 -threshold 0

# Rules for creating/updating {ts|qm}-files
include(translations/i18n.pri)
# Build all the qm files now, to make RCC happy
system($$fromfile(translations/i18n.pri, updateallqm))

isEmpty(GIT_VERSION) {
    GIT_VERSION = $$system(git rev-parse HEAD 2> /dev/null || echo "built without git")
}
DEFINES += GIT_VERSION=\"\\\"$$quote($$GIT_VERSION)\\\"\"
isEmpty(GIT_DESCRIBE) {
    GIT_DESCRIBE = $$system(git describe --tags 2> /dev/null || echo "Nightly")
}
DEFINES += GIT_DESCRIBE=\"\\\"$$quote($$GIT_DESCRIBE)\\\"\"
# date works on linux/mac, but it would hangs qmake on windows
# This hack returns 0 on batch (windows), but executes "date +%s" or return 0 if it fails on bash (linux/mac)
TIMESTAMP = $$system($1 2>null||echo 0||a;rm null;date +%s||echo 0) # I'm so sorry
DEFINES += TIMESTAMP=$$TIMESTAMP
DEFINES += LOG_TO_FILE
DEFINES += QT_MESSAGELOGCONTEXT

contains(DISABLE_PLATFORM_EXT, YES) {

} else {
    DEFINES += QTOX_PLATFORM_EXT
}

contains(JENKINS,YES) {
    INCLUDEPATH += ./libs/include/
} else {
    INCLUDEPATH += libs/include
}

contains(DEFINES, QTOX_PLATFORM_EXT) {
    HEADERS += src/platform/timer.h
    SOURCES += src/platform/timer_osx.cpp \
               src/platform/timer_win.cpp \
               src/platform/timer_x11.cpp

    HEADERS += src/platform/autorun.h
    SOURCES += src/platform/autorun_win.cpp \
               src/platform/autorun_xdg.cpp \
               src/platform/autorun_osx.cpp

    HEADERS += src/platform/capslock.h
    SOURCES += src/platform/capslock_win.cpp \
               src/platform/capslock_x11.cpp \
               src/platform/capslock_osx.cpp
}

# Rules for Windows, Mac OSX, and Linux
win32 {
    RC_FILE = windows/qtox.rc
    LIBS += -L$$PWD/libs/lib -ltoxav -ltoxcore -ltoxencryptsave -lsodium -lvpx -lpthread
    LIBS += -L$$PWD/libs/lib -lavdevice -lavformat -lavcodec -lavutil -lswscale -lOpenAL32 -lopus
    LIBS += -lqrencode -lsqlcipher -lcrypto
    LIBS += -lopengl32 -lole32 -loleaut32 -lvfw32 -lws2_32 -liphlpapi -lgdi32 -lshlwapi -luuid
    LIBS += -lstrmiids # For DirectShow
} else {
    macx {
        BUNDLEID = chat.tox.qtox
        ICON = img/icons/qtox.icns
        QMAKE_INFO_PLIST = osx/info.plist
        QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.7
        LIBS += -L$$PWD/libs/lib/ -ltoxcore -ltoxav -ltoxencryptsave -lsodium -lvpx -lopus -framework OpenAL -lavformat -lavdevice -lavcodec -lavutil -lswscale -mmacosx-version-min=10.7
        LIBS += -framework AVFoundation -framework Foundation -framework CoreMedia -framework ApplicationServices
        LIBS += -lqrencode -lsqlcipher
        contains(DEFINES, QTOX_PLATFORM_EXT) { LIBS += -framework IOKit -framework CoreFoundation }
        #Files to be includes into the qTox.app/Contents/Resources folder
        #OSX-Migrater.sh part of migrateProfiles() compatabilty code
        APP_RESOURCE.files = img/icons/qtox_profile.icns OSX-Migrater.sh
        APP_RESOURCE.path = Contents/Resources
        QMAKE_BUNDLE_DATA += APP_RESOURCE
        #Dynamic versioning for Info.plist
        INFO_PLIST_PATH = $$shell_quote($${OUT_PWD}/$${TARGET}.app/Contents/Info.plist)
        QMAKE_POST_LINK += /usr/libexec/PlistBuddy -c \"Set :CFBundleShortVersionString $${GIT_DESCRIBE}\" $${INFO_PLIST_PATH}
    } else {
        isEmpty(PREFIX) {
            PREFIX = /usr
        }

        BINDIR = $$PREFIX/bin
        DATADIR = $$PREFIX/share
        target.path = $$BINDIR
        desktop.path = $$DATADIR/applications
        desktop.files += qTox.desktop
        appdata.path = $$DATADIR/appdata
        appdata.files += res/qTox.appdata.xml
        INSTALLS += target desktop appdata

        # Install application icons according to the XDG spec
        ICON_SIZES = 14 16 22 24 32 36 48 64 72 96 128 192 256 512
        for(icon_size, ICON_SIZES) {
            icon_$${icon_size}.files = img/icons/$${icon_size}x$${icon_size}/qtox.png
            icon_$${icon_size}.path = $$DATADIR/icons/hicolor/$${icon_size}x$${icon_size}/apps
            INSTALLS += icon_$${icon_size}
        }
        icon_scalable.files = img/icons/qtox.svg
        icon_scalable.path = $$DATADIR/icons/hicolor/scalable/apps
        INSTALLS += icon_scalable

        # If we're building a package, static link libtox[core,av] and libsodium, since they are not provided by any package
        contains(STATICPKG, YES) {
            LIBS += -L$$PWD/libs/lib/ -lopus -lvpx -lopenal -Wl,-Bstatic -ltoxcore -ltoxav -ltoxencryptsave -lsodium -lavformat -lavdevice -lavcodec -lavutil -lswscale -lz -Wl,-Bdynamic
            LIBS += -Wl,-Bstatic -ljpeg -ltiff -lpng -ljasper -lIlmImf -lIlmThread -lIex -ldc1394 -lraw1394 -lHalf -lz -llzma -ljbig
            LIBS += -Wl,-Bdynamic -lv4l1 -lv4l2 -lavformat -lavcodec -lavutil -lswscale -lusb-1.0
            LIBS += -lqrencode -lsqlcipher
        } else {
            LIBS += -L$$PWD/libs/lib/ -ltoxcore -ltoxav -ltoxencryptsave -lvpx -lsodium -lopenal -lavformat -lavdevice -lavcodec -lavutil -lswscale
            LIBS += -lqrencode -lsqlcipher
        }

        contains(DEFINES, QTOX_PLATFORM_EXT) {
            LIBS += -lX11 -lXss
        }

        contains(JENKINS, YES) {
            LIBS = ./libs/lib/libtoxav.a ./libs/lib/libvpx.a ./libs/lib/libopus.a ./libs/lib/libtoxencryptsave.a ./libs/lib/libtoxcore.a ./libs/lib/libopenal.a ./libs/lib/libsodium.a ./libs/lib/libavdevice.a ./libs/lib/libavformat.a ./libs/lib/libavcodec.a ./libs/lib/libavutil.a ./libs/lib/libswscale.a ./libs/lib/libqrencode.a -ldl -lX11 -lXss
            contains(ENABLE_SYSTRAY_UNITY_BACKEND, YES) {
                LIBS += -lgobject-2.0 -lappindicator -lgtk-x11-2.0
            }
            LIBS += -s
        }
    }
}

unix:!macx {
    # The systray Unity backend implements the system tray icon on Unity (Ubuntu) and GNOME desktops.
    contains(ENABLE_SYSTRAY_UNITY_BACKEND, YES) {
        DEFINES += ENABLE_SYSTRAY_UNITY_BACKEND

        PKGCONFIG += glib-2.0 gtk+-2.0 atk
        PKGCONFIG += cairo gdk-pixbuf-2.0 pango
        PKGCONFIG += appindicator-0.1 dbusmenu-glib-0.4
    }

    # The systray Status Notifier backend implements the system tray icon on KDE and compatible desktops
    !contains(ENABLE_SYSTRAY_STATUSNOTIFIER_BACKEND, NO) {
        DEFINES += ENABLE_SYSTRAY_STATUSNOTIFIER_BACKEND

        PKGCONFIG += glib-2.0 gtk+-2.0 atk
        PKGCONFIG += cairo gdk-pixbuf-2.0 pango

        SOURCES +=     src/platform/statusnotifier/closures.c \
        src/platform/statusnotifier/enums.c \
        src/platform/statusnotifier/statusnotifier.c

        HEADERS += src/platform/statusnotifier/closures.h \
        src/platform/statusnotifier/enums.h \
        src/platform/statusnotifier/interfaces.h \
        src/platform/statusnotifier/statusnotifier.h
    }

    # The systray GTK backend implements a system tray icon compatible with many systems
    !contains(ENABLE_SYSTRAY_GTK_BACKEND, NO) {
        DEFINES += ENABLE_SYSTRAY_GTK_BACKEND

        PKGCONFIG += glib-2.0 gtk+-2.0 atk
        PKGCONFIG += gdk-pixbuf-2.0 cairo pango
    }

    # ffmpeg
    PKGCONFIG += libavformat libavdevice libavcodec
    PKGCONFIG += libavutil libswscale
}

win32 {
    HEADERS += \
        src/platform/camera/directshow.h

    SOURCES += \
        src/platform/camera/directshow.cpp
}

unix:!macx {
    HEADERS += \
        src/platform/camera/v4l2.h

    SOURCES += \
        src/platform/camera/v4l2.cpp
}

macx {
    SOURCES += \
        src/platform/install_osx.cpp

    HEADERS += \
        src/platform/install_osx.h \
        src/platform/camera/avfoundation.h

    OBJECTIVE_SOURCES += \
        src/platform/camera/avfoundation.mm
}

macx {
    INCLUDEPATH += /usr/local/include
    LIBPATH += /usr/local/lib
}

RESOURCES += res.qrc \
    qml/qml.qrc

!contains(SMILEYS, NO) {
    RESOURCES += smileys/emojione.qrc
    !contains(SMILEYS, MIN) {
        RESOURCES += smileys/smileys.qrc
    }
}

HEADERS  += \
    src/friend.h \
    src/friendlist.h \
    src/group.h \
    src/grouplist.h \
    src/ipc.h \
    src/nexus.h \
    src/audio/audio.h \
    src/core/core.h \
    src/core/coreav.h \
    src/core/coredefines.h \
    src/core/corefile.h \
    src/core/corestructs.h \
    src/core/cdata.h \
    src/core/cstring.h \
    src/core/toxid.h \
    src/core/indexedlist.h \
    src/core/recursivesignalblocker.h \
    src/core/toxcall.h \
    src/net/toxuri.h \
    src/net/autoupdate.h \
    src/net/toxme.h \
    src/net/avatarbroadcaster.h \
    src/persistence/smileypack.h \
    src/persistence/toxsave.h \
    src/persistence/serialize.h \
    src/persistence/offlinemsgengine.h \
    src/persistence/profilelocker.h \
    src/persistence/profile.h \
    src/persistence/settingsserializer.h \
    src/persistence/db/rawdatabase.h \
    src/persistence/history.h \
    src/persistence/historykeeper.h \
    src/persistence/settings.h \
    src/persistence/db/genericddinterface.h \
    src/persistence/db/plaindb.h \
    src/persistence/db/encrypteddb.h \
    src/video/videoframe.h \
    src/video/videosource.h \
    src/video/cameradevice.h \
    src/video/camerasource.h \
    src/video/corevideosource.h \
    src/video/videomode.h \
    src/qmlview.h

SOURCES += \
    src/ipc.cpp \
    src/friend.cpp \
    src/friendlist.cpp \
    src/group.cpp \
    src/grouplist.cpp \
    src/main.cpp \
    src/nexus.cpp \
    src/audio/audio.cpp \
    src/core/cdata.cpp \
    src/core/cstring.cpp \
    src/core/core.cpp \
    src/core/coreav.cpp \
    src/core/coreencryption.cpp \
    src/core/corefile.cpp \
    src/core/corestructs.cpp \
    src/core/recursivesignalblocker.cpp \
    src/core/toxid.cpp \
    src/core/toxcall.cpp \
    src/net/autoupdate.cpp \
    src/net/toxuri.cpp \
    src/net/toxme.cpp \
    src/net/avatarbroadcaster.cpp \
    src/persistence/historykeeper.cpp \
    src/persistence/settings.cpp \
    src/persistence/db/genericddinterface.cpp \
    src/persistence/db/plaindb.cpp \
    src/persistence/db/encrypteddb.cpp \
    src/persistence/profile.cpp \
    src/persistence/settingsserializer.cpp \
    src/persistence/smileypack.cpp \
    src/persistence/toxsave.cpp \
    src/persistence/serialize.cpp \
    src/persistence/offlinemsgengine.cpp \
    src/persistence/profilelocker.cpp \
    src/persistence/db/rawdatabase.cpp \
    src/persistence/history.cpp \
    src/video/videoframe.cpp \
    src/video/videosource.cpp \
    src/video/cameradevice.cpp \
    src/video/camerasource.cpp \
    src/video/corevideosource.cpp \
    src/video/videomode.cpp \
    src/qmlview.cpp
