#pragma once

#include <iostream>
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "utils/loadShader.h"
#include "utils/loadDDS.h"
#include "Inputs.h"
#include "utils/vboIndexer.h"

#define CUBE_MODEL ("CUBE")
/*
extern const unsigned char _binary_cube_obj_start[];
extern const unsigned char _binary_cube_obj_end[];

extern size_t cubeSize;
size_t cubeSize = _binary_cube_obj_end - _binary_cube_obj_start;

std::string cubeData(
    (const char *)_binary_cube_obj_start,
    cubeSize);
*/

struct ModelConfig
{
    const char *fileName;
    GLuint *prog;
    glm::mat4 *View;
    glm::mat4 *camera;
    glm::vec3 *lightPos;
};

extern ModelConfig DefaultModelConfig;


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