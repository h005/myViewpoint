#ifndef RENDERINTERFACE_HH
#define RENDERINTERFACE_HH

#include "common.hh"
#include <vector>

template <typename MeshT>
class RenderInterface {
public:
    /**
     * @brief 给定一个视点，得到该视点下可见点的列表
     * @param K 相机内参数
     * @param R 外参数中的旋转分量
     * @param t 外参数中的平移分量
     * @param mesh 给定的网格
     * @return
     */
    static std::vector<typename MeshT::VertexHandle> visiblePoints(
            const glm::mat3 &K,
            const glm::mat3 &R,
            const glm::vec3 &t,
            MeshT &mesh)
    {
        return std::vector<typename MeshT::VertexHandle>();
    }
};


#endif // RENDERINTERFACE_HH

