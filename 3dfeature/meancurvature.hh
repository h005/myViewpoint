#ifndef MEANCURVATURE_HH
#define MEANCURVATURE_HH

#include "common.hh"
#include "Curvature.hh"
#include "colormap.hh"
#include "abstractfeature.hh"

template <typename MeshT>
class MeanCurvature: public AbstractFeature<MeshT>
{
public:
    MeanCurvature(MeshT &in_mesh)
        : m_mesh(in_mesh), m_PropertyKeyword("Mean Curvature")
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
                if (curvatureMax < m_mesh.property(valuePerArea, *v_it))
                    curvatureMax = m_mesh.property(valuePerArea, *v_it);

            for (v_it = m_mesh.vertices_begin(); v_it != v_end; v_it++) {
                m_mesh.property(m_vPropHandle, *v_it) =
                        m_mesh.property(valuePerArea, *v_it) / curvatureMax;

            }

            m_mesh.remove_property(valuePerArea);
            m_mesh.remove_property(vertexBoundingArea);
        }
    }

    ~MeanCurvature()
    {
    }

    /**
     * @brief 将各个顶点的曲率结果，以颜色形式保存到绑定的mesh对象中，便于后续输出到文件
     */
    void assignVertexColor()
    {
        if (!m_mesh.has_vertex_colors())
            m_mesh.request_vertex_colors();

        typename MeshT::VertexIter v_it, v_end(m_mesh.vertices_end());
        for (v_it = m_mesh.vertices_begin(); v_it != v_end; v_it++) {
            double rgb[3];
            double v = m_mesh.property(m_vPropHandle, *v_it);
            ColorMap::jet(rgb, v, 0, 1);
            typename MeshT::Color color(rgb[0], rgb[1], rgb[2]);
            m_mesh.set_color(*v_it, color);
        }
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

