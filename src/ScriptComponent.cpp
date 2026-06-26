#include "ScriptComponent.h"

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

std::vector<std::string> ScriptComponent::GetScriptsInFolder(const std::string &folderPath)
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

void ScriptComponent::BindFunctions(sol::state &lua)
{
    lua.new_usertype<Object>("Object",
                             "Update", &Object::Update,
                             "Draw", &Object::Draw,
                             "Destroy", &Object::Destroy,
                             "AddModels", &Object::AddModels,
                             "GetModel", &Object::GetModel,
                             "SetPosition", &Object::SetPosition,
                             "SetRotation", &Object::SetRotation,
                             "SetScale", &Object::SetScale);

    lua.new_usertype<Model>("Model",
                             "SetTexture", &Model::SetTexture,
                             "SetNormalMap", &Model::SetNormalMap,
                             "SetSpecularMap", &Model::SetSpecularMap,
                             "SetColor", &Model::SetColor);

    lua.new_usertype<Inputs>("Inputs", "IsKeyDown", &Inputs::IsKeyDown);

    lua.new_usertype<Camera>("Camera", 
                              "SetPosition", &Camera::SetPosition,
                              "SetRotation", &Camera::SetRotation,
                              "SetProjMode", &Camera::SetProjMode);
}

/*

*/

void ScriptComponent::SetupConstants(sol::state &lua)
{
    /* Printable keys */
    lua["INPUT_KEY_SPACE"]          = SDLK_SPACE;
    lua["INPUT_KEY_APOSTROPHE"]     = SDLK_APOSTROPHE;   /* ' */
    lua["INPUT_KEY_COMMA"]          = SDLK_COMMA;        /* , */
    lua["INPUT_KEY_MINUS"]          = SDLK_MINUS;        /* - */
    lua["INPUT_KEY_PERIOD"]         = SDLK_PERIOD;       /* . */
    lua["INPUT_KEY_SLASH"]          = SDLK_SLASH;        /* / */
    lua["INPUT_KEY_0"]              = SDLK_0;
    lua["INPUT_KEY_1"]              = SDLK_1;
    lua["INPUT_KEY_2"]              = SDLK_2;
    lua["INPUT_KEY_3"]              = SDLK_3;
    lua["INPUT_KEY_4"]              = SDLK_4;
    lua["INPUT_KEY_5"]              = SDLK_5;
    lua["INPUT_KEY_6"]              = SDLK_6;
    lua["INPUT_KEY_7"]              = SDLK_7;
    lua["INPUT_KEY_8"]              = SDLK_8;
    lua["INPUT_KEY_9"]              = SDLK_9;
    lua["INPUT_KEY_SEMICOLON"]      = SDLK_SEMICOLON;    /* ; */
    lua["INPUT_KEY_EQUAL"]          = SDLK_EQUALS;       /* = */
    lua["INPUT_KEY_A"]              = SDLK_A;
    lua["INPUT_KEY_B"]              = SDLK_B;
    lua["INPUT_KEY_C"]              = SDLK_C;
    lua["INPUT_KEY_D"]              = SDLK_D;
    lua["INPUT_KEY_E"]              = SDLK_E;
    lua["INPUT_KEY_F"]              = SDLK_F;
    lua["INPUT_KEY_G"]              = SDLK_G;
    lua["INPUT_KEY_H"]              = SDLK_H;
    lua["INPUT_KEY_I"]              = SDLK_I;
    lua["INPUT_KEY_J"]              = SDLK_J;
    lua["INPUT_KEY_K"]              = SDLK_K;
    lua["INPUT_KEY_L"]              = SDLK_L;
    lua["INPUT_KEY_M"]              = SDLK_M;
    lua["INPUT_KEY_N"]              = SDLK_N;
    lua["INPUT_KEY_O"]              = SDLK_O;
    lua["INPUT_KEY_P"]              = SDLK_P;
    lua["INPUT_KEY_Q"]              = SDLK_Q;
    lua["INPUT_KEY_R"]              = SDLK_R;
    lua["INPUT_KEY_S"]              = SDLK_S;
    lua["INPUT_KEY_T"]              = SDLK_T;
    lua["INPUT_KEY_U"]              = SDLK_U;
    lua["INPUT_KEY_V"]              = SDLK_V;
    lua["INPUT_KEY_W"]              = SDLK_W;
    lua["INPUT_KEY_X"]              = SDLK_X;
    lua["INPUT_KEY_Y"]              = SDLK_Y;
    lua["INPUT_KEY_Z"]              = SDLK_Z;
    lua["INPUT_KEY_LEFT_BRACKET"]   = SDLK_LEFTBRACKET;  /* [ */
    lua["INPUT_KEY_BACKSLASH"]      = SDLK_BACKSLASH;    /* \ */
    lua["INPUT_KEY_RIGHT_BRACKET"]  = SDLK_RIGHTBRACKET; /* ] */
    lua["INPUT_KEY_GRAVE_ACCENT"]   = SDLK_GRAVE;        /* ` */
    lua["INPUT_KEY_WORLD_1"]        = SDLK_UNKNOWN;      /* Omitted / Layout dependent in SDL3 */
    lua["INPUT_KEY_WORLD_2"]        = SDLK_UNKNOWN;      /* Omitted / Layout dependent in SDL3 */

    /* Function keys */
    lua["INPUT_KEY_ESCAPE"]         = SDLK_ESCAPE;
    lua["INPUT_KEY_ENTER"]          = SDLK_RETURN;
    lua["INPUT_KEY_TAB"]            = SDLK_TAB;
    lua["INPUT_KEY_BACKSPACE"]      = SDLK_BACKSPACE;
    lua["INPUT_KEY_INSERT"]         = SDLK_INSERT;
    lua["INPUT_KEY_DELETE"]         = SDLK_DELETE;
    lua["INPUT_KEY_RIGHT"]          = SDLK_RIGHT;
    lua["INPUT_KEY_LEFT"]           = SDLK_LEFT;
    lua["INPUT_KEY_DOWN"]           = SDLK_DOWN;
    lua["INPUT_KEY_UP"]             = SDLK_UP;
    lua["INPUT_KEY_PAGE_UP"]        = SDLK_PAGEUP;
    lua["INPUT_KEY_PAGE_DOWN"]      = SDLK_PAGEDOWN;
    lua["INPUT_KEY_HOME"]           = SDLK_HOME;
    lua["INPUT_KEY_END"]            = SDLK_END;
    lua["INPUT_KEY_CAPS_LOCK"]      = SDLK_CAPSLOCK;
    lua["INPUT_KEY_SCROLL_LOCK"]    = SDLK_SCROLLLOCK;
    lua["INPUT_KEY_NUM_LOCK"]       = SDLK_NUMLOCKCLEAR;
    lua["INPUT_KEY_PRINT_SCREEN"]   = SDLK_PRINTSCREEN;
    lua["INPUT_KEY_PAUSE"]          = SDLK_PAUSE;
    lua["INPUT_KEY_F1"]             = SDLK_F1;
    lua["INPUT_KEY_F2"]             = SDLK_F2;
    lua["INPUT_KEY_F3"]             = SDLK_F3;
    lua["INPUT_KEY_F4"]             = SDLK_F4;
    lua["INPUT_KEY_F5"]             = SDLK_F5;
    lua["INPUT_KEY_F6"]             = SDLK_F6;
    lua["INPUT_KEY_F7"]             = SDLK_F7;
    lua["INPUT_KEY_F8"]             = SDLK_F8;
    lua["INPUT_KEY_F9"]             = SDLK_F9;
    lua["INPUT_KEY_F10"]            = SDLK_F10;
    lua["INPUT_KEY_F11"]            = SDLK_F11;
    lua["INPUT_KEY_F12"]            = SDLK_F12;
    lua["INPUT_KEY_F13"]            = SDLK_F13;
    lua["INPUT_KEY_F14"]            = SDLK_F14;
    lua["INPUT_KEY_F15"]            = SDLK_F15;
    lua["INPUT_KEY_F16"]            = SDLK_F16;
    lua["INPUT_KEY_F17"]            = SDLK_F17;
    lua["INPUT_KEY_F18"]            = SDLK_F18;
    lua["INPUT_KEY_F19"]            = SDLK_F19;
    lua["INPUT_KEY_F20"]            = SDLK_F20;
    lua["INPUT_KEY_F21"]            = SDLK_F21;
    lua["INPUT_KEY_F22"]            = SDLK_F22;
    lua["INPUT_KEY_F23"]            = SDLK_F23;
    lua["INPUT_KEY_F24"]            = SDLK_F24;
    lua["INPUT_KEY_F25"]            = SDLK_UNKNOWN;      /* SDL3 maps up to F24 */
    lua["INPUT_KEY_KP_0"]           = SDLK_KP_0;
    lua["INPUT_KEY_KP_1"]           = SDLK_KP_1;
    lua["INPUT_KEY_KP_2"]           = SDLK_KP_2;
    lua["INPUT_KEY_KP_3"]           = SDLK_KP_3;
    lua["INPUT_KEY_KP_4"]           = SDLK_KP_4;
    lua["INPUT_KEY_KP_5"]           = SDLK_KP_5;
    lua["INPUT_KEY_KP_6"]           = SDLK_KP_6;
    lua["INPUT_KEY_KP_7"]           = SDLK_KP_7;
    lua["INPUT_KEY_KP_8"]           = SDLK_KP_8;
    lua["INPUT_KEY_KP_9"]           = SDLK_KP_9;
    lua["INPUT_KEY_KP_DECIMAL"]     = SDLK_KP_PERIOD;
    lua["INPUT_KEY_KP_DIVIDE"]      = SDLK_KP_DIVIDE;
    lua["INPUT_KEY_KP_MULTIPLY"]    = SDLK_KP_MULTIPLY;
    lua["INPUT_KEY_KP_SUBTRACT"]    = SDLK_KP_MINUS;
    lua["INPUT_KEY_KP_ADD"]         = SDLK_KP_PLUS;
    lua["INPUT_KEY_KP_ENTER"]       = SDLK_KP_ENTER;
    lua["INPUT_KEY_KP_EQUAL"]       = SDLK_KP_EQUALS;
    lua["INPUT_KEY_LEFT_SHIFT"]     = SDLK_LSHIFT;
    lua["INPUT_KEY_LEFT_CONTROL"]   = SDLK_LCTRL;
    lua["INPUT_KEY_LEFT_ALT"]       = SDLK_LALT;
    lua["INPUT_KEY_LEFT_SUPER"]     = SDLK_LGUI;
    lua["INPUT_KEY_RIGHT_SHIFT"]    = SDLK_RSHIFT;
    lua["INPUT_KEY_RIGHT_CONTROL"]  = SDLK_RCTRL;
    lua["INPUT_KEY_RIGHT_ALT"]      = SDLK_RALT;
    lua["INPUT_KEY_RIGHT_SUPER"]    = SDLK_RGUI;
    lua["INPUT_KEY_MENU"]           = SDLK_MENU;
}