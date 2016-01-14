TEMPLATE = app

QT += qml quick widgets

CONFIG += c++11

SOURCES += main.cpp \
    api/apimanager.cpp \
    api/models/couponmodel.cpp

HEADERS += \
    api/apimanager.h \
    api/models/couponmodel.h \
    api/usingleton.h

RESOURCES += qml/qml.qrc \
             assets/assets.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

#Translations
lupdate_only {
    SOURCES += $$PWD/qml/*.qml \
    $$PWD/qml/activities/*.qml \
    $$PWD/qml/includes/*.qml \
    $$PWD/qml/models/*.qml
}
TRANSLATIONS += i18n/skidkz_ru.ts

# Default rules for deployment.
include(deployment.pri)

#AdCtl: Google Analytics, AdMob, StartAD.mobi
ANDROID_PACKAGE_SOURCE_DIR = $$PWD/mobile/android
include(mobile/adctl/AdCtl.pri)
android {
  OTHER_FILES += $$PWD/mobile/android/AndroidManifest.xml
}

DISTFILES += \
    mobile/android/AndroidManifest.xml \
    mobile/android/gradle/wrapper/gradle-wrapper.jar \
    mobile/android/gradlew \
    mobile/android/res/values/libs.xml \
    mobile/android/build.gradle \
    mobile/android/gradle/wrapper/gradle-wrapper.properties \
    mobile/android/gradlew.bat
