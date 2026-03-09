#pragma once

#include <sol/sol.hpp>
#include <iostream>
#include <filesystem>
#include <vector>
#include <string>

#include "Object.h"
#include "Model.h"

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

ScriptComponent::~ScriptComponent()
{
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

std::vector<std::string> GetScriptsInFolder(const std::string &folderPath)
{
    std::vector<std::string> scripts;

    for (const auto &entry : std::filesystem::directory_iterator(folderPath))
    {
        if (entry.is_regular_file())
        {
            auto path = entry.path();
            if (path.extension() == ".lua")
            {
                scripts.push_back(path.string());
            }
        }
    }

    return scripts;
}

void BindFunctions(sol::state &lua)
{
    lua.new_usertype<Object>("Object",
                             sol::constructors<
                                 Object()>(),
                             "Update", &Object::Update,
                             "Draw", &Object::Draw,
                             "AddModels", &Object::AddModels,
                             "GetModel", &Object::GetModel,
                             "SetPosition", &Object::SetPosition,
                             "SetRotation", &Object::SetRotation,
                             "SetScale", &Object::SetScale);

    lua.new_usertype<Model>("Model",
                             "SetTexture", &Model::SetTexture,
                             "SetNormalMap", &Model::SetNormalMap,
                             "SetSpecularMap", &Model::SetSpecularMap);

    lua.new_usertype<Inputs>("Inputs", "IsKeyDown", &Inputs::IsKeyDown);
}

/*

*/

void SetupConstants(sol::state &lua)
{
    /*Printablekeys*/
    lua["INPUT_KEY_SPACE"] = 32;
    lua["INPUT_KEY_APOSTROPHE"] = 39; /*'*/
    lua["INPUT_KEY_COMMA"] = 44;      /*,*/
    lua["INPUT_KEY_MINUS"] = 45;      /*-*/
    lua["INPUT_KEY_PERIOD"] = 46;     /*.*/
    lua["INPUT_KEY_SLASH"] = 47;      /*/*/
    lua["INPUT_KEY_0"] = 48;
    lua["INPUT_KEY_1"] = 49;
    lua["INPUT_KEY_2"] = 50;
    lua["INPUT_KEY_3"] = 51;
    lua["INPUT_KEY_4"] = 52;
    lua["INPUT_KEY_5"] = 53;
    lua["INPUT_KEY_6"] = 54;
    lua["INPUT_KEY_7"] = 55;
    lua["INPUT_KEY_8"] = 56;
    lua["INPUT_KEY_9"] = 57;
    lua["INPUT_KEY_SEMICOLON"] = 59; /*;*/
    lua["INPUT_KEY_EQUAL"] = 61;     /*=*/
    lua["INPUT_KEY_A"] = 65;
    lua["INPUT_KEY_B"] = 66;
    lua["INPUT_KEY_C"] = 67;
    lua["INPUT_KEY_D"] = 68;
    lua["INPUT_KEY_E"] = 69;
    lua["INPUT_KEY_F"] = 70;
    lua["INPUT_KEY_G"] = 71;
    lua["INPUT_KEY_H"] = 72;
    lua["INPUT_KEY_I"] = 73;
    lua["INPUT_KEY_J"] = 74;
    lua["INPUT_KEY_K"] = 75;
    lua["INPUT_KEY_L"] = 76;
    lua["INPUT_KEY_M"] = 77;
    lua["INPUT_KEY_N"] = 78;
    lua["INPUT_KEY_O"] = 79;
    lua["INPUT_KEY_P"] = 80;
    lua["INPUT_KEY_Q"] = 81;
    lua["INPUT_KEY_R"] = 82;
    lua["INPUT_KEY_S"] = 83;
    lua["INPUT_KEY_T"] = 84;
    lua["INPUT_KEY_U"] = 85;
    lua["INPUT_KEY_V"] = 86;
    lua["INPUT_KEY_W"] = 87;
    lua["INPUT_KEY_X"] = 88;
    lua["INPUT_KEY_Y"] = 89;
    lua["INPUT_KEY_Z"] = 90;
    lua["INPUT_KEY_LEFT_BRACKET"] = 91;  /*[*/
    lua["INPUT_KEY_BACKSLASH"] = 92;     /*\*/
    lua["INPUT_KEY_RIGHT_BRACKET"] = 93; /*]*/
    lua["INPUT_KEY_GRAVE_ACCENT"] = 96;  /*`*/
    lua["INPUT_KEY_WORLD_1"] = 161;      /*non-US#1*/
    lua["INPUT_KEY_WORLD_2"] = 162;      /*non-US#2*/

    /*Functionkeys*/
    lua["INPUT_KEY_ESCAPE"] = 256;
    lua["INPUT_KEY_ENTER"] = 257;
    lua["INPUT_KEY_TAB"] = 258;
    lua["INPUT_KEY_BACKSPACE"] = 259;
    lua["INPUT_KEY_INSERT"] = 260;
    lua["INPUT_KEY_DELETE"] = 261;
    lua["INPUT_KEY_RIGHT"] = 262;
    lua["INPUT_KEY_LEFT"] = 263;
    lua["INPUT_KEY_DOWN"] = 264;
    lua["INPUT_KEY_UP"] = 265;
    lua["INPUT_KEY_PAGE_UP"] = 266;
    lua["INPUT_KEY_PAGE_DOWN"] = 267;
    lua["INPUT_KEY_HOME"] = 268;
    lua["INPUT_KEY_END"] = 269;
    lua["INPUT_KEY_CAPS_LOCK"] = 280;
    lua["INPUT_KEY_SCROLL_LOCK"] = 281;
    lua["INPUT_KEY_NUM_LOCK"] = 282;
    lua["INPUT_KEY_PRINT_SCREEN"] = 283;
    lua["INPUT_KEY_PAUSE"] = 284;
    lua["INPUT_KEY_F1"] = 290;
    lua["INPUT_KEY_F2"] = 291;
    lua["INPUT_KEY_F3"] = 292;
    lua["INPUT_KEY_F4"] = 293;
    lua["INPUT_KEY_F5"] = 294;
    lua["INPUT_KEY_F6"] = 295;
    lua["INPUT_KEY_F7"] = 296;
    lua["INPUT_KEY_F8"] = 297;
    lua["INPUT_KEY_F9"] = 298;
    lua["INPUT_KEY_F10"] = 299;
    lua["INPUT_KEY_F11"] = 300;
    lua["INPUT_KEY_F12"] = 301;
    lua["INPUT_KEY_F13"] = 302;
    lua["INPUT_KEY_F14"] = 303;
    lua["INPUT_KEY_F15"] = 304;
    lua["INPUT_KEY_F16"] = 305;
    lua["INPUT_KEY_F17"] = 306;
    lua["INPUT_KEY_F18"] = 307;
    lua["INPUT_KEY_F19"] = 308;
    lua["INPUT_KEY_F20"] = 309;
    lua["INPUT_KEY_F21"] = 310;
    lua["INPUT_KEY_F22"] = 311;
    lua["INPUT_KEY_F23"] = 312;
    lua["INPUT_KEY_F24"] = 313;
    lua["INPUT_KEY_F25"] = 314;
    lua["INPUT_KEY_KP_0"] = 320;
    lua["INPUT_KEY_KP_1"] = 321;
    lua["INPUT_KEY_KP_2"] = 322;
    lua["INPUT_KEY_KP_3"] = 323;
    lua["INPUT_KEY_KP_4"] = 324;
    lua["INPUT_KEY_KP_5"] = 325;
    lua["INPUT_KEY_KP_6"] = 326;
    lua["INPUT_KEY_KP_7"] = 327;
    lua["INPUT_KEY_KP_8"] = 328;
    lua["INPUT_KEY_KP_9"] = 329;
    lua["INPUT_KEY_KP_DECIMAL"] = 330;
    lua["INPUT_KEY_KP_DIVIDE"] = 331;
    lua["INPUT_KEY_KP_MULTIPLY"] = 332;
    lua["INPUT_KEY_KP_SUBTRACT"] = 333;
    lua["INPUT_KEY_KP_ADD"] = 334;
    lua["INPUT_KEY_KP_ENTER"] = 335;
    lua["INPUT_KEY_KP_EQUAL"] = 336;
    lua["INPUT_KEY_LEFT_SHIFT"] = 340;
    lua["INPUT_KEY_LEFT_CONTROL"] = 341;
    lua["INPUT_KEY_LEFT_ALT"] = 342;
    lua["INPUT_KEY_LEFT_SUPER"] = 343;
    lua["INPUT_KEY_RIGHT_SHIFT"] = 344;
    lua["INPUT_KEY_RIGHT_CONTROL"] = 345;
    lua["INPUT_KEY_RIGHT_ALT"] = 346;
    lua["INPUT_KEY_RIGHT_SUPER"] = 347;
    lua["INPUT_KEY_MENU"] = 348;
}