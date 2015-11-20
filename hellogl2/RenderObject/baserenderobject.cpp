#include "baserenderobject.h"

BaseRenderObject::BaseRenderObject()
{
}

BaseRenderObject::~BaseRenderObject()
{
    // 清理显存中的数据
    if (!m_isInited) {
        return;
    }
    if(m_vboVertex) {
        glDeleteBuffers(1, &m_vboVertex);
    }
    if(m_vboVertexNormal) {
        glDeleteBuffers(1, &m_vboVertexNormal);
    }
    if(m_vboIndex) {
        glDeleteBuffers(1, &m_vboIndex);
    }
    if (m_vao) {
        glDeleteVertexArrays(1, &m_vao);
    }

    m_isInited = false;
    m_vao = 0;
    m_vboVertex = 0;
    m_vboIndex = 0;
}

bool BaseRenderObject::load()
{
    return true;
}

void BaseRenderObject::bindDataToGL(GLuint args[], void *others)
{
}

