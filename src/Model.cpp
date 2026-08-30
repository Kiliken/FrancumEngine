#include "Model.h"

#include "configs.h"

Model::Model(const std::vector<glm::vec3> &inPositions, const std::vector<glm::vec2> &inUvs, const std::vector<glm::vec3> &inNormals, const std::vector<unsigned int> inIndices, GLuint *prog)
    : shaders(prog)
{
    std::vector<glm::vec3> tempPositions = inPositions;
    std::vector<glm::vec2> tempUvs = inUvs;
    std::vector<glm::vec3> tempNormals = inNormals;
    std::vector<glm::vec3> tempTangents;
    std::vector<glm::vec3> tempBitangents;

    if (inIndices.empty())
    {
        Utils::computeTangentBasis(tempPositions, tempUvs, tempNormals, tempTangents, tempBitangents);
        Utils::indexVBO_TBN(tempPositions, tempUvs, tempNormals, tempTangents, tempBitangents, indices, positions, uvs, normals, tangents, bitangents);
    }
    else
    {

        indices = inIndices;

        positions = tempPositions;
        uvs = tempUvs;
        normals = tempNormals;

        Utils::computeTangentsIndexed(positions, uvs, normals, indices, tangents, bitangents);
    }

    std::vector<Vertex> vertices;

    for (size_t i = 0; i < positions.size(); i++)
    {
        Vertex v;
        v.position = positions[i];
        v.uv = uvs[i];
        v.normal = normals[i];
        v.tangent = tangents[i];
        v.bitangent = bitangents[i];
        vertices.push_back(v);
    }

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0); // position
    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE,
        sizeof(Vertex),
        (void *)offsetof(Vertex, position));

    glEnableVertexAttribArray(1); // uv
    glVertexAttribPointer(
        1, 2, GL_FLOAT, GL_FALSE,
        sizeof(Vertex),
        (void *)offsetof(Vertex, uv));

    glEnableVertexAttribArray(2); // normal
    glVertexAttribPointer(
        2, 3, GL_FLOAT, GL_FALSE,
        sizeof(Vertex),
        (void *)offsetof(Vertex, normal));

    glEnableVertexAttribArray(3); // tangent
    glVertexAttribPointer(
        3, 3, GL_FLOAT, GL_FALSE,
        sizeof(Vertex),
        (void *)offsetof(Vertex, tangent));

    glEnableVertexAttribArray(4); // bitangent
    glVertexAttribPointer(
        4, 3, GL_FLOAT, GL_FALSE,
        sizeof(Vertex),
        (void *)offsetof(Vertex, bitangent));

    glGenBuffers(1, &elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    DiffuseTexture = Utils::loadDDS(ASSETS("baseDiffuse.dds"));
    NormalTexture = Utils::loadDDS(ASSETS("baseNormals.dds"));
    SpecularTexture = Utils::loadDDS(ASSETS("baseSpecular.dds"));

    mat.diffuse = 0;
    mat.normal = 1;
    mat.specular = 2;

    glGenBuffers(1, &MaterialUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, MaterialUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(MaterialIDs), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, MaterialUBO);

    //MatrixID = glGetUniformLocation(*shaders, "P");
    //viewId = glGetUniformLocation(*shaders, "V");
    ModelMatrixID = glGetUniformLocation(*shaders, "M");
    ColorID = glGetUniformLocation(*shaders, "AlbedoColor");

    TexturesID = glGetUniformLocation(*shaders, "textures");

    transform = glm::mat4(1.0f);
    color = glm::vec3(1.0f);
}

Model::Model(const std::vector<glm::vec3> &inPositions, const std::vector<glm::vec2> &inUvs, const std::vector<glm::vec3> &inNormals, const std::vector<unsigned int> inIndices)
    : Model(inPositions, inUvs, inNormals, inIndices, DefaultModelConfig.prog)
{
}

Model::Model(const std::vector<glm::vec3> &inPositions, const std::vector<glm::vec2> &inUvs, const std::vector<glm::vec3> &inNormals)
    : Model(inPositions, inUvs, inNormals, std::vector<unsigned int>{}, DefaultModelConfig.prog)
{
}

void Model::Update(float deltaTime, const glm::mat4 &trans)
{

    transform = trans;
}

void Model::Draw()
{

    glUseProgram(*shaders);

    glBindVertexArray(vao);

    glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &transform[0][0]);
    glUniform3f(ColorID, color.x, color.y, color.z);

    glBindBuffer(GL_UNIFORM_BUFFER, MaterialUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(MaterialIDs), &mat);

    glUniform1i(TexturesID, 0);

    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, DiffuseTexture);
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, NormalTexture);
    glActiveTexture(GL_TEXTURE0 + 2);
    glBindTexture(GL_TEXTURE_2D, SpecularTexture);

    // Draw the triangles
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
}

Model::~Model()
{
}

void Model::Destroy()
{
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &uvsbuffer);
    glDeleteBuffers(1, &normalbuffer);
    glDeleteBuffers(1, &uvsbuffer);
    glDeleteBuffers(1, &normalbuffer);
    glDeleteBuffers(1, &tangentbuffer);
    glDeleteBuffers(1, &bitangentbuffer);
    glDeleteBuffers(1, &elementbuffer);
    glDeleteBuffers(1, &vao);
    glDeleteBuffers(1, &MaterialUBO);
    glDeleteTextures(1, &DiffuseTexture);
    glDeleteTextures(1, &NormalTexture);
    glDeleteTextures(1, &SpecularTexture);
}

void Model::SetTexture(const char *path)
{
    char fullPath[512];
    snprintf(fullPath, sizeof(fullPath), ASSETS("%s"), path);

    DiffuseTexture = Utils::loadDDS(fullPath);
}

void Model::SetNormalMap(const char *path)
{
    char fullPath[512];
    snprintf(fullPath, sizeof(fullPath), ASSETS("%s"), path);

    NormalTexture = Utils::loadDDS(fullPath);
}

void Model::SetSpecularMap(const char *path)
{
    char fullPath[512];
    snprintf(fullPath, sizeof(fullPath), ASSETS("%s"), path);

    SpecularTexture = Utils::loadDDS(fullPath);
}

void Model::SetColor(const char *colorHex){
    color = Utils::hexToVec3(colorHex);
}