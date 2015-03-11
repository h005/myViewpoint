HEADERS       = glwidget.h \
                window.h \
                mainwindow.h \
    custom.h \
    DLT.h \
    GModel.h \
    trackball.h \
    imageandpoint.h \
    shader.hpp \
    pointsmatchrelation.h \
    sphere.h \
    alignresultwidget.h \
    entity.h \
    entitymanager.h
SOURCES       = glwidget.cpp \
                main.cpp \
                window.cpp \
                mainwindow.cpp \
    custom.cpp \
    DLT.cpp \
    GModel.cpp \
    trackball.cpp \
    imageandpoint.cpp \
    shader.cpp \
    pointsmatchrelation.cpp \
    sphere.cpp \
    alignresultwidget.cpp \
    entity.cpp \
    entitymanager.cpp

QT           += widgets

DEFINES += _CRT_SECURE_NO_WARNINGS

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
    win32:CONFIG(release, debug|release): LIBS += -L$$DP_TOOLS_DIR/opencv/build/x86/vc12/lib/ -lopencv_core249 -lopencv_highgui249 -lopencv_imgproc249
    else:win32:CONFIG(debug, debug|release): LIBS += -L$$DP_TOOLS_DIR/opencv/build/x86/vc12/lib/ -lopencv_core249d -lopencv_highgui249d -lopencv_imgproc249d
    INCLUDEPATH += $$DP_TOOLS_DIR/opencv/build/include
    # assimp
    LIBS += -L$$DP_TOOLS_DIR/assimp-3.1.1-win-binaries/build/code/Release -lassimp
    INCLUDEPATH += $$DP_TOOLS_DIR/assimp-3.1.1-win-binaries/include
    # glew
    INCLUDEPATH += $$DP_TOOLS_DIR/glew/include
    LIBS += -L$$DP_TOOLS_DIR/glew/lib/Release/Win32 -lglew32
}

# glm
INCLUDEPATH += $$_PRO_FILE_PWD_/../vendor/glm
