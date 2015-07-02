################################################################################
#
################################################################################

################################################################################

DIRECTORIES = .

SOURCES += cube.cc colormap.cc
HEADERS += Curvature.hh \
    gausscurvature.hh \
    meancurvature.hh \
    colormap.hh \
    common.hh

DEFINES += _USE_MATH_DEFINES

#CONFIG += console

win32 {
    DP_TOOLS_DIR = $$(DP_TOOLS_DIR)

    INCLUDEPATH += $$DP_TOOLS_DIR/openmesh/include
    LIBS += -L$$DP_TOOLS_DIR/openmesh/lib/vs2013 -lOpenMeshCored
}

# glm
INCLUDEPATH += $$_PRO_FILE_PWD_/../vendor/glm
