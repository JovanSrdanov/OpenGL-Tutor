#include "mesh.hpp"

#include <fstream>
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
void
Mesh::RenderAveragedNormals() const {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glBindVertexArray(averaged_normal_lines_vao);
    glDrawArrays(GL_LINES, 0, averaged_normal_vertices.size() / 3);
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




bool contains_element(const glm::vec3& target, const std::vector<glm::vec3>& added) {
    return std::find(added.begin(), added.end(), target) != added.end();
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

   
    std::ifstream inputFile("averaged_normal_vertices.txt");
    if (inputFile.is_open()) {
        float value;
        while (inputFile >> value) {
            averaged_normal_vertices.push_back(value);
        }
        inputFile.close();
    }
    else {

        std::vector<glm::vec3> added_to_vertices;
        for (size_t i = 0; i < mVertices.size(); i += 8) {
            glm::vec3 averaged_normal(0.0f);
            std::vector<glm::vec3> added_to_current_normal_calculation;

            if (i % 8000 == 0)
            {
                std::cout << i << "\n";
                std::cout << mVertices.size() << "\n";
            }

            float start_x = mVertices[i];
            float start_y = mVertices[i + 1];
            float start_z = mVertices[i + 2];

            if (!contains_element(glm::vec3(start_x, start_y, start_z), added_to_vertices))
            {
                added_to_vertices.emplace_back(start_x, start_y, start_z);
            }
            else
            {
                continue;
            }


            for (size_t j = i; j < mVertices.size(); j += 8) {
                float current_x = mVertices[j];
                float current_y = mVertices[j + 1];
                float current_z = mVertices[j + 2];
                float current_nx = mVertices[j + 3];
                float current_ny = mVertices[j + 4];
                float current_nz = mVertices[j + 5];


                if (start_x == current_x && start_y == current_y && start_z == current_z) {
                    glm::vec3 current_normal(current_nx, current_ny, current_nz);
                    if (!contains_element(current_normal, added_to_current_normal_calculation)) {
                        averaged_normal += current_normal;
                        added_to_current_normal_calculation.push_back(current_normal);
                    }
                }
            }

            if (averaged_normal != glm::vec3(0.0f)) {
                averaged_normal = static_cast<float>(1.00 / added_to_current_normal_calculation.size()) * averaged_normal;
                averaged_normal = glm::normalize(averaged_normal);

                glm::vec3 scaled_direction = 0.2f * averaged_normal;
                glm::vec3 end_point = glm::vec3(start_x, start_y, start_z) + scaled_direction;

                averaged_normal_vertices.push_back(start_x);
                averaged_normal_vertices.push_back(start_y);
                averaged_normal_vertices.push_back(start_z);
                averaged_normal_vertices.push_back(end_point.x);
                averaged_normal_vertices.push_back(end_point.y);
                averaged_normal_vertices.push_back(end_point.z);


            }

        }


        std::ofstream output_file("averaged_normal_vertices.txt");
        if (output_file.is_open()) {
            for (const float& value : averaged_normal_vertices) {
                output_file << value << "\n";
            }
            output_file.close();
        }
        else {
            std::cerr << "Unable to save data to file.\n";
        }
    }

	averaged_normal_lines_vao;
    glGenVertexArrays(1, &averaged_normal_lines_vao);
    glBindVertexArray(averaged_normal_lines_vao);

	averaged_normal_lines_vbo;
    glGenBuffers(1, &averaged_normal_lines_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, averaged_normal_lines_vbo);
    glBufferData(GL_ARRAY_BUFFER, averaged_normal_vertices.size() * sizeof(float), averaged_normal_vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


}
std::vector<float> Mesh::GetVertices() const {
    return mVertices;
}
