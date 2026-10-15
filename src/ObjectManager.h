#pragma once
#include <unordered_map>
#include <memory>
#include <iostream>
#include "Object.h"

class ObjectManager
{
private:
    std::unordered_map<uint32_t, std::unique_ptr<Object>> objects;
    uint32_t idCounter = 1;

public:
    ObjectManager();
    ~ObjectManager();


    // O(1) Lookup
    Object *Get(uint32_t id);

    // Spawn an object and return its light wrapper
    uint32_t CreateObject();

    bool AddModels(uint32_t id, const char *path);
    Model *GetModel(uint32_t id, int index);

    void SetPosition(uint32_t id, float x, float y, float z);
    void SetRotation(uint32_t id, float x, float y, float z);
    void SetScale(uint32_t id, float x, float y, float z);

    void DestroyObject(uint32_t id);
    void Cleanup();
    
    void Update(uint32_t id, float deltaTime);
    void Draw(uint32_t id);
};