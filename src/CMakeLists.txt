cmake_minimum_required(VERSION 3.10)

set(PROJECT SwitchDisplay)

project(${PROJECT} VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

source_group("Generated Files" REGULAR_EXPRESSION "((mocs_.*\.cpp)|(qrc_.*\.cpp)|(.*\.qrc)|(ui_.*\.h)|(autouic_.*\.stamp))$")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif()

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check https://doc.qt.io/qt/deployment-android.html for more information.
# They need to be set before the find_package( ...) calls below.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

set(QHOTKEY_EXAMPLES OFF)
add_subdirectory(external/QHotkey)
include_directories(external/QHotkey/QHotkey)

add_subdirectory(external/AudioDeviceLib)
set_directory_properties(PROPERTIES EXCLUDE_FROM_ALL TRUE)
set_property(DIRECTORY external/AudioDeviceLib/demos PROPERTY  EXCLUDE_FROM_ALL TRUE)
include_directories(external/AudioDeviceLib/include)

add_subdirectory(autorun)
include_directories(autorun)

set(PROJECT_SOURCES
    AudioDeviceManager.cpp
    AudioDeviceManager.h
    main.cpp
	main.qrc
    DisplayThread.cpp
    DisplayThread.h
    MainWindow.cpp
    MainWindow.h
    Settings.cpp
    Settings.h
    Settings.ui
	SettingsHelper.cpp
	SettingsHelper.h
	res/SwitchDisplay.rc
    autorun/AuthorizationHelper.cpp
    autorun/AuthorizationHelper.h
    autorun/TaskSchedulerManager.cpp
    autorun/TaskSchedulerManager.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(${PROJECT}
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(${PROJECT} SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(${PROJECT}
			WIN32
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(${PROJECT} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets QHotkey::QHotkey AudioDeviceLib taskschd comsuppw)

set_target_properties(${PROJECT} PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

find_program(WINDEPLOYQT_EXECUTABLE NAMES windeployqt PATHS "${Qt6_DIR}/../bin" REQUIRED)
if(NOT WINDEPLOYQT_EXECUTABLE)
    message(FATAL_ERROR "windeployqt not found!")
endif()

add_custom_command(TARGET ${PROJECT} POST_BUILD
    COMMAND ${WINDEPLOYQT_EXECUTABLE} $<TARGET_FILE:${PROJECT}>
    COMMENT "Deploying Qt libraries with windeployqt..."
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${PROJECT})
endif()
