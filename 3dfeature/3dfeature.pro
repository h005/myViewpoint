################################################################################
#
################################################################################

################################################################################

QT           += widgets
DIRECTORIES = .

SOURCES += colormap.cc glwidget.cc shader.cc trackball.cc \
    main.cc \
    mainwindow.cc
HEADERS += Curvature.hh \
    gausscurvature.hh \
    meancurvature.hh \
    colormap.hh \
    common.hh \
    externalimporter.hh \
    surfacevisibility.hh \
    abstractfeature.hh \
    viewpointentropy.hh \
    projectedarea.hh \
    renderinterface.hh \
    glwidget.hh \
    shader.hh \
    meshglhelper.hh \
    trackball.hh \
    mainwindow.hh
OTHER_FILES += shader/*.vert shader/*.frag

DEFINES += _USE_MATH_DEFINES BUILDIN_READER

win32 {
    DP_TOOLS_DIR = $$(DP_TOOLS_DIR)

    # OpenMesh
    INCLUDEPATH += $$DP_TOOLS_DIR/openmesh/include
    LIBS += -L$$DP_TOOLS_DIR/openmesh/lib/vs2013 -lOpenMeshCored

    # assimp
    LIBS += -L$$DP_TOOLS_DIR/assimp-3.1.1-win-binaries/build/code/Release -lassimp
    INCLUDEPATH += $$DP_TOOLS_DIR/assimp-3.1.1-win-binaries/include

    # glew
    INCLUDEPATH += $$DP_TOOLS_DIR/glew/include
    LIBS += -L$$DP_TOOLS_DIR/glew/lib/Release/Win32 -lglew32
}

# glm
INCLUDEPATH += $$_PRO_FILE_PWD_/../vendor/glm
