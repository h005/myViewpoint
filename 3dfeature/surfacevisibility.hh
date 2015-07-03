#ifndef SURFACEVISIBILITY_HH
#define SURFACEVISIBILITY_HH

#include "common.hh"
#include "abstractfeature.hh"

template <typename MeshT>
class SurfaceVisibility: public AbstractFeature<MeshT> {
public:
public:
    SurfaceVisibility(MeshT &in_mesh)
        : m_mesh(in_mesh), m_PropertyKeyword("Surface Visibility")
    {
        if(!m_mesh.get_property_handle(m_vPropHandle, m_PropertyKeyword))
            m_mesh.add_property(m_vPropHandle, m_PropertyKeyword);
    }

    ~SurfaceVisibility()
    {
    }

    /**
     * @brief 将各个顶点的曲率结果，以颜色形式保存到绑定的mesh对象中，便于后续输出到文件
     */
    void assignVertexColor()
    {
        assert(0);
    }

    double compute(const glm::mat3 &K, const glm::mat3 &R, const glm::vec3 &t)
    {
        return 1;
    }

private:
    MeshT &m_mesh;
    OpenMesh::VPropHandleT<double> m_vPropHandle;
    const char *m_PropertyKeyword;
};

#endif // SURFACEVISIBILITY_HH

