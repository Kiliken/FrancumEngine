#pragma once

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "loadShader.h"
#include "loadDDS.h"
#include "loadOBJ.h"
#include "Inputs.h"
#include "vboIndexer.h"

class Object
{

public:
    Object(const char* fileName, GLuint* prog, glm::mat4* View, glm::mat4& camera, glm::vec3* lightPos);
    ~Object();

    void Update(float deltaTime);
    void Draw();

    void Transform(const glm::mat4& transform);
    void Position(const glm::vec3& position);
    void Rotation(const glm::vec3& rotation);
    void Scale(const glm::vec3& scale);

private:
    // Render
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec3> tangents;
    std::vector<glm::vec3> bitangents;
    std::vector<unsigned short> indices;


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
    glm::mat4 projection;
    glm::mat4 mvp;

    glm::mat4 mv;
    glm::mat3 mv33;

    glm::mat4 Model;
    glm::vec3 localPos, localRot, localScale = {1,1,1};

    // External
    GLuint* shaders;


    glm::mat4* view;
    glm::vec3* lightPos;


};

Object::Object(const char* fileName, GLuint* prog, glm::mat4* View, glm::mat4& camera, glm::vec3* LightPos)
{
    std::vector<glm::vec3> tempVertices;
    std::vector<glm::vec2> tempUvs;
    std::vector<glm::vec3> tempNormals;
    std::vector<glm::vec3> tempTangents;
    std::vector<glm::vec3> tempBitangents; 

    loadOBJ(fileName, tempVertices, tempUvs, tempNormals);
    computeTangentBasis(tempVertices, tempUvs, tempNormals, tempTangents, tempBitangents);
    indexVBO_TBN(tempVertices,tempUvs,tempNormals,tempTangents, tempBitangents,indices, vertices,uvs,normals, tangents, bitangents);

    shaders = prog;
    projection = camera;
    view = View;
    lightPos = LightPos;

    glGenBuffers(1, &elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &uvsbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvsbuffer);
    glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec3), &uvs[0], GL_STATIC_DRAW);

    glGenBuffers(1, &normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);

    glGenBuffers(1, &tangentbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, tangentbuffer);
    glBufferData(GL_ARRAY_BUFFER, tangents.size() * sizeof(glm::vec3), &tangents[0], GL_STATIC_DRAW);

    glGenBuffers(1, &bitangentbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, bitangentbuffer);
    glBufferData(GL_ARRAY_BUFFER, bitangents.size() * sizeof(glm::vec3), &bitangents[0], GL_STATIC_DRAW);

    DiffuseTexture = loadDDS("../res/diffuse.dds");
    NormalTexture = loadDDS("../res/normals.dds");
    SpecularTexture = loadDDS("../res/specular.dds");

    DiffuseTextureID = glGetUniformLocation(*shaders, "DiffuseTextureSampler");
    NormalTextureID = glGetUniformLocation(*shaders, "NormalTextureSampler");
    SpecularTextureID = glGetUniformLocation(*shaders, "SpecularTextureSampler");
    MatrixID = glGetUniformLocation(*shaders, "MVP");
    viewId = glGetUniformLocation(*shaders, "V");
    ModelMatrixID = glGetUniformLocation(*shaders, "M");
    ModelView3x3MatrixID = glGetUniformLocation(*shaders, "MV3x3");
    light = glGetUniformLocation(*shaders, "LightPosition_worldspace");

    Model = glm::mat4(1.0f);
}

void Object::Update(float deltaTime)
{

    Model = glm::mat4(1.0f);
    Model = glm::translate(Model, localPos);
    Model = glm::rotate(Model, localRot.x, {1,0,0});
    Model = glm::rotate(Model, localRot.y, {0,1,0});
    Model = glm::rotate(Model, localRot.z, {0,0,1});
    Model = glm::scale(Model, localScale);


    mv = *view * Model;
    mv33 = glm::mat3(mv);
    mvp = projection * *view * Model;
}

void Object::Draw()
{
    glUseProgram(*shaders);

    
    // 1st attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(
        0,        // attribute 0. No particular reason for 0, but must match the layout in the shader.
        3,        // size
        GL_FLOAT, // type
        GL_FALSE, // normalized?
        0,        // stride
        (void *)0 // array buffer offset
    );

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, uvsbuffer);
    glVertexAttribPointer(
        1,        // attribute. No particular reason for 1, but must match the layout in the shader.
        2,        // size
        GL_FLOAT, // type
        GL_FALSE, // normalized?
        0,        // stride
        (void *)0 // array buffer offset
    );

    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glVertexAttribPointer(
        2,        // attribute
        3,        // size
        GL_FLOAT, // type
        GL_FALSE, // normalized?
        0,        // stride
        (void *)0 // array buffer offset
    );

    glEnableVertexAttribArray(3);
    glBindBuffer(GL_ARRAY_BUFFER, tangentbuffer);
    glVertexAttribPointer(
        3,        // attribute
        3,        // size
        GL_FLOAT, // type
        GL_FALSE, // normalized?
        0,        // stride
        (void *)0 // array buffer offset
    );

    // 5th attribute buffer : bitangents
    glEnableVertexAttribArray(4);
    glBindBuffer(GL_ARRAY_BUFFER, bitangentbuffer);
    glVertexAttribPointer(
        4,        // attribute
        3,        // size
        GL_FLOAT, // type
        GL_FALSE, // normalized?
        0,        // stride
        (void *)0 // array buffer offset
    );


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
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glDrawElements(
        GL_TRIANGLES,      // mode
        indices.size(),    // count
        GL_UNSIGNED_SHORT, // type
        (void *)0          // element array buffer offset
    );

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
    glDisableVertexAttribArray(4);
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
    glDeleteTextures(1, &DiffuseTexture);
    glDeleteTextures(1, &NormalTexture);
    glDeleteTextures(1, &SpecularTexture);
}

// Transform Stuff
void Object::Transform(const glm::mat4& transform){
    Model = transform;
}

void Object::Position(const glm::vec3& position){
    localPos = position;
}

void Object::Rotation(const glm::vec3& rotation){
    localRot = glm::radians(rotation);
}

void Object::Scale(const glm::vec3& scale){
    localScale = scale;
}