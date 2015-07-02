#ifndef GAUSSCURVATURE_H
#define GAUSSCURVATURE_H

#include "common.hh"
#include "Curvature.hh"

template <typename MeshT>
class GaussCurvature
{
public:
    GaussCurvature(MeshT &mesh)
        : m_mesh(mesh), m_PropertyKeyword("Gauss Curvature")
    {
        if(!m_mesh.get_property_handle(m_vPropHandle, m_PropertyKeyword))
            m_mesh.add_property(m_vPropHandle, m_PropertyKeyword);

        typename MeshT::VertexIter v_it, v_end(m_mesh.vertices_end());
        for (v_it = m_mesh.vertices_begin(); v_it != v_end; v_it++) {
            double curv = curvature::gauss_curvature<MeshT>(m_mesh, *v_it);
            m_mesh.property(m_vPropHandle, *v_it) = curv;
        }
    }

    ~GaussCurvature()
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

#endif // GAUSSCURVATURE_H
