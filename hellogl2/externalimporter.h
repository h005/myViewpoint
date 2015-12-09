#ifndef EXTERNALIMPORTER_H
#define EXTERNALIMPORTER_H

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/DefaultLogger.hpp>
#include <assimp/LogStream.hpp>

template <typename T>
class ExternalImporter
{
public:
    /**
     * @brief 读取一个模型文件的所有顶点
     * @param vertices 结果存放位置
     * @param path 模型所在路径
     * @return 是否读取成功
     */
    static bool read_vertices(std::vector<glm::vec3> &vertices, const char *path)
    {
        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(path, aiProcessPreset_TargetRealtime_Quality);
        if (!scene)
            return false;

        // 此时文件读取完毕，需要将Assimp中的数据结构转换为OpenMesh中的数据结构
        // aiScene呈现树形结构，递归转换每个节点
        recursive_create(scene, scene->mRootNode, glm::mat4(), vertices);
        std::cout << "Assimp Importer: " << vertices.size() << " Vertices Loaded." << std::endl;
        return true;
    }

private:
    static void recursive_create(const aiScene *sc, const aiNode* nd, const glm::mat4 &inheritedTransformation, std::vector<glm::vec3> &vertices)
    {
        assert(nd && sc);
        unsigned int n = 0;

        // 相对于父节点的变换，aiMatrix4x4中是行优先存储的，
        // 所以需要先将内存结构变为列优先存储
        glm::mat4 mTransformation = glm::transpose(glm::make_mat4((float *)&nd->mTransformation));
        glm::mat4 absoluteTransformation = inheritedTransformation * mTransformation;
        // 设置shader中的model view矩阵

        for (; n < nd->mNumMeshes; ++n)
        {
            // 一个aiNode中存有其mesh的索引，
            // 在aiScene中可以用这个索引拿到真正的aiMesh
            const struct aiMesh* mesh = sc->mMeshes[nd->mMeshes[n]];

            // 将所有点变换后，加入vector中
            if(mesh->HasPositions()) {
                for(uint32_t i = 0; i < mesh->mNumVertices; ++i) {
                    glm::vec3 position(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
                    glm::vec4 absolutePosition = absoluteTransformation * glm::vec4(position, 1.f);

                    vertices.push_back(glm::vec3(absolutePosition.x, absolutePosition.y, absolutePosition.z));
                }
            }

        }
        // create all children
        for (n = 0; n < nd->mNumChildren; ++n)
            recursive_create(sc, nd->mChildren[n], absoluteTransformation, vertices);
    }
};

#endif // EXTERNALIMPORTER_H
