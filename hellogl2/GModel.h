#pragma once
#include <map>
#include <QOpenGLShaderProgram>
#include <glm/glm.hpp>

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "assimp/DefaultLogger.hpp"
#include "assimp/LogStream.hpp"

typedef std::map<std::string, GLuint *> TextureIdMapType;

#define aisgl_min(x,y) (x<y?x:y)
#define aisgl_max(x,y) (y>x?y:x)

void *imgData(const char *texturePath, int &width, int &height);

class GModel
{
    static enum BUFFERS {
        VERTEX_BUFFER, TEXCOORD_BUFFER, NORMAL_BUFFER, INDEX_BUFFER, BUFFER_COUNT
    };

private:
	Assimp::Importer *pImporter;
	const aiScene *scene;
	std::string basePath;
	TextureIdMapType textureIdMap;
	GLuint *textureIds;
    GLuint mvMatrixID;

public:
	aiVector3D scene_min, scene_max, scene_center;

	GModel();
	bool load(const char *modelPath);
	bool hasModel();
	void bindTextureToGL();
	void drawNormalizedModel();
	float drawScale();
	~GModel();

private:
	void apply_material(const aiMaterial *mtl);
    void recursive_render(const aiScene *sc, const aiNode* nd, const glm::mat4 &inheritedTransformation);
	void cleanUp();
};

