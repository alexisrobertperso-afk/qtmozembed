# Qt6 build of QtMozEmbed, for REDNET BROWSER (desktop Linux). The qmake
# files still build the Qt5 variant for Sailfish OS; this file serves Qt6 only.
cmake_minimum_required(VERSION 3.24)
project(qt6mozembed VERSION 2.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON) # gnu++20, as src.pro asks
include(GNUInstallDirs)
enable_testing()

find_package(Qt6 6.7 REQUIRED COMPONENTS Core Gui GuiPrivate Network OpenGL Qml Quick Test QuickTest)
qt_standard_project_setup(REQUIRES 6.7)

# Gecko-free surface test: the frame and tab logic, against EmbedLite stubs.
set(SURFACE_SOURCES
    src/runtime/qmozchromesession_p.cpp src/runtime/qmozchromewindowshutdown_p.cpp
    src/runtime/qmozframestream_p.cpp src/runtime/qmozsurface_p.cpp
    src/runtime/qmoztabmodel_p.cpp
    src/backends/embedlite/embedlitechromesession_p.cpp
    src/backends/embedlite/embedlitesurface_p.cpp)
file(GLOB SURFACE_HEADERS CONFIGURE_DEPENDS src/runtime/*_p.h src/backends/embedlite/*_p.h)
qt_add_executable(tst_qmozsurface tests/surface/tst_qmozsurface.cpp ${SURFACE_SOURCES} ${SURFACE_HEADERS})
target_include_directories(tst_qmozsurface PRIVATE tests/surface/stubs src)
target_link_libraries(tst_qmozsurface PRIVATE Qt6::Core Qt6::Test)
add_test(NAME tst_qmozsurface COMMAND tst_qmozsurface)

find_package(PkgConfig REQUIRED)
# libxul, as src/qmozembed.pri: QtMozEmbed calls XRE functions directly
# (-lxpcomglue -lxul, with an rpath to the engine directory).
pkg_check_modules(GECKO REQUIRED IMPORTED_TARGET libxul nspr pixman-1 egl)
pkg_get_variable(GECKO_SDKDIR libxul sdkdir)
pkg_get_variable(GECKO_RUNTIMEDIR libxul runtimedir)
find_package(OpenGL REQUIRED COMPONENTS OpenGL)

# Not built for Qt6: qmozextmaterialnode (Qt5 scene-graph shaders; the Qt6
# path draws with QSGSimpleTextureNode, see QuickMozView::updatePaintNode).
# qmoznativeview is built: QuickMozView qobject_casts to it.
set(QTMOZEMBED_SOURCES
    src/qmozcontext.cpp src/qmozenginesettings.cpp src/qmozscrolldecorator.cpp
    src/qmessagepump.cpp src/EmbedQtKeyUtils.cpp src/qmozsecurity.cpp
    src/qmozview_p.cpp src/qmozwindow.cpp src/qmozwindow_p.cpp
    src/qmozmediacontroller.cpp src/quickmozview.cpp src/qmoznativeview.cpp src/qmozexttexture.cpp
    src/runtime/qmozchromewindowregistry_p.cpp src/runtime/qmozruntime_p.cpp
    src/runtime/qmoztexturelease_p.cpp
    ${SURFACE_SOURCES})
file(GLOB QTMOZEMBED_HEADERS CONFIGURE_DEPENDS src/*.h src/runtime/*.h src/backends/embedlite/*.h)
list(FILTER QTMOZEMBED_HEADERS EXCLUDE REGEX "/qmozextmaterialnode\\.h$")

qt_add_library(qt6mozembed SHARED ${QTMOZEMBED_SOURCES} ${QTMOZEMBED_HEADERS})
target_include_directories(qt6mozembed PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/qt6mozembed>)
target_compile_definitions(qt6mozembed PRIVATE
    XPCOM_GLUE=1 XPCOM_GLUE_USE_NSPR=1 MOZ_GLUE_IN_PROGRAM=1 MESA_EGL_NO_X11_HEADERS
    BUILD_GRE_HOME="${GECKO_RUNTIMEDIR}")
target_compile_options(qt6mozembed PRIVATE
    -include mozilla-config.h -fno-short-wchar -Wno-attributes -Wno-ignored-qualifiers)
target_link_directories(qt6mozembed PRIVATE ${GECKO_SDKDIR}/lib)
target_link_libraries(qt6mozembed
    PUBLIC Qt6::Core Qt6::Gui Qt6::Network Qt6::Qml Qt6::Quick
    PRIVATE Qt6::GuiPrivate Qt6::OpenGL OpenGL::OpenGL PkgConfig::GECKO
            "$<LINK_LIBRARY:WHOLE_ARCHIVE,${GECKO_SDKDIR}/lib/libmozglue.a>" dl)
# libxul has no rpath of its own: give this library a DT_RPATH (not a
# RUNPATH) to the engine directory, which the loader also uses for libxul's
# dependencies (libmozsandbox, libmozsqlite3, ...).
target_link_options(qt6mozembed PRIVATE -rdynamic -Wl,--disable-new-dtags)
set_target_properties(qt6mozembed PROPERTIES
    BUILD_RPATH "${GECKO_RUNTIMEDIR}"
    INSTALL_RPATH "${GECKO_RUNTIMEDIR}")
# Executables linking this library let the linker resolve libxul's own
# dependencies (libgkcodecs, libmozsandbox, ...) in the engine directory.
target_link_options(qt6mozembed INTERFACE "LINKER:-rpath-link,${GECKO_RUNTIMEDIR}")

file(GLOB QTMOZEMBED_PUBLIC_HEADERS src/q*.h)
list(FILTER QTMOZEMBED_PUBLIC_HEADERS EXCLUDE REGEX "(_p\\.h|qmessagepump\\.h|qmozextmaterialnode\\.h|qmozexttexture\\.h)$")
install(TARGETS qt6mozembed EXPORT Qt6MozEmbedTargets LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES ${QTMOZEMBED_PUBLIC_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/qt6mozembed)
install(EXPORT Qt6MozEmbedTargets NAMESPACE RedNet:: DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Qt6MozEmbed)
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/Qt6MozEmbedConfig.cmake
     "include(CMakeFindDependencyMacro)\n"
     "find_dependency(Qt6 COMPONENTS Core Gui Network Qml Quick)\n"
     "include(\"\${CMAKE_CURRENT_LIST_DIR}/Qt6MozEmbedTargets.cmake\")\n")
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/Qt6MozEmbedConfig.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Qt6MozEmbed)

# RedNet.Web: the QML module of REDNET BROWSER (qmlplugin6/).
qt_add_qml_module(rednetweb
    URI RedNet.Web
    VERSION 1.0
    PLUGIN_TARGET rednetweb
    NO_GENERATE_PLUGIN_SOURCE
    CLASS_NAME RedNetWebPlugin
    OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/qml/RedNet/Web
    SOURCES qmlplugin6/plugin.cpp
    QML_FILES qmlplugin6/WebView.qml)
target_compile_definitions(rednetweb PRIVATE DEFAULT_COMPONENTS_PATH="${GECKO_RUNTIMEDIR}/")
target_link_libraries(rednetweb PRIVATE qt6mozembed Qt6::Qml Qt6::Quick)
set_target_properties(rednetweb PROPERTIES INSTALL_RPATH "$ORIGIN/../../../${CMAKE_INSTALL_LIBDIR}")
install(TARGETS rednetweb LIBRARY DESTINATION qml/RedNet/Web)
install(FILES ${CMAKE_BINARY_DIR}/qml/RedNet/Web/qmldir DESTINATION qml/RedNet/Web)

qt_add_executable(tst_rednetweb tests/qt6/tst_rednetweb.cpp)
target_link_libraries(tst_rednetweb PRIVATE Qt6::QuickTest Qt6::Quick)
add_test(NAME tst_rednetweb
         COMMAND tst_rednetweb -input ${CMAKE_CURRENT_SOURCE_DIR}/tests/qt6
                 -import ${CMAKE_BINARY_DIR}/qml -platform offscreen)
