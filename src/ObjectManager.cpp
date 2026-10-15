#include "ObjectManager.h"

ObjectManager::ObjectManager()
{
}

ObjectManager::~ObjectManager()
{
    
}

// O(1) Lookup
Object *ObjectManager::Get(uint32_t id)
{
    auto it = objects.find(id);
    return (it != objects.end()) ? it->second.get() : nullptr;
}

// Spawn an object and return its light wrapper
uint32_t ObjectManager::CreateObject()
{
    uint32_t id = idCounter++;
    auto obj = std::make_unique<Object>();
    obj->id = id;
    objects[id] = std::move(obj);
    return id;
}

bool ObjectManager::AddModels(uint32_t id, const char *path)
{
    if (Object *obj = Get(id))
    {
        return obj->AddModels(path);
    }
    return false;
}

Model *ObjectManager::GetModel(uint32_t id, int index)
{
    if (Object *obj = Get(id))
    {
        return obj->GetModel(index);
    }
    return nullptr;
}

void ObjectManager::SetPosition(uint32_t id, float x, float y, float z)
{
    if (Object *obj = Get(id))
    {
        obj->SetPosition(x, y, z);
    }
    else
    {
        std::cerr << "[ObjectManager Warning] Object ID " << id << " not found!\n";
    }
}

void ObjectManager::SetRotation(uint32_t id, float x, float y, float z)
{
    if (Object *obj = Get(id))
    {
        obj->SetRotation(x, y, z);
    }
}

void ObjectManager::SetScale(uint32_t id, float x, float y, float z)
{
    if (Object *obj = Get(id))
    {
        obj->SetScale(x, y, z);
    }
}

void ObjectManager::DestroyObject(uint32_t id)
{
    if (Object *obj = Get(id))
    {
        obj->Destroy();
    }
}

void ObjectManager::Update(uint32_t id, float deltaTime)
{
    if (Object *obj = Get(id))
    {
        if (!obj->toDelete)
        {
            obj->Update(deltaTime);
        }
    }
}

void ObjectManager::Cleanup()
{
    auto it = objects.begin();
    while (it != objects.end())
    {
        Object *obj = it->second.get();

        if (obj->toDelete)
        {
            it = objects.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void ObjectManager::Draw(uint32_t id)
{
    if (Object *obj = Get(id))
    {
        if (!obj->toDelete)
        {
            obj->Draw();
        }
    }
}
