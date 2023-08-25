#ifndef MESH_H

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <algorithm>
#include <vector>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shader.hpp"
#include "mesh.hpp"

#define POSITION_LOCATION 0
#define NORMAL_LOCATION 1

#define POSTPROCESS_FLAGS (aiProcess_Triangulate)
// TOOD: (Jovan): IF model loads with bad textures, use this instead
// #define POSTPROCESS_FLAGS (aiProcess_Triangulate | aiProcess_FlipUVs)
#define INVALID_MATERIAL 0xFFFFFFFF

enum EBufferType {
	INDEX_BUFFER = 0,
	POS_VB = 1,
	TEXCOORD_VB = 2,
	NORM_VB = 3,
	BUFFER_COUNT = 4,
};

class Model {
public:
	std::vector<Mesh> mMeshes;

public:
	std::string mFilename;
	std::string mDirectory;
	Model(std::string filename);
	bool Load();
	void Render();
	std::vector<float> GetVertices() const;
};

#define MESH_HP
#endif