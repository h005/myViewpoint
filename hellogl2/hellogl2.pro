HEADERS       = glwidget.h \
                window.h \
    DLT.h \
    GModel.h \
    trackball.h \
    shader.hpp \
    pointsmatchrelation.h \
    alignresultwidget.h \
    entity.h \
    entitymanager.h \
    camerashowwidget.h \
    axis.h \
    cvdlt.h \
    lmdlt.h \
    mainentrywindow.h \
    alignwindow.h \
    gloffscreenrenderframework.h \
    OffscreenRender.h \
    TransformationUtils.h \
    pointcloudwidget.h \
    logo.h \
    RenderObject/baserenderobject.h \
    RenderObject/plycloudobject.h \
    RenderObject/sphere.h \
    dragablewidget.h \
    pointcloudcapturewidget.h \
    pointcloudoffscreenrender.h \
    bothwidget.h
SOURCES       = glwidget.cpp \
                main.cpp \
                window.cpp \
    DLT.cpp \
    GModel.cpp \
    trackball.cpp \
    shader.cpp \
    pointsmatchrelation.cpp \
    alignresultwidget.cpp \
    entity.cpp \
    entitymanager.cpp \
    camerashowwidget.cpp \
    axis.cpp \
    expfit.cpp \
    cvdlt.cpp \
    lmdlt.cpp \
    mainentrywindow.cpp \
    alignwindow.cpp \
    gloffscreenrenderframework.cpp \
    OffscreenRender.cpp \
    TransformationUtils.cpp \
    pointcloudwidget.cpp \
    logo.cpp \
    RenderObject/baserenderobject.cpp \
    RenderObject/plycloudobject.cpp \
    RenderObject/sphere.cpp \
    dragablewidget.cpp \
    pointcloudcapturewidget.cpp \
    pointcloudoffscreenrender.cpp \
    bothwidget.cpp

OTHER_FILES += shader/*.vert shader/*.frag

QT           += widgets

DEFINES += _CRT_SECURE_NO_WARNINGS GLM_FORCE_RADIANS
#DEFINES += __TEST_EXPFIT

macx {
    LIBS += -L/usr/local/Cellar/glew/1.11.0/lib -lGLEW
    LIBS += -L/usr/local/Cellar/assimp/3.1.1/lib/ -lassimp
    LIBS += -L/usr/local/Cellar/opencv/2.4.9/lib/ -lopencv_core -lopencv_highgui -lopencv_imgproc
    INCLUDEPATH += /usr/local/Cellar/glew/1.11.0/include
    INCLUDEPATH += /usr/local/Cellar/assimp/3.1.1/include
    INCLUDEPATH += /usr/local/Cellar/opencv/2.4.9/include

}
win32 {
    DP_TOOLS_DIR = $$(DP_TOOLS_DIR)
    # opencv
    win32:CONFIG(release, debug|release): LIBS += -L$$DP_TOOLS_DIR/opencv/build/x86/vc12/lib/ -lopencv_core249 -lopencv_highgui249 -lopencv_imgproc249 -lopencv_calib3d249
    else:win32:CONFIG(debug, debug|release): LIBS += -L$$DP_TOOLS_DIR/opencv/build/x86/vc12/lib/ -lopencv_core249d -lopencv_highgui249d -lopencv_imgproc249d -lopencv_calib3d249d
    INCLUDEPATH += $$DP_TOOLS_DIR/opencv/build/include
    # assimp
    LIBS += -L$$DP_TOOLS_DIR/assimp-3.1.1-win-binaries/build/code/Release -lassimp
    INCLUDEPATH += $$DP_TOOLS_DIR/assimp-3.1.1-win-binaries/include
    # glew
    INCLUDEPATH += $$DP_TOOLS_DIR/glew/include
    LIBS += -L$$DP_TOOLS_DIR/glew/lib/Release/Win32 -lglew32

    INCLUDEPATH += $$DP_TOOLS_DIR/levmar
    LIBS += -L$$DP_TOOLS_DIR/levmar -llevmar
}

# glm
INCLUDEPATH += $$_PRO_FILE_PWD_/../vendor/glm

FORMS += \
    mainentrywindow.ui
