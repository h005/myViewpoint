#ifndef BASERENDEROBJECT_H
#define BASERENDEROBJECT_H

#include <GL/glew.h>

///
/// \brief The BaseRenderObject class
/// 管理模型到显存之间的映射
///
class BaseRenderObject
{
public:
    BaseRenderObject();
    ~BaseRenderObject();

    ///
    /// \brief cleanup
    /// 可以执行一些自定义的清理操作，需要在OpenGL环境中调用
    /// 一般情况下不用覆盖此实现
    virtual void cleanup();

    ///
    /// \brief load
    /// 可以用于从磁盘中读取模型，或者也可以不用
    ///
    /// \return
    /// 是否成功
    ///
    virtual bool load();

    ///
    /// \brief bindDataToGL
    /// 必须在OpenGL上下文中调用，主要将数据拷贝到显存空间中
    ///
    /// \param args
    /// 可以传递一些shader中代表位置的参数，以便进行vao绑定
    ///
    virtual void bindDataToGL(GLuint args[], void *others=0);

    ///
    /// \brief draw
    /// 在OpenGL上下文中调用，此处用于绘制
    ///
    virtual void draw() = 0;

protected:
    bool m_isInited = false;
    GLuint m_vao = 0, m_vboVertex = 0, m_vboVertexNormal = 0, m_vboVertexColor = 0, m_vboIndex = 0;
};

#endif // BASERENDEROBJECT_H
