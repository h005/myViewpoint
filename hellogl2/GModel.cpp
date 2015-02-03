#include "GModel.h"
#include <opencv2/opencv.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
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



void GModel::recursive_create(const aiScene *sc, const aiNode* nd, const glm::mat4 &inheritedTransformation)
{
	assert(nd && sc);
    unsigned int n = 0;

    // 相对于父节点的变换，aiMatrix4x4中是行优先存储的，
    // 所以需要先将内存结构变为列优先存储
    glm::mat4 mTransformation = glm::transpose(glm::make_mat4((float *)&nd->mTransformation));
    glm::mat4 absoluteTransformation = inheritedTransformation * mTransformation;
    // 设置shader中的model view矩阵
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();

	for (; n < nd->mNumMeshes; ++n)
	{
        // 一个aiNode中存有其mesh的索引，
        // 在aiScene中可以用这个索引拿到真正的aiMesh
		const struct aiMesh* mesh = sc->mMeshes[nd->mMeshes[n]];
        meshEntries.push_back(new GModel::MeshEntry(mesh, absoluteTransformation, f));
    }


    // create all children
	for (n = 0; n < nd->mNumChildren; ++n)
	{
        recursive_create(sc, nd->mChildren[n], absoluteTransformation);
    }
}

GModel::GModel()
{
	pImporter = NULL;
	scene = NULL;
    textureIds = NULL;
    m_program = NULL;
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
	basePath = getBasePath(modelPath);

	return 1;
}

void GModel::bindDataToGL() {
    // 读入shader程序并编译
    // 需要在OpenGL环境下调用，放在这里合适
    m_program = new QOpenGLShaderProgram;
    m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, QString("simpleShader.vert"));
    m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, QString("simpleShader.frag"));
    m_program->link();

    // 1.将纹理读取到显存
    // 2.递归创建meshEntry
	textureIdMap.clear();
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

    recursive_create(scene, scene->mRootNode, glm::mat4());
}


void GModel::drawNormalizedModel(const glm::mat4 &inheritModelView, const glm::mat4 &projection) {
    assert(scene != NULL);
    glPushAttrib(GL_ENABLE_BIT);

    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();

    // 下面这个过程可以认作硬件实现的点变换
    // 对模型进行移中和缩放
    float scale = drawScale();
    glm::mat4 transformation = glm::scale(inheritModelView, glm::vec3(scale, scale, scale));
    transformation = glm::translate(transformation, glm::vec3(-scene_center.x, -scene_center.y, -scene_center.z));

    // 绑定使用的shader并设置其中的投影矩阵
    m_program->bind();
    QMatrix4x4 Q_projMatrix(glm::value_ptr(glm::transpose(projection)));
    m_program->setUniformValue("projMatrix", Q_projMatrix);

    std::vector<GModel::MeshEntry *>::iterator it;
    for (it = meshEntries.begin(); it != meshEntries.end(); it++) {
        // 绘制一个mesh主要分为下面几个步骤
        //
        // 1.应用材质、纹理
        // 2 设置Transformation
        // 3.调用MeshEntry的绘制函数:
        // 3.1 切换vao
        // 3.2 绘制三角形
        //

        const aiMesh *mesh = (*it)->mesh;
        // 一个aiMesh拥有一致的纹理和材质
//        f->glActiveTexture(GL_TEXTURE0);
        apply_material(scene->mMaterials[mesh->mMaterialIndex]);
//        m_program->setUniformValue("myTextureSampler", 0);
        if (mesh->HasNormals())
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

        // 计算最终的model view矩阵以及对应的法向变换矩阵
        glm::mat4 modelViewMatrix = transformation * (*it)->finalTransformation;
        glm::mat3 gl_NormalMatrix = glm::inverseTranspose(glm::mat3(modelViewMatrix));
        // 转换成Qt中的OpenGL类型
        QMatrix4x4 Q_modelViewMatrix(glm::value_ptr(glm::transpose(modelViewMatrix)));
        QMatrix3x3 Q_normalMatrix(glm::value_ptr(glm::transpose(gl_NormalMatrix)));
        // 在shader程序中设置
        m_program->setUniformValue("mvMatrix", Q_modelViewMatrix);
        m_program->setUniformValue("normalMatrix", Q_normalMatrix);

        (*it)->render();
    }
    m_program->release();

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
    if (m_program) {
        delete m_program;
        m_program = NULL;
    }

	scene = NULL;
    meshEntries.clear();
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

GModel::MeshEntry::MeshEntry(const aiMesh *mesh, const glm::mat4 &transformation, QOpenGLFunctions *f)
    : f(f), finalTransformation(transformation), mesh(mesh) {
    m_vao.create();
    m_vao.bind();

    elementCount = mesh->mNumFaces * 3;
    if(mesh->HasPositions()) {
        float *vertices = new float[mesh->mNumVertices * 3];
        for(uint32_t i = 0; i < mesh->mNumVertices; ++i) {
            vertices[i * 3] = mesh->mVertices[i].x;
            vertices[i * 3 + 1] = mesh->mVertices[i].y;
            vertices[i * 3 + 2] = mesh->mVertices[i].z;
        }

        m_vbo[VERTEX_BUFFER].create();
        m_vbo[VERTEX_BUFFER].bind();
        m_vbo[VERTEX_BUFFER].allocate(vertices, 3 * mesh->mNumVertices * sizeof(GLfloat));

        f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        f->glEnableVertexAttribArray (0);

        delete vertices;
    }


    if(mesh->HasTextureCoords(0)) {
        float *texCoords = new float[mesh->mNumVertices * 2];
        for(uint32_t i = 0; i < mesh->mNumVertices; ++i) {
            texCoords[i * 2] = mesh->mTextureCoords[0][i].x;
            texCoords[i * 2 + 1] = mesh->mTextureCoords[0][i].y;
        }

        m_vbo[TEXCOORD_BUFFER].create();
        m_vbo[TEXCOORD_BUFFER].bind();
        m_vbo[TEXCOORD_BUFFER].allocate(texCoords, 2 * mesh->mNumVertices * sizeof(GLfloat));

        f->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        f->glEnableVertexAttribArray (1);

        delete texCoords;
    }


    if(mesh->HasNormals()) {
        float *normals = new float[mesh->mNumVertices * 3];
        for(uint32_t i = 0; i < mesh->mNumVertices; ++i) {
            normals[i * 3] = mesh->mNormals[i].x;
            normals[i * 3 + 1] = mesh->mNormals[i].y;
            normals[i * 3 + 2] = mesh->mNormals[i].z;
        }

        m_vbo[NORMAL_BUFFER].create();
        m_vbo[NORMAL_BUFFER].bind();
        m_vbo[NORMAL_BUFFER].allocate(normals, 3 * mesh->mNumVertices * sizeof(GLfloat));

        f->glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        f->glEnableVertexAttribArray (2);

        delete normals;
    }


    if(mesh->HasFaces()) {
        unsigned int *indices = new unsigned int[mesh->mNumFaces * 3];
        for(uint32_t i = 0; i < mesh->mNumFaces; ++i) {
            indices[i * 3] = mesh->mFaces[i].mIndices[0];
            indices[i * 3 + 1] = mesh->mFaces[i].mIndices[1];
            indices[i * 3 + 2] = mesh->mFaces[i].mIndices[2];
            assert(mesh->mFaces[i].mNumIndices == 3);
        }

        m_vbo[INDEX_BUFFER].create();
        m_vbo[INDEX_BUFFER].bind();
        m_vbo[INDEX_BUFFER].allocate(indices, 3 * mesh->mNumFaces * sizeof(GLuint));

        f->glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        f->glEnableVertexAttribArray (3);

        delete indices;
    }

    m_vao.release();
}

/**
*	Deletes the allocated OpenGL buffers
**/
GModel::MeshEntry::~MeshEntry() {
    if(m_vbo[VERTEX_BUFFER].isCreated()) {
        m_vbo[VERTEX_BUFFER].destroy();
    }

    if(m_vbo[TEXCOORD_BUFFER].isCreated()) {
        m_vbo[TEXCOORD_BUFFER].destroy();
    }

    if(m_vbo[NORMAL_BUFFER].isCreated()) {
        m_vbo[NORMAL_BUFFER].destroy();
    }

    if(m_vbo[INDEX_BUFFER].isCreated()) {
        m_vbo[INDEX_BUFFER].destroy();
    }

    if (m_vao.isCreated()) {
        m_vao.release();
        m_vao.destroy();
    }
}

/**
*	Renders this MeshEntry
**/
void GModel::MeshEntry::render() {
    // 3.调用MeshEntry的绘制函数:
    // 3.1 切换vao
    // 3.2 绘制三角形
    m_vao.bind();
    // glDrawElements第三个参数似乎不对，一直crash；修改后也无法显示模型
    // glDrawElements(GL_TRIANGLES, elementCount, GL_UNSIGNED_INT, NULL);
    glDrawArrays(GL_TRIANGLES, 0, mesh->mNumVertices);
    m_vao.release();
}


