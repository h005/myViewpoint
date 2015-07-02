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
    common.hh \
    externalimporter.hh

DEFINES += _USE_MATH_DEFINES

#CONFIG += console

win32 {
    DP_TOOLS_DIR = $$(DP_TOOLS_DIR)

    # OpenMesh
    INCLUDEPATH += $$DP_TOOLS_DIR/openmesh/include
    LIBS += -L$$DP_TOOLS_DIR/openmesh/lib/vs2013 -lOpenMeshCored

    # assimp
    LIBS += -L$$DP_TOOLS_DIR/assimp-3.1.1-win-binaries/build/code/Release -lassimp
    INCLUDEPATH += $$DP_TOOLS_DIR/assimp-3.1.1-win-binaries/include
}

# glm
INCLUDEPATH += $$_PRO_FILE_PWD_/../vendor/glm
