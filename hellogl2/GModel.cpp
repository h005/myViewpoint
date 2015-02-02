#include "GModel.h"
#include <opencv2/opencv.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <qopenglfunctions.h>

void *imgData(const char *texturePath, int &width, int &height) {
	cv::Mat img = cv::imread(texturePath);
	if (img.rows <= 0 || img.cols <= 0) {
		return NULL;
	}
	assert(img.channels() == 3);
	
	cv::Mat fliped;
	cv::flip(img, fliped, 0);
	cv::Mat out;
	cv::cvtColor(fliped, out, CV_RGB2BGR);
	size_t mem = out.rows * out.cols * out.channels();
	uchar *data = new uchar[mem];
	size_t rowSize = out.cols * out.channels();
	uchar *pivot = data;
	for (int i = 0; i < out.rows; i++) {
		uchar *ptr = img.ptr(i);
		memcpy(pivot, ptr, rowSize);
		pivot += rowSize;
	}
	assert(pivot - data == mem);
	width = out.size().width;
	height = out.size().height;
	return data;
}

// Can't send color down as a pointer to aiColor4D because AI colors are ABGR.
static void Color4f(const aiColor4D *color)
{
	glColor4f(color->r, color->g, color->b, color->a);
}

static void set_float4(float f[4], float a, float b, float c, float d)
{
	f[0] = a;
	f[1] = b;
	f[2] = c;
	f[3] = d;
}

static void color4_to_float4(const aiColor4D *c, float f[4])
{
	f[0] = c->r;
	f[1] = c->g;
	f[2] = c->b;
	f[3] = c->a;
}

static std::string getBasePath(const std::string& path)
{
	size_t pos = path.find_last_of("\\/");
	return (std::string::npos == pos) ? "" : path.substr(0, pos + 1);
}

static void get_bounding_box_for_node(const aiScene *sc,
	const aiNode* nd,
	aiVector3D* min,
	aiVector3D* max,
	aiMatrix4x4 &prev
	){
	unsigned int n = 0, t;

	aiMatrix4x4 trafo = prev;
	trafo *= nd->mTransformation;

	for (; n < nd->mNumMeshes; ++n) {
		const aiMesh* mesh = sc->mMeshes[nd->mMeshes[n]];
		for (t = 0; t < mesh->mNumVertices; ++t) {

			aiVector3D tmp = mesh->mVertices[t];
			tmp *= trafo;

			min->x = aisgl_min(min->x, tmp.x);
			min->y = aisgl_min(min->y, tmp.y);
			min->z = aisgl_min(min->z, tmp.z);

			max->x = aisgl_max(max->x, tmp.x);
			max->y = aisgl_max(max->y, tmp.y);
			max->z = aisgl_max(max->z, tmp.z);
		}
	}

	for (n = 0; n < nd->mNumChildren; ++n) {
		get_bounding_box_for_node(sc, nd->mChildren[n], min, max, trafo);
	}
}

static void get_bounding_box(const aiScene *sc, aiVector3D* min, aiVector3D* max)
{
	// set identity
	aiMatrix4x4 rootTransformation;

	min->x = min->y = min->z = 1e10f;
	max->x = max->y = max->z = -1e10f;
	get_bounding_box_for_node(sc, sc->mRootNode, min, max, rootTransformation);
}

void GModel::apply_material(const aiMaterial *mtl)
{
	float c[4];

	GLenum fill_mode;
	int ret1, ret2;
	aiColor4D diffuse;
	aiColor4D specular;
	aiColor4D ambient;
	aiColor4D emission;
	float shininess, strength;
	int two_sided;
	int wireframe;
	unsigned int max;	// changed: to unsigned

	int texIndex = 0;
	aiString texPath;	//contains filename of texture

	if (AI_SUCCESS == mtl->GetTexture(aiTextureType_DIFFUSE, texIndex, &texPath) && textureIdMap[texPath.data])
	{
		//bind texture
		unsigned int texId = *textureIdMap[texPath.data];
		glBindTexture(GL_TEXTURE_2D, texId);
	}

	set_float4(c, 0.8f, 0.8f, 0.8f, 1.0f);
	if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_DIFFUSE, &diffuse))
		color4_to_float4(&diffuse, c);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, c);

	set_float4(c, 0.0f, 0.0f, 0.0f, 1.0f);
	if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_SPECULAR, &specular))
		color4_to_float4(&specular, c);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, c);

	set_float4(c, 0.2f, 0.2f, 0.2f, 1.0f);
	if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_AMBIENT, &ambient))
		color4_to_float4(&ambient, c);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, c);

	set_float4(c, 0.0f, 0.0f, 0.0f, 1.0f);
	if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_EMISSIVE, &emission))
		color4_to_float4(&emission, c);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, c);

	max = 1;
	ret1 = aiGetMaterialFloatArray(mtl, AI_MATKEY_SHININESS, &shininess, &max);
	max = 1;
	ret2 = aiGetMaterialFloatArray(mtl, AI_MATKEY_SHININESS_STRENGTH, &strength, &max);
	if ((ret1 == AI_SUCCESS) && (ret2 == AI_SUCCESS))
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess * strength);
	else {
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0.0f);
		set_float4(c, 0.0f, 0.0f, 0.0f, 0.0f);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, c);
	}

	max = 1;
	if (AI_SUCCESS == aiGetMaterialIntegerArray(mtl, AI_MATKEY_ENABLE_WIREFRAME, &wireframe, &max))
		fill_mode = wireframe ? GL_LINE : GL_FILL;
	else
		fill_mode = GL_FILL;
	glPolygonMode(GL_FRONT_AND_BACK, fill_mode);

	max = 1;
	if ((AI_SUCCESS == aiGetMaterialIntegerArray(mtl, AI_MATKEY_TWOSIDED, &two_sided, &max)) && two_sided)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);
}



void GModel::recursive_render(const aiScene *sc, const aiNode* nd, const glm::mat4 &inheritedTransformation)
{
	assert(nd && sc);
	unsigned int i;
	unsigned int n = 0, t;

    // 相对于父节点的变换，aiMatrix4x4中是行优先存储的，
    // 所以需要先将内存结构变为列优先存储
    glm::mat4 mTransformation = glm::transpose(glm::make_mat4((float *)&nd->mTransformation));
    glm::mat4 absoluteTransformation = inheritedTransformation * mTransformation;
    // 设置shader中的model view矩阵
    glUniformMatrix4fv(mvMatrixID, 1, GL_FALSE,glm::value_ptr(absoluteTransformation));

	for (; n < nd->mNumMeshes; ++n)
	{
        // 一个aiNode中存有其mesh的索引，
        // 在aiScene中可以用这个索引拿到真正的aiMesh
		const struct aiMesh* mesh = sc->mMeshes[nd->mMeshes[n]];

        // 一个aiMesh拥有一致的纹理和材质
		apply_material(sc->mMaterials[mesh->mMaterialIndex]);

		if (mesh->mNormals == NULL)
		{
			glDisable(GL_LIGHTING);
		}
		else
		{
			glEnable(GL_LIGHTING);
		}

		if (mesh->mColors[0] != NULL)
		{
			glEnable(GL_COLOR_MATERIAL);
		}
		else
		{
			glDisable(GL_COLOR_MATERIAL);
		}

        // 将顶点的数据拷贝到显存中
        GLuint vbo[BUFFER_COUNT];

        if(mesh->HasPositions()) {
            float *vertices = new float[mesh->mNumVertices * 3];
            for(int i = 0; i < mesh->mNumVertices; ++i) {
                vertices[i * 3] = mesh->mVertices[i].x;
                vertices[i * 3 + 1] = mesh->mVertices[i].y;
                vertices[i * 3 + 2] = mesh->mVertices[i].z;
            }

            glGenBuffers(1, &vbo[VERTEX_BUFFER]);
            glBindBuffer(GL_ARRAY_BUFFER, vbo[VERTEX_BUFFER]);
            glBufferData(GL_ARRAY_BUFFER, 3 * mesh->mNumVertices * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
            glEnableVertexAttribArray (0);

            delete vertices;
        }


        if(mesh->HasTextureCoords(0)) {
            float *texCoords = new float[mesh->mNumVertices * 2];
            for(int i = 0; i < mesh->mNumVertices; ++i) {
                texCoords[i * 2] = mesh->mTextureCoords[0][i].x;
                texCoords[i * 2 + 1] = mesh->mTextureCoords[0][i].y;
            }

            glGenBuffers(1, &vbo[TEXCOORD_BUFFER]);
            glBindBuffer(GL_ARRAY_BUFFER, vbo[TEXCOORD_BUFFER]);
            glBufferData(GL_ARRAY_BUFFER, 2 * mesh->mNumVertices * sizeof(GLfloat), texCoords, GL_STATIC_DRAW);

            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
            glEnableVertexAttribArray (1);

            delete texCoords;
        }

        if(mesh->HasNormals()) {
            float *normals = new float[mesh->mNumVertices * 3];
            for(int i = 0; i < mesh->mNumVertices; ++i) {
                normals[i * 3] = mesh->mNormals[i].x;
                normals[i * 3 + 1] = mesh->mNormals[i].y;
                normals[i * 3 + 2] = mesh->mNormals[i].z;
            }

            glGenBuffers(1, &vbo[NORMAL_BUFFER]);
            glBindBuffer(GL_ARRAY_BUFFER, vbo[NORMAL_BUFFER]);
            glBufferData(GL_ARRAY_BUFFER, 3 * mesh->mNumVertices * sizeof(GLfloat), normals, GL_STATIC_DRAW);

            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);
            glEnableVertexAttribArray (2);

            delete normals;
        }


        if(mesh->HasFaces()) {
            unsigned int *indices = new unsigned int[mesh->mNumFaces * 3];
            for(int i = 0; i < mesh->mNumFaces; ++i) {
                indices[i * 3] = mesh->mFaces[i].mIndices[0];
                indices[i * 3 + 1] = mesh->mFaces[i].mIndices[1];
                indices[i * 3 + 2] = mesh->mFaces[i].mIndices[2];
            }

            glGenBuffers(1, &vbo[INDEX_BUFFER]);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[INDEX_BUFFER]);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * mesh->mNumFaces * sizeof(GLuint), indices, GL_STATIC_DRAW);

            glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, NULL);
            glEnableVertexAttribArray (3);

            delete indices;
        }
        glDrawArrays(GL_TRIANGLES, 0, mesh->mNumVertices * 3);

//        // 一个mesh拥有多个面
//		for (t = 0; t < mesh->mNumFaces; ++t) {
//			const struct aiFace* face = &mesh->mFaces[t];
//			GLenum face_mode;

//			switch (face->mNumIndices)
//			{
//			case 1: face_mode = GL_POINTS; break;
//			case 2: face_mode = GL_LINES; break;
//			case 3: face_mode = GL_TRIANGLES; break;
//			default: face_mode = GL_POLYGON; break;
//			}

//			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
//			glBegin(face_mode);
//			for (i = 0; i < face->mNumIndices; i++)		// go through all vertices in face
//			{
//				int vertexIndex = face->mIndices[i];	// get group index for current index
//				if (mesh->mColors[0] != NULL)
//					Color4f(&mesh->mColors[0][vertexIndex]);
//				if (mesh->mNormals != NULL) {
//					if (mesh->HasTextureCoords(0))		//HasTextureCoords(texture_coordinates_set)
//					{
//						glTexCoord2f(mesh->mTextureCoords[0][vertexIndex].x, 1 - mesh->mTextureCoords[0][vertexIndex].y); //mTextureCoords[channel][vertex]
//					}
//					glNormal3fv(&mesh->mNormals[vertexIndex].x);
//				}
//				// 实际上传入的是x, y, z的序列
//				glVertex3fv(&mesh->mVertices[vertexIndex].x);
//			}
//			glEnd();
//		}

    }


	// draw all children
	for (n = 0; n < nd->mNumChildren; ++n)
	{
        recursive_render(sc, nd->mChildren[n], absoluteTransformation);
    }
}

GModel::GModel()
{
	pImporter = NULL;
	scene = NULL;
	textureIds = NULL;
}

bool GModel::load(const char *modelPath) {
	cleanUp();

	pImporter = new Assimp::Importer();
	scene = pImporter->ReadFile(modelPath, aiProcessPreset_TargetRealtime_Quality);
	if (!scene) {
		return 0;
	}

	// Generate Model Information
	get_bounding_box(scene, &scene_min, &scene_max);
	scene_center.x = (scene_min.x + scene_max.x) / 2.f;
	scene_center.y = (scene_min.y + scene_max.y) / 2.f;
	scene_center.z = (scene_min.z + scene_max.z) / 2.f;
	printf("%f %f %f\n", scene_center.x, scene_center.y, scene_center.z);
	basePath = getBasePath(modelPath);

	return 1;
}

void GModel::bindTextureToGL() {
	textureIdMap.clear();

	printf("%d %d %d %d\n", scene->mNumMeshes, scene->mNumTextures, scene->mNumMaterials, scene->HasTextures());
	for (unsigned int m = 0; m<scene->mNumMaterials; m++)
	{
		int texIndex = 0;
		aiReturn texFound;
		aiString path;	// filename
		while ((texFound = scene->mMaterials[m]->GetTexture(aiTextureType_DIFFUSE, texIndex, &path)) == AI_SUCCESS)
		{
			textureIdMap[path.data] = NULL;
			texIndex++;
		}
	}

	size_t numTextures = textureIdMap.size();
	textureIds = new GLuint[numTextures];
	glGenTextures(numTextures, textureIds);
	TextureIdMapType::iterator it;
	int i;
	for (i = 0, it = textureIdMap.begin(); it != textureIdMap.end(); it++, i++) {
		std::string filename = basePath + it->first;

		glBindTexture(GL_TEXTURE_2D, textureIds[i]); /* Binding of texture name */
		//redefine standard texture values
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); /* We will use linear
																		  interpolation for magnification filter */
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); /* We will use linear
																		  interpolation for minifying filter */

		int width, height;
		void *data = imgData(filename.c_str(), width, height);
		if (data) {
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width,
				height, 0, GL_RGB, GL_UNSIGNED_BYTE,
				data); /* Texture specification */
			delete data;

			it->second = &textureIds[i];
		}
	}
}


void GModel::drawNormalizedModel(GLuint mvMatrixID, const glm::mat4 &initTransformation) {
    this->mvMatrixID = mvMatrixID;
    assert(scene != NULL);
    glPushAttrib(GL_ENABLE_BIT);

    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

    // 下面这个过程可以认作硬件实现的点变换
    float scale = drawScale();
    glm::mat4 transform = glm::scale(initTransformation, glm::vec3(scale, scale, scale));
    transform = glm::translate(transform, glm::vec3(-scene_center.x, -scene_center.y, -scene_center.z));
    recursive_render(scene, scene->mRootNode, transformation);

    glPopAttrib();

}

void GModel::cleanUp() {
	if (pImporter) {
		delete pImporter;
		pImporter = NULL;
	}

	if (textureIds) {
		glDeleteTextures(textureIdMap.size(), textureIds);
		delete textureIds;
		textureIds = NULL;
	}

	scene = NULL;
}

bool GModel::hasModel() {
	return scene != NULL;
}

float GModel::drawScale() {
	float tmp = -1e10;
	tmp = aisgl_max(scene_max.x - scene_min.x, tmp);
	tmp = aisgl_max(scene_max.y - scene_min.y, tmp);
	tmp = aisgl_max(scene_max.z - scene_min.z, tmp);
	float scale = 2.f / tmp;
	return scale;
}


GModel::~GModel()
{
	cleanUp();
}
