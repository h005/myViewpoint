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
        if(!m_mesh.get_property_handle(m_vPropHandle, m_PropertyKeyword)) {
            m_mesh.add_property(m_vPropHandle, m_PropertyKeyword);

            OpenMesh::VPropHandleT<double> valuePerArea;
            OpenMesh::VPropHandleT<double> vertexBoundingArea;
            m_mesh.add_property(valuePerArea);
            m_mesh.add_property(vertexBoundingArea);

            typename MeshT::VertexIter v_it, v_end(m_mesh.vertices_end());
            for (v_it = m_mesh.vertices_begin(); v_it != v_end; v_it++) {
                OpenMesh::VectorT<float,3> n;
                double area;
                curvature::discrete_mean_curv_op<MeshT>(m_mesh, *v_it, n, area);
                m_mesh.property(valuePerArea, *v_it) = n.norm() / 2.0;
                m_mesh.property(vertexBoundingArea, *v_it) = area;
            }

            // TODO:
            // 优化缩放比的选择
            // 这里可能存在问题，由于包围面积过小，除法后结果值过大
            double curvatureMax = -1;
            for (v_it = m_mesh.vertices_begin(); v_it != v_end; v_it++)
                if (m_mesh.property(vertexBoundingArea, *v_it) > 1e-3
                        && curvatureMax < m_mesh.property(valuePerArea, *v_it)) {
                    curvatureMax = m_mesh.property(valuePerArea, *v_it);
                    std::cout << m_mesh.property(valuePerArea, *v_it) << " " <<  m_mesh.property(vertexBoundingArea, *v_it) << std::endl;
                }

            for (v_it = m_mesh.vertices_begin(); v_it != v_end; v_it++)
                m_mesh.property(m_vPropHandle, *v_it) =
                        m_mesh.property(valuePerArea, *v_it) / curvatureMax * m_mesh.property(vertexBoundingArea, *v_it);

            m_mesh.remove_property(valuePerArea);
            m_mesh.remove_property(vertexBoundingArea);
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

