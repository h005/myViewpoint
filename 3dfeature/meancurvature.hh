#ifndef MEANCURVATURE_HH
#define MEANCURVATURE_HH

#include "common.hh"
#include "Curvature.hh"

template <typename MeshT>
class MeanCurvature
{
public:
    MeanCurvature(MeshT &mesh)
        : m_mesh(mesh), m_PropertyKeyword("Mean Curvature")
    {
        if(!m_mesh.get_property_handle(m_vPropHandle, m_PropertyKeyword))
            m_mesh.add_property(m_vPropHandle, m_PropertyKeyword);

        typename MeshT::VertexIter v_it, v_end(m_mesh.vertices_end());
        for (v_it = m_mesh.vertices_begin(); v_it != v_end; v_it++) {
            glm::vec3 n;
            double area;
            curvature::discrete_mean_curv_op<MeshT>(m_mesh, *v_it, n, area);
//            m_mesh.property(m_vPropHandle, *v_it) = curv;
//            std::cout << m_mesh.property(m_vPropHandle, *v_it) << std::endl;
        }
    }

    ~MeanCurvature()
    {
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

#endif // MEANCURVATURE_HH

