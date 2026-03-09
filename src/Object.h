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
#include "Model.h"

class Object
{

public:
    Object();
    ~Object();

    void Update(float deltaTime);
    void Draw();

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

    std::vector<Model> models;
};

Object::Object()
{
    transform = glm::mat4(1.0f);
    localPos = glm::vec3(0.0f);
    localRot = glm::vec3(0.0f);
    localScale = glm::vec3(1.0f);
}

Object::~Object()
{
    for (Model &model : models)
    {
        model.Destroy();
    }

    models.clear();
}

bool Object::AddModels(const char *path)
{

    if (strlen(path) < 4)
        return false;

    const char *ext = path + strlen(path) - 4;

    if (strcmp(ext, "gltf") == 0 || strcmp(ext, ".glb") == 0)
        return loadGlTf(path, models);

    if (strcmp(ext, ".obj") == 0)
        return loadOBJ(path, models);

    return false;
}

void Object::Update(float deltaTime)
{

    transform = glm::mat4(1.0f);
    transform = glm::translate(transform, localPos);
    transform = glm::rotate(transform, localRot.x, {1, 0, 0});
    transform = glm::rotate(transform, localRot.y, {0, 1, 0});
    transform = glm::rotate(transform, localRot.z, {0, 0, 1});
    transform = glm::scale(transform, localScale);

    // call the models update
    for (Model &model : models)
    {
        model.Update(deltaTime, transform);
    }
}

void Object::Draw()
{
    // call the models draw
    for (Model &model : models)
    {
        model.Draw();
    }
}

// Transform Stuff
void Object::Transform(const glm::mat4 &trans)
{
    transform = trans;
}

void Object::SetPosition(const float &x, const float &y, const float &z)
{
    localPos = glm::vec3(x, y, z);
}

void Object::SetRotation(const float &x, const float &y, const float &z)
{
    localRot = glm::radians(glm::vec3(x, y, z));
}

void Object::SetScale(const float &x, const float &y, const float &z)
{
    localScale = glm::vec3(x, y, z);
}

Model *Object::GetModel(int index)
{
    return &models[index];
}