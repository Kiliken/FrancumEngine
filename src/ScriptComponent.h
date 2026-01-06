#pragma once

#include <sol/sol.hpp>
#include <iostream>
#include <filesystem>
#include <vector>
#include <string>

#include "Object.h"

class ScriptComponent
{
public:
    sol::environment env;
    sol::function onStart;
    sol::function onUpdate;
    sol::function onDraw;

    ScriptComponent(sol::state &lua, const std::string &scriptPath);
    ~ScriptComponent();

    void Start();
    void Update(float dt);
    void Draw();

};

ScriptComponent::ScriptComponent(sol::state &lua, const std::string &scriptPath)
    : env(lua, sol::create, lua.globals())
{
    lua.script_file(scriptPath, env);

    onStart = env["OnStart"];
    onUpdate = env["OnUpdate"];
    onDraw = env["OnDraw"];
}

ScriptComponent::~ScriptComponent(){

}

void ScriptComponent::Start()
{
    if (onStart.valid())
        onStart();
}

void ScriptComponent::Update(float dt)
{
    if (onUpdate.valid())
        onUpdate(dt);
}

void ScriptComponent::Draw()
{
    if (onDraw.valid())
        onDraw();
}

// Other Stuff


std::vector<std::string> GetScriptsInFolder(const std::string& folderPath) {
    std::vector<std::string> scripts;

    for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
        if (entry.is_regular_file()) {
            auto path = entry.path();
            if (path.extension() == ".lua") {
                scripts.push_back(path.string());
            }
        }
    }

    return scripts;
}


void BindFunctions(sol::state& lua) {
    lua.new_usertype<Object>("Object",
        sol::constructors<
            Object(const char*)
        >(),
        "Update", &Object::Update,
        "Draw", &Object::Draw

    );
}


