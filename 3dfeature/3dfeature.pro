################################################################################
#
################################################################################

################################################################################

DIRECTORIES = .

SOURCES += cube.cc

DEFINES += _USE_MATH_DEFINES

CONFIG += console

win32 {
    DP_TOOLS_DIR = $$(DP_TOOLS_DIR)

    INCLUDEPATH += $$DP_TOOLS_DIR/openmesh/include
    LIBS += -L$$DP_TOOLS_DIR/openmesh/lib/vs2013 -lOpenMeshCored
}
