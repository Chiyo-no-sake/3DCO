//TODO: mesh will be a field inside the aiNode
//TODO: update scene node to use composition with mesh instead of inheritance

#include "AssimpStrategy.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "glm/glm.hpp"

#include "../model/coNode.h"

#define uint unsigned int

const aiScene* parsingScene;

coNode* parseNode(aiNode* aiNode);
coNode* parseLights(aiScene* aiScene);

glm::mat4 convertMatrix(const aiMatrix4x4& aiMat);
coMesh* parseMesh(uint meshIndex);

inline glm::vec3 convertColor(aiColor3D c) {
	return glm::vec3{ c.r, c.g, c.b };
}

inline glm::vec3 convertVec3(aiVector3D v) {
	return glm::vec3{ v.x, v.y, v.z };
}

inline glm::vec2 convertVec2(aiVector3D v) {
	return glm::vec2{ v.x, v.y };
}

void AssimpStrategy::execute()
{
	CG_CR_LOG_INFO("Starting to parse");
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(m_filepath,
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_GenSmoothNormals |
		aiProcess_FindInvalidData);
	parsingScene = scene;

	CG_CR_LOG_INFO("Finished parsing");

	CG_CR_LOG_INFO("Starting nodes conversion");
	m_parsed = parseNode(scene->mRootNode);
	CG_CR_LOG_INFO("Finished converting nodes");

	if (scene->HasLights()) {
		CG_CR_LOG_INFO("Starting lights conversion");

	}
}

coNode* parseLightsIntoScene(aiScene* aiScene, coNode* targetScene) {
	coNode* parsedLightsRoot = nullptr;

	for (int i = 0; i < aiScene->mNumLights; i++) {

		coLight* ourLight;
		aiLight* theirLight = aiScene->mLights[i];

		switch (theirLight->mType) {
		case aiLightSourceType::aiLightSource_DIRECTIONAL:
			auto light = new CGLib::DirectionalLight();
			
			ourLight = light;
			break;

		case aiLightSourceType::aiLightSource_POINT:
			auto light = new CGLib::OmniDirectionalLight();

			light->setPosition(theirLight->mPosition);
			ourLight = light;
			break;

		case aiLightSourceType::aiLightSource_SPOT:
			auto light = new CGLib::SpotLight();

			ourLight = light;
			break;
		default:
			CG_CR_LOG_WARN("Light {} is of unknown type", theirLight->mName);
		}

		ourLight->setLinearAttenuation(theirLight->mAttenuationLinear);
		ourLight->setConstantAttenuation(theirLight->mAttenuationConstant);
		ourLight->setQuadraticAttenuation(theirLight->mAttenuationQuadratic);

		ourLight->setAmbient(convertColor(theirLight->mColorAmbient));
		ourLight->setDiffuse(convertColor(theirLight->mColorDiffuse));
		ourLight->setSpecular(convertColor(theirLight->mColorSpecular));
	}

	
}

coNode* parseNode(aiNode* aiNode) {
	coNode* node = new CGLib::EmptyNode();

	CG_CR_LOG_INFO("converting {}", aiNode->mName.C_Str());

	node->m_name = aiNode->mName.C_Str();
	node->m_transform = (convertMatrix(aiNode->mTransformation);

	// node has meshes
	if (aiNode->mNumMeshes != 0) {
		CG_CR_LOG_INFO("found {} mesh to add as children", aiNode->mNumMeshes);

		for (int i = 0; i < aiNode->mNumMeshes; i++) {
			node->addChild(parseMesh(aiNode->mMeshes[i]));
		}
	}

	// node is light

	for (int i = 0; i < aiNode->mNumChildren; i++) {
		CG_CR_LOG_INFO("parsing child #{} of {}", i, node->m_name);

		node->addChild(parseNode(aiNode->mChildren[i]));
	}

	return node;
}

glm::mat4 convertMatrix(const aiMatrix4x4& aiMat) {
	return {
		aiMat.a1, aiMat.b1, aiMat.c1, aiMat.d1,
		aiMat.a2, aiMat.b2, aiMat.c2, aiMat.d2,
		aiMat.a3, aiMat.b3, aiMat.c3, aiMat.d3,
		aiMat.a4, aiMat.b4, aiMat.c4, aiMat.d4
	};
}

coMesh* parseMesh(uint meshIndex) {
	// TODO look for tangent
	CG_CR_LOG_INFO("converting mesh");

	aiMesh* mesh = parsingScene->mMeshes[meshIndex];
	uint allocatedTris = 0;
	uint* triangles = (uint*)malloc(sizeof(uint) * mesh->mNumFaces * 3);
	glm::vec3* vertices = (glm::vec3*)malloc(sizeof(glm::vec3) * mesh->mNumVertices);
	glm::vec3* normals = (glm::vec3*)malloc(sizeof(glm::vec3) * mesh->mNumVertices);
	glm::vec2* mapping = (glm::vec2*)malloc(sizeof(glm::vec2) * mesh->mNumVertices);

	CG_CR_LOG_INFO("num triangles: {}", mesh->mNumFaces);
	CG_CR_LOG_INFO("num vertices: {}", mesh->mNumVertices);

	if (mesh->mNumFaces == 0) {
		CG_CR_LOG_WARN("Mesh has no face");
	}

	if (mesh->mNumVertices == 0) {
		CG_CR_LOG_WARN("Mesh has no vertices");
	}

	for (int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		if (i == 0) {
			triangles = (uint*)malloc(sizeof(uint) * face.mNumIndices);
		}
		else {
			triangles = (uint*)realloc(triangles, sizeof(uint) * (allocatedTris + face.mNumIndices));
		}
		allocatedTris += face.mNumIndices;

		for (uint j = 0; j < face.mNumIndices; j++) {
			triangles[i * 3 + j] = face.mIndices[j];
			CG_CR_LOG_TRACE("parsed index: {}", triangles[i * 3 + j]);
		}
	}

	for (int i = 0; i < mesh->mNumVertices; i++) {
		// parse vertex
		vertices[i] = convertVec3(mesh->mVertices[i]);
		CG_CR_LOG_TRACE("parsed vertex: {}, {}, {}", vertices[i].x, vertices[i].y, vertices[i].z);


		// parse normals
		normals[i] = convertVec3(mesh->mNormals[i]);
		CG_CR_LOG_TRACE("parsed normal: {}, {}, {}", vertices[i].x, vertices[i].y, vertices[i].z);
		
		mapping[i] = convertVec2(mesh->mTextureCoords[0][i]);
		CG_CR_LOG_TRACE("parsed uv: {}, {}", mapping[i].x, mapping[i].y);
	}

	coMesh* newMesh = new coMesh(
		vertices, triangles, mesh->mNumVertices, mesh->mNumFaces);

	newMesh->setMapping(mapping);
	newMesh->setNormals(normals);

	CG_CR_LOG_INFO("mesh converted");

	return newMesh;
}