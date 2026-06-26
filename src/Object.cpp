#include "Object.h"

Object::Object()
{
    transform = glm::mat4(1.0f);
    localPos = glm::vec3(0.0f);
    localRot = glm::vec3(0.0f);
    localScale = glm::vec3(1.0f);
}

Object::~Object()
{
    for (Model *model : models)
    {
        model->Destroy();
        delete model;
    }

    models.clear();
}

bool Object::AddModels(const char *path)
{

    if (strlen(path) < 4)
        return false;

    const char *ext = path + strlen(path) - 4;

    if (strcmp(ext, "gltf") == 0 || strcmp(ext, ".glb") == 0)
        return Utils::loadGlTf(path, models);

    if (strcmp(ext, ".obj") == 0)
        return Utils::loadOBJ(path, models);

    return false;
}

void Object::Update(float deltaTime)
{
    if(!updateTransform) return;

    transform = glm::mat4(1.0f);
    transform = glm::translate(transform, localPos);
    transform = glm::rotate(transform, localRot.x, {1, 0, 0});
    transform = glm::rotate(transform, localRot.y, {0, 1, 0});
    transform = glm::rotate(transform, localRot.z, {0, 0, 1});
    transform = glm::scale(transform, localScale);

    // call the models update
    for (Model *model : models)
    {
        model->Update(deltaTime, transform);
    }

    updateTransform = false;
}

void Object::Draw()
{
    // call the models draw
    for (Model *model : models)
    {
        model->Draw();
    }
}

void Object::Destroy()
{
    toDelete = true;
}

// Transform Stuff
void Object::Transform(const glm::mat4 &trans)
{
    transform = trans;
}

void Object::SetPosition(const float &x, const float &y, const float &z)
{
    localPos = glm::vec3(x, y, z);
    updateTransform = true;
}

void Object::SetRotation(const float &x, const float &y, const float &z)
{
    localRot = glm::radians(glm::vec3(x, y, z));
    updateTransform = true;
}

void Object::SetScale(const float &x, const float &y, const float &z)
{
    localScale = glm::vec3(x, y, z);
    updateTransform = true;
}

Model *Object::GetModel(int index)
{
    return models[index];
}