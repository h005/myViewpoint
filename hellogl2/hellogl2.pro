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
    bothwidget.h \
    RenderObject/axis.h \
    externalimporter.h \
    LMModelMainComponent.h \
    ccwindow.h \
    ccmodelwidget.h \
    imglabel.h \
    ccentitymanager.h \
    ccsift.h \
    ccsiftmatch.h \
    ccsiftmatchwindow.h

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
    bothwidget.cpp \
    RenderObject/axis.cpp \
    LMModelMainComponent.cpp \
    ccwindow.cpp \
    ccmodelwidget.cpp \
    imglabel.cpp \
    ccentitymanager.cpp \
    ccsift.cpp \
    ccsiftmatch.cpp \
    ccsiftmatchwindow.cpp

OTHER_FILES += shader/*.vert shader/*.frag

QT           += widgets

CONFIG += c++11

DEFINES += _CRT_SECURE_NO_WARNINGS GLM_FORCE_RADIANS
#DEFINES += __TEST_EXPFIT

#macx {
#    LIBS += -L/usr/local/Cellar/glew/1.11.0/lib -lGLEW
#    LIBS += -L/usr/local/Cellar/assimp/3.1.1/lib/ -lassimp
#    LIBS += -L/usr/local/Cellar/opencv/2.4.9/lib/ -lopencv_core -lopencv_highgui -lopencv_imgproc
#    INCLUDEPATH += /usr/local/Cellar/glew/1.11.0/include
#    INCLUDEPATH += /usr/local/Cellar/assimp/3.1.1/include
#    INCLUDEPATH += /usr/local/Cellar/opencv/2.4.9/include

#}
#win32 {
#    DP_TOOLS_DIR = $$(DP_TOOLS_DIR)
#    # opencv
#    win32:CONFIG(release, debug|release): LIBS += -L$$DP_TOOLS_DIR/opencv/build/x86/vc12/lib/ -lopencv_core249 -lopencv_highgui249 -lopencv_imgproc249 -lopencv_calib3d249
#    else:win32:CONFIG(debug, debug|release): LIBS += -L$$DP_TOOLS_DIR/opencv/build/x86/vc12/lib/ -lopencv_core249d -lopencv_highgui249d -lopencv_imgproc249d -lopencv_calib3d249d
#    INCLUDEPATH += $$DP_TOOLS_DIR/opencv/build/include
#    # assimp
#    LIBS += -L$$DP_TOOLS_DIR/assimp-3.1.1-win-binaries/build/code/Release -lassimp
#    INCLUDEPATH += $$DP_TOOLS_DIR/assimp-3.1.1-win-binaries/include
#    # glew
#    INCLUDEPATH += $$DP_TOOLS_DIR/glew/include
#    LIBS += -L$$DP_TOOLS_DIR/glew/lib/Release/Win32 -lglew32

#    INCLUDEPATH += $$DP_TOOLS_DIR/levmar
#    LIBS += -L$$DP_TOOLS_DIR/levmar -llevmar
#}



## glm
#INCLUDEPATH += $$_PRO_FILE_PWD_/../vendor/glm

# levmar
# levmar very depends on lapack and blas, and should be added as blew, wtf , it cost my so much time!
INCLUDEPATH += /home/h005/levmar-2.6
LIBS += -L/home/h005/levmar-2.6/build -llevmar \
                -L/usr/lib -llapack \
                -L/usr/lib -lblas


# OpenMesh
INCLUDEPATH += /usr/local/include
LIBS += -lOpenMeshCore

# assimp
INCLUDEPATH += /usr/include/assimp
LIBS += -lassimp

# glew
# INCLUDEPATH +=
LIBS += -lGLEW -lGLU -lGL

# glm
INCLUDEPATH += usr/include/glm

#opencv
INCLUDEPATH += /usr/local/include \
                /usr/local/include/opencv \
                /usr/local/include/opencv2 \

LIBS += /usr/local/lib/libopencv_aruco.so.3.1 \
        /usr/local/lib/libopencv_bgsegm.so.3.1 \
        /usr/local/lib/libopencv_bioinspired.so.3.1 \
        /usr/local/lib/libopencv_calib3d.so.3.1 \
        /usr/local/lib/libopencv_ccalib.so.3.1 \
        /usr/local/lib/libopencv_core.so.3.1 \
        /usr/local/lib/libopencv_datasets.so.3.1 \
        /usr/local/lib/libopencv_dnn.so.3.1 \
        /usr/local/lib/libopencv_dpm.so.3.1 \
        /usr/local/lib/libopencv_face.so.3.1 \
        /usr/local/lib/libopencv_features2d.so.3.1 \
        /usr/local/lib/libopencv_flann.so.3.1 \
        /usr/local/lib/libopencv_fuzzy.so.3.1 \
        /usr/local/lib/libopencv_hdf.so.3.1 \
        /usr/local/lib/libopencv_highgui.so.3.1 \
        /usr/local/lib/libopencv_imgcodecs.so.3.1 \
        /usr/local/lib/libopencv_imgproc.so.3.1 \
        /usr/local/lib/libopencv_line_descriptor.so.3.1 \
        /usr/local/lib/libopencv_ml.so.3.1 \
        /usr/local/lib/libopencv_objdetect.so.3.1 \
        /usr/local/lib/libopencv_optflow.so.3.1 \
        /usr/local/lib/libopencv_photo.so.3.1 \
        /usr/local/lib/libopencv_plot.so.3.1 \
        /usr/local/lib/libopencv_reg.so.3.1 \
        /usr/local/lib/libopencv_rgbd.so.3.1 \
        /usr/local/lib/libopencv_saliency.so.3.1 \
        /usr/local/lib/libopencv_sfm.so.3.1 \
        /usr/local/lib/libopencv_shape.so.3.1 \
        /usr/local/lib/libopencv_stereo.so.3.1 \
        /usr/local/lib/libopencv_stitching.so.3.1 \
        /usr/local/lib/libopencv_structured_light.so.3.1 \
        /usr/local/lib/libopencv_superres.so.3.1 \
        /usr/local/lib/libopencv_surface_matching.so.3.1 \
        /usr/local/lib/libopencv_text.so.3.1 \
        /usr/local/lib/libopencv_tracking.so.3.1 \
        /usr/local/lib/libopencv_videoio.so.3.1 \
        /usr/local/lib/libopencv_video.so.3.1 \
        /usr/local/lib/libopencv_videostab.so.3.1 \
        /usr/local/lib/libopencv_viz.so.3.1 \
        /usr/local/lib/libopencv_xfeatures2d.so.3.1 \
        /usr/local/lib/libopencv_ximgproc.so.3.1 \
        /usr/local/lib/libopencv_xobjdetect.so.3.1 \
        /usr/local/lib/libopencv_xphoto.so.3.1

FORMS += \
    mainentrywindow.ui
