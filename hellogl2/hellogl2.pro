HEADERS       = glwidget.h \
                window.h \
                mainwindow.h \
    custom.h \
    DLT.h \
    GModel.h \
    trackball.h \
    imageandpoint.h \
    shader.hpp
SOURCES       = glwidget.cpp \
                main.cpp \
                window.cpp \
                mainwindow.cpp \
    custom.cpp \
    DLT.cpp \
    GModel.cpp \
    trackball.cpp \
    imageandpoint.cpp \
    shader.cpp

QT           += widgets

DEFINES += _CRT_SECURE_NO_WARNINGS

DP_TOOLS_DIR = $$(DP_TOOLS_DIR)
# opencv
win32:CONFIG(release, debug|release): LIBS += -L$$DP_TOOLS_DIR/opencv/build/x86/vc12/lib/ -lopencv_core249 -lopencv_highgui249 -lopencv_imgproc249
else:win32:CONFIG(debug, debug|release): LIBS += -L$$DP_TOOLS_DIR/opencv/build/x86/vc12/lib/ -lopencv_core249d -lopencv_highgui249d -lopencv_imgproc249d
INCLUDEPATH += $$DP_TOOLS_DIR/opencv/build/include
# assimp
LIBS += -L$$DP_TOOLS_DIR/assimp-3.1.1-win-binaries/build/code/Release -lassimp
INCLUDEPATH += $$DP_TOOLS_DIR/assimp-3.1.1-win-binaries/include
# glm
INCLUDEPATH += $$_PRO_FILE_PWD_/../vendor/glm
# glew
INCLUDEPATH += $$DP_TOOLS_DIR/glew/include
LIBS += -L$$DP_TOOLS_DIR/glew/lib/Release/Win32 -lglew32
