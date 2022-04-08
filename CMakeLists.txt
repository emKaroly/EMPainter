cmake_minimum_required(VERSION 3.5)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMake/Modules/")

if(MSVC)
  if (CMAKE_CL_64 OR ${CMAKE_SIZEOF_VOID_P} EQUAL "8")
    set(OUTPUT_DIRECTORY "bin/x64")
  else()
    set(OUTPUT_DIRECTORY "bin/x86")
  endif()
elseif(APPLE)
  set(OUTPUT_DIRECTORY "bin/darwin")
elseif(UNIX)
  set(OUTPUT_DIRECTORY "bin/linux")
else()
endif()

project(EMPainter VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

set(iconMacOS ${CMAKE_SOURCE_DIR}/resources/icon/empainter.icns)
set(iconWin ${CMAKE_SOURCE_DIR}/resources/icon/empainter.rc)
set_source_files_properties(${iconMacOS}
  PROPERTIES
  MACOSX_PACKAGE_LOCATION "Resources"
  )

set(PROJECT_SOURCES
  src/main.cpp
  src/mainwindow.cpp
  src/mainwindow.h
  src/mainwindow.ui
  src/paintarea.cpp
  src/paintarea.h
  )

if(APPLE)
  file(COPY ${CMAKE_SOURCE_DIR}/resources/ DESTINATION  ${CMAKE_SOURCE_DIR}/${OUTPUT_DIRECTORY}/EMPainter.app/Contents/MacOS)
else()
  file(COPY ${CMAKE_SOURCE_DIR}/resources/ DESTINATION  ${CMAKE_SOURCE_DIR}/${OUTPUT_DIRECTORY})
endif()

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
  qt_add_executable(EMPainter
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
    )
  # Define target properties for Android with Qt 6 as:
  #    set_property(TARGET EMPainter APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
  #                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
  # For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
  if(ANDROID)
    add_library(EMPainter SHARED
      ${PROJECT_SOURCES}
      )
    # Define properties for Android with Qt 5 after find_package() calls as:
    #    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
  else()
    add_executable(EMPainter
      MACOSX_BUNDLE
      WIN32
      ${PROJECT_SOURCES}
      ${CMAKE_SOURCE_DIR}/resources/resources.qrc
      ${iconMacOS}
      ${iconWin}
      )
  endif()
endif()

target_link_libraries(EMPainter PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

set_target_properties(EMPainter
  PROPERTIES
  OUTPUT_NAME EMPainter
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED TRUE

  MACOSX_BUNDLE_GUI_IDENTIFIER www.escapemotions.com
  MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
  MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}

  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/${OUTPUT_DIRECTORY}"
  RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/${OUTPUT_DIRECTORY}"
  RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/${OUTPUT_DIRECTORY}"
  # extra output directory settings for xcode
  XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_SOURCE_DIR}/${OUTPUT_DIRECTORY}"
  MACOS_BUNDLE_ICON_FILE empainter.icns
  )

if(QT_VERSION_MAJOR EQUAL 6)
  qt_finalize_executable(EMPainter)
endif()
