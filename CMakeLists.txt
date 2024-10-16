cmake_minimum_required(VERSION 3.16)

project(A0 VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

add_compile_options("/utf-8")

find_package(Geant4 REQUIRED)
find_package(ASSIMP REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Widgets Charts OpenGL)

file(GLOB_RECURSE OpenGLModule_SOURCE OpenGLModule/*.h OpenGLModule/*.cpp OpenGLModule/*.qrc)
include_directories(OpenGLModule)
add_library(OpenGLModule SHARED ${OpenGLModule_SOURCE})
target_link_libraries(OpenGLModule PRIVATE Qt5::OpenGL ${ASSIMP_LIBRARIES})

file(GLOB_RECURSE Geant4Module_SOURCE Geant4Module/*.h Geant4Module/*.cpp Geant4Module/*.qrc)
include_directories(Geant4Module)
add_library(Geant4Module SHARED ${Geant4Module_SOURCE})
target_link_libraries(Geant4Module PRIVATE ${Geant4_LIBRARIES})

file(GLOB_RECURSE SpectrumModule_SOURCE SpectrumModule/*.h SpectrumModule/*.cpp SpectrumModule/*.qrc)
include_directories(SpectrumModule)
add_library(SpectrumModule SHARED ${SpectrumModule_SOURCE})
target_link_libraries(SpectrumModule PRIVATE Qt5::Widgets Qt5::Charts)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    res.qrc
)

add_executable(A0
    ${PROJECT_SOURCES}

)

set(EXAMPLEB1_SCRIPTS
  g4.ini
  )

foreach(_script ${EXAMPLEB1_SCRIPTS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
    )
endforeach()

target_link_libraries(A0 PRIVATE Qt5::Widgets Qt5::Charts
    SpectrumModule
    OpenGLModule
    ${ASSIMP_LIBRARIES}
    Geant4Module
    ${Geant4_LIBRARIES}
)

set_target_properties(A0 PROPERTIES WIN32_EXECUTABLE TRUE)

install(TARGETS A0
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
