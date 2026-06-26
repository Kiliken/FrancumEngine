#pragma once

#include <iostream>
#include <glad/gl.h>
#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "utils/loadShader.h"
#include "utils/loadDDS.h"
#include "utils/loadOBJ.h"
#include "Inputs.h"
#include "utils/vboIndexer.h"
#include "Model.h"

class Object
{

public:
    Object();
    ~Object();

public:

    bool toDelete = false;

    void Update(float deltaTime);
    void Draw();
    void Destroy();

    void Transform(const glm::mat4 &transform);
    void SetPosition(const float &x, const float &y, const float &z);
    void SetRotation(const float &x, const float &y, const float &z);
    void SetScale(const float &x, const float &y, const float &z);

    bool AddModels(const char *path);

    Model *GetModel(int index);


private:
    glm::mat4 transform;
    glm::vec3 localPos;
    glm::vec3 localRot;
    glm::vec3 localScale;
    bool updateTransform = false;

    std::vector<Model*> models;
};