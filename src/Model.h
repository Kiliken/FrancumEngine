#pragma once

#include <iostream>
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "loadShader.h"
#include "loadDDS.h"
#include "Inputs.h"
#include "vboIndexer.h"

#define CUBE_MODEL ("CUBE")

extern const unsigned char _binary_cube_obj_start[];
extern const unsigned char _binary_cube_obj_end[];

size_t cubeSize = _binary_cube_obj_end - _binary_cube_obj_start;

std::string cubeData(
    (const char *)_binary_cube_obj_start,
    cubeSize);

struct ModelConfig
{
    const char *fileName;
    GLuint *prog;
    glm::mat4 *View;
    glm::mat4 *camera;
    glm::vec3 *lightPos;
};

ModelConfig DefaultModelConfig;

struct Vertex
{
    glm::vec3 position;
    glm::vec2 uv;
    glm::vec3 normal;
    glm::vec3 tangent;
    glm::vec3 bitangent;
};

struct CameraUBO
{
    glm::mat4 M;
    glm::mat4 V;
    glm::mat4 P;
};

struct MaterialIDs
{
    int diffuse;
    int normal;
    int specular;
    int padding;
};

class Model
{

public:
    Model(const std::vector<glm::vec3> &inPositions, const std::vector<glm::vec2> &inUvs, const std::vector<glm::vec3> &inNormals, const std::vector<unsigned int> inIndices, GLuint *prog, glm::mat4 *View, glm::mat4 *camera, glm::vec3 *lightPos);
    Model(const std::vector<glm::vec3> &inPositions, const std::vector<glm::vec2> &inUvs, const std::vector<glm::vec3> &inNormals, const std::vector<unsigned int> inIndices);
    Model(const std::vector<glm::vec3> &inPositions, const std::vector<glm::vec2> &inUvs, const std::vector<glm::vec3> &inNormals);
    ~Model();

    void Update(float deltaTime, const glm::mat4 &trans);
    void Draw();
    void Destroy();

    void SetTexture(const char *path);
    void SetNormalMap(const char *path);
    void SetSpecularMap(const char *path);

private:
    // Render

    std::vector<Vertex> vertices;

    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec3> tangents;
    std::vector<glm::vec3> bitangents;
    std::vector<unsigned int> indices;

    GLuint vao;
    GLuint vbo;
    GLuint elementbuffer;
    GLuint vertexbuffer;
    GLuint uvsbuffer;
    GLuint normalbuffer;
    GLuint tangentbuffer;
    GLuint bitangentbuffer;

    GLuint MatrixID;
    GLuint ModelMatrixID;
    GLuint ModelViewMatrixID;
    GLuint viewId;
    GLuint light;

    // Material
    GLuint TexturesID;
    MaterialIDs mat;
    GLuint MaterialUBO;

    GLuint DiffuseTexture;
    GLuint NormalTexture;
    GLuint SpecularTexture;

    // Transform
    GLuint CamUBOID;
    CameraUBO cam;

    glm::mat4 *projection;
    glm::mat4 transform;
    glm::mat4 *view;
    glm::vec3 *lightPos;

    // External
    GLuint *shaders;
};

Model::Model(const std::vector<glm::vec3> &inPositions, const std::vector<glm::vec2> &inUvs, const std::vector<glm::vec3> &inNormals, const std::vector<unsigned int> inIndices, GLuint *prog, glm::mat4 *View, glm::mat4 *camera, glm::vec3 *lightPos)
    : shaders(prog), view(View), projection(camera), lightPos(lightPos)
{
    std::vector<glm::vec3> tempPositions = inPositions;
    std::vector<glm::vec2> tempUvs = inUvs;
    std::vector<glm::vec3> tempNormals = inNormals;
    std::vector<glm::vec3> tempTangents;
    std::vector<glm::vec3> tempBitangents;

    if (inIndices.empty())
    {
        computeTangentBasis(tempPositions, tempUvs, tempNormals, tempTangents, tempBitangents);
        indexVBO_TBN(tempPositions, tempUvs, tempNormals, tempTangents, tempBitangents, indices, positions, uvs, normals, tangents, bitangents);
    }
    else
    {

        indices = inIndices;

        positions = tempPositions;
        uvs = tempUvs;
        normals = tempNormals;

        computeTangentsIndexed(positions, uvs, normals, indices, tangents, bitangents);
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

    DiffuseTexture = loadDDS("../res/baseDiffuse.dds");
    NormalTexture = loadDDS("../res/baseNormals.dds");
    SpecularTexture = loadDDS("../res/baseSpecular.dds");

    mat.diffuse = 0;
    mat.normal = 1;
    mat.specular = 2;

    glGenBuffers(1, &MaterialUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, MaterialUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(MaterialIDs), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, MaterialUBO);

    glGenBuffers(1, &CamUBOID);
    glBindBuffer(GL_UNIFORM_BUFFER, CamUBOID);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(CameraUBO), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 2, CamUBOID);

    //MatrixID = glGetUniformLocation(*shaders, "P");
    //viewId = glGetUniformLocation(*shaders, "V");
    ModelMatrixID = glGetUniformLocation(*shaders, "M");

    light = glGetUniformLocation(*shaders, "LightDirection_worldspace");

    TexturesID = glGetUniformLocation(*shaders, "textures");

    transform = glm::mat4(1.0f);
}

Model::Model(const std::vector<glm::vec3> &inPositions, const std::vector<glm::vec2> &inUvs, const std::vector<glm::vec3> &inNormals, const std::vector<unsigned int> inIndices)
    : Model(inPositions, inUvs, inNormals, inIndices, DefaultModelConfig.prog, DefaultModelConfig.View, DefaultModelConfig.camera, DefaultModelConfig.lightPos)
{
}

Model::Model(const std::vector<glm::vec3> &inPositions, const std::vector<glm::vec2> &inUvs, const std::vector<glm::vec3> &inNormals)
    : Model(inPositions, inUvs, inNormals, std::vector<unsigned int>{}, DefaultModelConfig.prog, DefaultModelConfig.View, DefaultModelConfig.camera, DefaultModelConfig.lightPos)
{
}

void Model::Update(float deltaTime, const glm::mat4 &trans)
{

    transform = trans;

    {
        cam.P = *projection;
        cam.V = *view;
        cam.M = transform;
    }
}

void Model::Draw()
{

    glUseProgram(*shaders);

    glBindVertexArray(vao);

    // This is done in the main loop since each model will have a different MVP matrix (At least for the M part)
    // glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &cam.P[0][0]);
    // glUniformMatrix4fv(viewId, 1, GL_FALSE, &cam.V[0][0]);
    glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &cam.M[0][0]);

    glUniform3f(light, (*lightPos).x, (*lightPos).y, (*lightPos).z);

    glBindBuffer(GL_UNIFORM_BUFFER, CamUBOID);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(CameraUBO), &cam);

    glBindBuffer(GL_UNIFORM_BUFFER, MaterialUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(MaterialIDs), &mat);

    glUniform1i(TexturesID, 0);

    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, DiffuseTexture);
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, NormalTexture);
    glActiveTexture(GL_TEXTURE0 + 2);
    glBindTexture(GL_TEXTURE_2D, SpecularTexture);

    // Draw the triangles !
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
    glDeleteBuffers(1, &CamUBOID);
    glDeleteTextures(1, &DiffuseTexture);
    glDeleteTextures(1, &NormalTexture);
    glDeleteTextures(1, &SpecularTexture);
}

void Model::SetTexture(const char *path)
{
    DiffuseTexture = loadDDS(path);
}

void Model::SetNormalMap(const char *path)
{
    NormalTexture = loadDDS(path);
}

void Model::SetSpecularMap(const char *path)
{
    SpecularTexture = loadDDS(path);
}