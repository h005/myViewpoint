#ifndef MESHGLHELPER_HH
#define MESHGLHELPER_HH

#include <assert.h>
#include <map>

template <typename MeshT>
class MeshGLHelper {
public:
    MeshGLHelper(MeshT &in_mesh)
        : m_mesh(in_mesh)
    {
    }

    /**
     * @brief 将mesh的结构转换为OpenGL中的vao和vbo，并送入显存
     * @param vertexPositionID shader中顶点变量所在位置
     */
    void init(GLuint vertexPositionID)
    {
        int index = 0;
        std::map<typename MeshT::VertexHandle, int> dict;

        std::vector<GLfloat> vertices;
        typename MeshT::VertexIter v_it, v_end(m_mesh.vertices_end());
        for (v_it = m_mesh.vertices_begin(); v_it != v_end; v_it++) {
            MeshT::Point pos = m_mesh.point(*v_it);
            vertices.push_back(pos[0]);
            vertices.push_back(pos[1]);
            vertices.push_back(pos[2]);

            dict[*v_it] = index;
            index++;
        }

        std::vector<GLuint> indices;
        typename MeshT::FaceIter f_it, f_end(m_mesh.faces_end());
        for (f_it = m_mesh.faces_begin(); f_it != f_end; f_it++) {
            typename MeshT::FaceVertexIter fv_it(m_mesh.fv_iter(*f_it));
            for (; fv_it; fv_it++)
                indices.push_back(dict[*fv_it]);
        }

        // 创建并绑定环境
        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);

        glGenBuffers(1, &m_vboVertex);
        glBindBuffer(GL_ARRAY_BUFFER, m_vboVertex);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);

        glVertexAttribPointer(vertexPositionID, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray (vertexPositionID);

        glGenBuffers(1, &m_vboIndex);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIndex);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

        numsToDraw = indices.size();
        m_isInited = true;
    }

    void draw()
    {
        if (!m_isInited) {
            std::cout << "please call init() before draw()" << std::endl;
            assert(0);
        }

        // draw sphere
        glBindVertexArray(m_vao);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIndex);
        glDrawElements(GL_TRIANGLES, numsToDraw, GL_UNSIGNED_INT, NULL);
    }

    void cleanup()
    {
        if (!m_isInited) {
            return;
        }
        if(m_vboVertex) {
            glDeleteBuffers(1, &m_vboVertex);
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

private:
    MeshT &m_mesh;

    bool m_isInited;
    GLuint m_vao, m_vboVertex, m_vboIndex;
    int numsToDraw;
};

#endif // MESHGLHELPER_HH

