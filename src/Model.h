#pragma once

#include <iostream>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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

    GLuint DiffuseTextureID;
    GLuint NormalTextureID;
    GLuint SpecularTextureID;
    GLuint MatrixID;
    GLuint ModelMatrixID;
    GLuint ModelView3x3MatrixID;
    GLuint viewId;
    GLuint light;

    GLuint DiffuseTexture;
    GLuint NormalTexture;
    GLuint SpecularTexture;

    // Transform
    glm::mat4 *projection;
    glm::mat4 mvp;

    glm::mat4 mv;
    glm::mat3 mv33;

    glm::mat4 transform;

    // External
    GLuint *shaders;

    glm::mat4 *view;
    glm::vec3 *lightPos;
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

    DiffuseTextureID = glGetUniformLocation(*shaders, "DiffuseTextureSampler");
    NormalTextureID = glGetUniformLocation(*shaders, "NormalTextureSampler");
    SpecularTextureID = glGetUniformLocation(*shaders, "SpecularTextureSampler");
    MatrixID = glGetUniformLocation(*shaders, "MVP");
    viewId = glGetUniformLocation(*shaders, "V");
    ModelMatrixID = glGetUniformLocation(*shaders, "M");
    ModelView3x3MatrixID = glGetUniformLocation(*shaders, "MV3x3");
    light = glGetUniformLocation(*shaders, "LightDirection_worldspace");

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

    mv = *view * transform;
    mv33 = glm::mat3(mv);
    mvp = *projection * mv;
}

void Model::Draw()
{

    glUseProgram(*shaders);

    glBindVertexArray(vao);

    // This is done in the main loop since each model will have a different MVP matrix (At least for the M part)
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);
    glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &transform[0][0]);
    glUniformMatrix4fv(viewId, 1, GL_FALSE, &((*view)[0][0]));
    glUniformMatrix3fv(ModelView3x3MatrixID, 1, GL_FALSE, &mv33[0][0]);

    glUniform3f(light, (*lightPos).x, (*lightPos).y, (*lightPos).z);

    // Bind our diffuse texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, DiffuseTexture);
    glUniform1i(DiffuseTextureID, 0);

    // Bind our normal texture in Texture Unit 1
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, NormalTexture);
    glUniform1i(NormalTextureID, 1);

    // Bind our specular texture in Texture Unit 2
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, SpecularTexture);
    glUniform1i(SpecularTextureID, 2);

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