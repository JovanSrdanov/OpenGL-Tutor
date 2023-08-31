#include "mesh.hpp"

#include <glm/vec3.hpp>
#include <glm/detail/func_geometric.inl>

Mesh::Mesh(const aiMesh* mesh, const aiMaterial* material, const std::string &resPath) {
    processMesh(mesh, material, resPath);
}

void
Mesh::RenderWithTexture() const {
    glBindVertexArray(mVAO);

    if (mDiffuseTexture) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mDiffuseTexture);
    }

    if (mSpecularTexture) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, mSpecularTexture);
    }

    if (mIndexCount) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
        glDrawElements(GL_TRIANGLES, mIndexCount, GL_UNSIGNED_INT, (void*)0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        return;
    }

    glDrawArrays(GL_TRIANGLES, 0, mVertexCount);
    glBindVertexArray(0);
}
void
Mesh::RenderTriangles() const {
    glBindVertexArray(mVAO);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    if (mIndexCount) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
        glDrawElements(GL_TRIANGLES, mIndexCount, GL_UNSIGNED_INT, (void*)0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBindVertexArray(0);
}
void
Mesh::RenderFilledTriangles() const {
    glBindVertexArray(mVAO);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    if (mIndexCount) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
        glDrawElements(GL_TRIANGLES, mIndexCount, GL_UNSIGNED_INT, (void*)0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBindVertexArray(0);
}

void
Mesh::RenderVertices() const {
    glBindVertexArray(mVAO);
    if (mIndexCount) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
        glDrawElements(GL_POINTS, mIndexCount, GL_UNSIGNED_INT, (void*)0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    glBindVertexArray(0);
}

void
Mesh::RenderNormals() const {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glBindVertexArray(normal_lines_vao);
    glDrawArrays(GL_LINES, 0, normal_line_vertices.size() / 3);
    glBindVertexArray(0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}


unsigned
Mesh::loadMeshTexture(const aiMaterial* material, const std::string& resPath, aiTextureType type) {
    if (material && material->GetTextureCount(type) > 0) {
        aiString Path;
        if (material->GetTexture(type, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
            std::string FullPath = resPath + "/" + Path.data;
            unsigned TextureID = Texture::LoadImageToTexture(FullPath);
            return TextureID;
        }
    }

    return 0;
}

void
Mesh::processMesh(const aiMesh* mesh, const aiMaterial* material, const std::string& resPath) {
    const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

    for (unsigned VertexIndex = 0; VertexIndex < mesh->mNumVertices; ++VertexIndex) {
        std::vector<float> Position = { mesh->mVertices[VertexIndex].x, mesh->mVertices[VertexIndex].y, mesh->mVertices[VertexIndex].z };
        mVertices.insert(mVertices.end(), Position.begin(), Position.end());
        std::vector<float> Normals = { mesh->mNormals[VertexIndex].x, mesh->mNormals[VertexIndex].y, mesh->mNormals[VertexIndex].z };
        mVertices.insert(mVertices.end(), Normals.begin(), Normals.end());
        const aiVector3D* TexCoords = mesh->HasTextureCoords(0) ? &(mesh->mTextureCoords[0][VertexIndex]) : &Zero3D;
        std::vector<float> UV = { TexCoords->x, TexCoords->y };
        mVertices.insert(mVertices.end(), UV.begin(), UV.end());
    }

    for (unsigned FaceIndex = 0; FaceIndex < mesh->mNumFaces; ++FaceIndex) {
        const aiFace& Face = mesh->mFaces[FaceIndex];
        mIndices.push_back(Face.mIndices[0]);
        mIndices.push_back(Face.mIndices[1]);
        mIndices.push_back(Face.mIndices[2]);
    }

    mVertexCount = mVertices.size() / 8;
    mIndexCount = mIndices.size();

    mDiffuseTexture = loadMeshTexture(material, resPath, aiTextureType_DIFFUSE);
    mSpecularTexture = loadMeshTexture(material, resPath, aiTextureType_SPECULAR);

    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);
    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(float), mVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    if (mIndexCount) {
        glGenBuffers(1, &mEBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndexCount * sizeof(float), mIndices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    glBindVertexArray(0);

 
    for (size_t i = 0; i < mVertices.size(); i += 8) {
        float x = mVertices[i];
        float y = mVertices[i + 1];
        float z = mVertices[i + 2];
        float nx = mVertices[i + 3];
        float ny = mVertices[i + 4];
        float nz = mVertices[i + 5];

        // Calculate the endpoints of the normal line
        glm::vec3 start_point(x, y, z);
        glm::vec3 direction(nx, ny, nz);

        // Normalize the direction vector
        direction = normalize(direction);

        glm::vec3 scaled_direction = 0.2f * direction;
        glm::vec3 end_point = start_point + scaled_direction;

        normal_line_vertices.push_back(start_point.x);
        normal_line_vertices.push_back(start_point.y);
        normal_line_vertices.push_back(start_point.z);
        normal_line_vertices.push_back(end_point.x);
        normal_line_vertices.push_back(end_point.y);
        normal_line_vertices.push_back(end_point.z);
    }
   
    glGenVertexArrays(1, &normal_lines_vao);
    glBindVertexArray(normal_lines_vao);

    glGenBuffers(1, &normal_lines_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, normal_lines_vbo);
    glBufferData(GL_ARRAY_BUFFER, normal_line_vertices.size() * sizeof(float), normal_line_vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));
    glEnableVertexAttribArray(0);

    // Unbind VAO and VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

}
std::vector<float> Mesh::GetVertices() const {
    return mVertices;
}