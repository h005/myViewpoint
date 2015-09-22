HEADERS       = glwidget.h \
                window.h \
    DLT.h \
    GModel.h \
    trackball.h \
    imageandpoint.h \
    shader.hpp \
    pointsmatchrelation.h \
    sphere.h \
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
    TransformationUtils.h
SOURCES       = glwidget.cpp \
                main.cpp \
                window.cpp \
    DLT.cpp \
    GModel.cpp \
    trackball.cpp \
    imageandpoint.cpp \
    shader.cpp \
    pointsmatchrelation.cpp \
    sphere.cpp \
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
    TransformationUtils.cpp

OTHER_FILES += shader/*.vert shader/*.frag

QT           += widgets

DEFINES += _CRT_SECURE_NO_WARNINGS
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

defineTest(copyToDestdir) {
    files = $$1
    for(FILE, files) {
        DDIR = $$OUT_PWD
        # Replace slashes in paths with backslashes for Windows
        win32:FILE ~= s,/,\\,g
        win32:DDIR ~= s,/,\\,g
        QMAKE_POST_LINK += $$QMAKE_COPY $$quote($$FILE) $$quote($$DDIR) $$escape_expand(\\n\\t)
    }

    export(QMAKE_POST_LINK)
}
copyToDestdir($$_PRO_FILE_PWD_/shader)
