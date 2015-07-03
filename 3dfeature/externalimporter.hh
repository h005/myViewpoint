#ifndef EXTERNALIMPORTER_H
#define EXTERNALIMPORTER_H

#include "common.hh"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

template <typename MeshT>
class ExternalImporter
{
public:
    /**
     * @brief 读取一个外部文件，类似于OpenMesh::IO::read_mesh
     * @param mesh 结果存放位置
     * @param path 文件路径
     * @return 是否读取成功
     * @see OpenMesh::IO::read_mesh
     */
    static bool read_mesh(MeshT &mesh, char *path)
    {
        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(path, aiProcessPreset_TargetRealtime_Quality);
        if (!scene)
            return false;

        // 此时文件读取完毕，需要将Assimp中的数据结构转换为OpenMesh中的数据结构
        // aiScene呈现树形结构，递归转换每个节点
        int count = 0;
        recursive_create(scene, scene->mRootNode, glm::mat4(), mesh, count);
        std::cout << "Assimp Importer: " << count << " Meshes Loaded." << std::endl;
        return true;
    }

private:
    static void recursive_create(const aiScene *sc, const aiNode* nd, const glm::mat4 &inheritedTransformation, MeshT &openMesh, int &count)
    {
        assert(nd && sc);
        unsigned int n = 0;

        // 相对于父节点的变换，aiMatrix4x4中是行优先存储的，
        // 所以需要先将内存结构变为列优先存储
        glm::mat4 mTransformation = glm::transpose(glm::make_mat4((float *)&nd->mTransformation));
        glm::mat4 absoluteTransformation = inheritedTransformation * mTransformation;
        // 设置shader中的model view矩阵

        // 累计遍历的mesh个数，读取完毕后用于输出
        count += nd->mNumMeshes;
        //
        for (; n < nd->mNumMeshes; ++n)
        {
            // 一个aiNode中存有其mesh的索引，
            // 在aiScene中可以用这个索引拿到真正的aiMesh
            const struct aiMesh* mesh = sc->mMeshes[nd->mMeshes[n]];

            // 将所有点变换后，加入OpenMesh结构中，并保存它们的索引
            std::vector<typename MeshT::VertexHandle> vHandle;
            if(mesh->HasPositions()) {
                for(uint32_t i = 0; i < mesh->mNumVertices; ++i) {
                    glm::vec3 position(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
                    glm::vec4 absolutePosition = absoluteTransformation * glm::vec4(position, 1.f);

                    typename MeshT::Point point(absolutePosition.x, absolutePosition.y, absolutePosition.z);
                    vHandle.push_back(openMesh.add_vertex(point));
                }
            }

            if(mesh->HasFaces() && vHandle.size() > 0) {
                std::vector<typename MeshT::VertexHandle> fHandle(3);
                // 一个face代表一个面（暂时只考虑三角形，其余类型pass），其存储着各个顶点的索引
                // 可以根据索引到mesh->mVertices[]中找到对应顶点的数据(x, y, z)
                for(uint32_t i = 0; i < mesh->mNumFaces; ++i) {
                    if (mesh->mFaces[i].mNumIndices == 3) {
                        fHandle[0] = vHandle[mesh->mFaces[i].mIndices[0]];
                        fHandle[1] = vHandle[mesh->mFaces[i].mIndices[1]];
                        fHandle[2] = vHandle[mesh->mFaces[i].mIndices[2]];
                        openMesh.add_face(fHandle);
                    }
                }
            }
        }


        // create all children
        for (n = 0; n < nd->mNumChildren; ++n)
            recursive_create(sc, nd->mChildren[n], absoluteTransformation, openMesh, count);
    }
};

#endif // EXTERNALIMPORTER_H
