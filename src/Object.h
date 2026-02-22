#pragma once

#include <iostream>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "loadShader.h"
#include "loadDDS.h"
#include "loadOBJ.h"
#include "Inputs.h"
#include "vboIndexer.h"

#define CUBE_MODEL ("CUBE")

extern const unsigned char _binary_cube_obj_start[];
extern const unsigned char _binary_cube_obj_end[];

size_t cubeSize = _binary_cube_obj_end - _binary_cube_obj_start;

std::string cubeData(
    (const char *)_binary_cube_obj_start,
    cubeSize);

struct ObjectConfig
{
    const char *fileName;
    GLuint *prog;
    glm::mat4 *View;
    glm::mat4 *camera;
    glm::vec3 *lightPos;
};

ObjectConfig DefaultObjectConfig;

class Object
{

public:
    Object(const char *fileName, GLuint *prog, glm::mat4 *View, glm::mat4 *camera, glm::vec3 *lightPos);
    Object(const ObjectConfig &cfg);
    Object(const char *fileName);
    Object();

    ~Object();

    void Update(float deltaTime);
    void Draw();

    void Transform(const glm::mat4 &transform);
    void SetPosition(const float& x, const float& y, const float& z);
    void SetRotation(const float& x, const float& y, const float& z);
    void SetScale(const float& x, const float& y, const float& z);
    
    void SetTexture(const char *path);
    void SetNormalMap(const char *path);
    void SetSpecularMap(const char *path);


private:
    // Render
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec3> tangents;
    std::vector<glm::vec3> bitangents;
    std::vector<unsigned short> indices;

    GLuint vao;
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

    glm::mat4 Model;
    glm::vec3 localPos;
    glm::vec3 localRot;
    glm::vec3 localScale;

    // External
    GLuint *shaders;

    glm::mat4 *view;
    glm::vec3 *lightPos;
};

Object::Object(const char *fileName, GLuint *prog, glm::mat4 *View, glm::mat4 *camera, glm::vec3 *lightPos)
    : shaders(prog), view(View), projection(camera), lightPos(lightPos)
{

    std::vector<glm::vec3> tempVertices;
    std::vector<glm::vec2> tempUvs;
    std::vector<glm::vec3> tempNormals;
    std::vector<glm::vec3> tempTangents;
    std::vector<glm::vec3> tempBitangents;

    if (fileName == CUBE_MODEL)
    {
        loadOBJ(cubeData, tempVertices, tempUvs, tempNormals);
    }
    else
        loadOBJ(fileName, tempVertices, tempUvs, tempNormals);

    computeTangentBasis(tempVertices, tempUvs, tempNormals, tempTangents, tempBitangents);
    indexVBO_TBN(tempVertices, tempUvs, tempNormals, tempTangents, tempBitangents, indices, vertices, uvs, normals, tangents, bitangents);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &uvsbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvsbuffer);
    glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(2);

    glGenBuffers(1, &tangentbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, tangentbuffer);
    glBufferData(GL_ARRAY_BUFFER, tangents.size() * sizeof(glm::vec3), &tangents[0], GL_STATIC_DRAW);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(3);

    glGenBuffers(1, &bitangentbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, bitangentbuffer);
    glBufferData(GL_ARRAY_BUFFER, bitangents.size() * sizeof(glm::vec3), &bitangents[0], GL_STATIC_DRAW);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(4);

    glGenBuffers(1, &elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);

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

    Model = glm::mat4(1.0f);
    localPos = glm::vec3(0.0f);
    localRot = glm::vec3(0.0f);
    localScale = glm::vec3(1.0f);
}

Object::Object(const ObjectConfig &cfg)
    : Object(cfg.fileName, cfg.prog, cfg.View, cfg.camera, cfg.lightPos) {}

Object::Object(const char *fileName)
    : Object(fileName, DefaultObjectConfig.prog, DefaultObjectConfig.View, DefaultObjectConfig.camera, DefaultObjectConfig.lightPos) {}

Object::Object()
    : Object(DefaultObjectConfig) {}






void Object::Update(float deltaTime)
{

    Model = glm::mat4(1.0f);
    Model = glm::translate(Model, localPos);
    Model = glm::rotate(Model, localRot.x, {1, 0, 0});
    Model = glm::rotate(Model, localRot.y, {0, 1, 0});
    Model = glm::rotate(Model, localRot.z, {0, 0, 1});
    Model = glm::scale(Model, localScale);

    mv = *view * Model;
    mv33 = glm::mat3(mv);
    mvp = *projection * *view * Model;
}

void Object::Draw()
{
    glUseProgram(*shaders);

    glBindVertexArray(vao);

    // This is done in the main loop since each model will have a different MVP matrix (At least for the M part)
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);
    glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &Model[0][0]);
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
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, 0);

    glBindVertexArray(0);
}

Object::~Object()
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

// Transform Stuff
void Object::Transform(const glm::mat4 &transform)
{
    Model = transform;
}

void Object::SetPosition(const float& x, const float& y, const float& z)
{
    localPos = glm::vec3(x,y,z);
}

void Object::SetRotation(const float& x, const float& y, const float& z)
{
    localRot = glm::radians(glm::vec3(x,y,z));
}

void Object::SetScale(const float& x, const float& y, const float& z)
{
    localScale = glm::vec3(x,y,z);
}

void Object::SetTexture(const char *path)
{
    DiffuseTexture = loadDDS(path);
}

void Object::SetNormalMap(const char *path)
{
    NormalTexture = loadDDS(path);
}

void Object::SetSpecularMap(const char *path)
{
    SpecularTexture = loadDDS(path);
}