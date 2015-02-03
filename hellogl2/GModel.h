#pragma once
#include <map>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <glm/glm.hpp>

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "assimp/DefaultLogger.hpp"
#include "assimp/LogStream.hpp"

typedef std::map<std::string, GLuint *> TextureIdMapType;

#define aisgl_min(x,y) (x<y?x:y)
#define aisgl_max(x,y) (y>x?y:x)

class QOpenGLFunctions;
void *imgData(const char *texturePath, int &width, int &height);

class GModel
{
    class MeshEntry {
    private:
        // 用于描述OpenGLContext
        QOpenGLFunctions *f;
        // 该mesh最终的Model矩阵
        QOpenGLVertexArrayObject m_vao;
        QOpenGLBuffer *m_vbo[4];
        int elementCount;

    public:
        static enum BUFFERS {
            VERTEX_BUFFER, TEXCOORD_BUFFER, NORMAL_BUFFER, INDEX_BUFFER
        };
        const aiMesh *mesh;
        glm::mat4 finalTransformation;

        MeshEntry(const aiMesh *mesh, const glm::mat4 &transformation, QOpenGLFunctions *f);
        ~MeshEntry();
        void render();
    };

private:
	Assimp::Importer *pImporter;
	const aiScene *scene;
	std::string basePath;
	TextureIdMapType textureIdMap;
	GLuint *textureIds;
    std::vector<MeshEntry *> meshEntries;
    QOpenGLShaderProgram *m_program;

public:
	aiVector3D scene_min, scene_max, scene_center;

	GModel();
	bool load(const char *modelPath);
	bool hasModel();
    void bindDataToGL();
    void drawNormalizedModel(const glm::mat4 &inheritModelView, const glm::mat4 &projection);
	float drawScale();
	~GModel();

private:
	void apply_material(const aiMaterial *mtl);
    void recursive_create(const aiScene *sc, const aiNode* nd, const glm::mat4 &inheritedTransformation);
	void cleanUp();
};

