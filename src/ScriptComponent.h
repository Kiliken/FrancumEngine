#pragma once

#include <sol/sol.hpp>
#include <iostream>
#include <filesystem>
#include <vector>
#include <string>

#include "Object.h"
#include "Model.h"
#include "Camera.h"

class ScriptComponent
{
public:
    sol::environment env;
    sol::protected_function onStart;
    sol::protected_function onUpdate;
    sol::protected_function onDraw;
    sol::protected_function onGUI;

    ScriptComponent(sol::state &lua, const std::string &scriptPath);
    ~ScriptComponent();

    void Start();
    void Update(float dt);
    void Draw();
    void GUI();

    // Static

    static std::vector<std::string> GetScriptsInFolder(const std::string &folderPath);
    static void BindFunctions(sol::state &lua);
    static void SetupConstants(sol::state &lua);
    static int OnCrash(lua_State* L);
};