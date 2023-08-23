#pragma once

#include <assimp/scene.h>
#include<vector>
#include <GL/glew.h>
#include <iostream>
#include "texture.hpp"

class Mesh {

private:
	unsigned mVAO;
	unsigned mVBO;
	unsigned mEBO;
	unsigned mVertexCount;
	unsigned mIndexCount;
	unsigned mDiffuseTexture;
	unsigned mSpecularTexture;
	unsigned loadMeshTexture(const aiMaterial* material, const std::string& resPath, aiTextureType type);
	void processMesh(const aiMesh* mesh, const aiMaterial* material, const std::string& resPath);

public:
	std::vector<unsigned> mIndices;
	std::vector<float> mVertices;
	Mesh(const aiMesh* mesh, const aiMaterial* material, const std::string& resPath);
	void Render() const;
	std::vector<float> GetVertices() const;
};
