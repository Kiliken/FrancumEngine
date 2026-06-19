#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */

#include <glad/gl.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_opengl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl3.h>
#include <imgui/imgui_impl_opengl3.h>

#include <sol/sol.hpp>

#include "utils/loadShader.h"
#include "utils/loadDDS.h"
#include "Inputs.h"
#include "Camera.h"
#include "utils/vboIndexer.h"
#include "Model.h"
#include "utils/loadOBJ.h"
#include "Object.h"
#include "SkySphere.h"
#include "ScriptComponent.h"

#include "configs.h"

// Force on Dedicated GPU
extern "C"
{
    __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

static uint64_t menuFlags = 0;
/*
Flags:
    1 - Fast Reload

*/

ModelConfig DefaultModelConfig;

// SDL Public Variables
static SDL_Window *window = NULL;
static SDL_GLContext glctx = NULL;

static sol::state lua;
std::vector<std::unique_ptr<ScriptComponent>> loadedScripts;

static Inputs *inputs = NULL;
static Camera *camera = NULL;

// UI Variables
int winWidth, winHeight;
ImVec4 clear_color;
glm::vec3 lightPos;
glm::vec3 cubePos;
glm::vec3 cubeRot;
float cubeScale;

// Render Variables
glm::mat4 View;
GLuint programID;
GLuint lightID;
Uint64 lastTime = 0;
Object cube;

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    // OpenGL 4.6 Core Profile Setup
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_WindowFlags window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;

    /* Create the window */
    window = SDL_CreateWindow("Francum Engine", 1280, 800, window_flags);

    if (!window)
    {
        SDL_Log("Couldn't create window and renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    /* Manage OpenGL context*/
    glctx = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, glctx);

    SDL_GL_SetSwapInterval(1); // Enable vsync
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_ShowWindow(window);

    float main_scale = SDL_GetDisplayContentScale(SDL_GetDisplayForWindow(window));
    if (main_scale <= 0.0f)
        main_scale = 1.0f; // Safety fallback

    int version = gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress);
    if (version == 0)
    {
        std::cout << "Failed to initialize OpenGL context" << std::endl;
        return SDL_APP_FAILURE;
    }

    std::cout << "Loaded OpenGL " << GLAD_VERSION_MAJOR(version) << "." << GLAD_VERSION_MINOR(version) << std::endl;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

    ImGui::StyleColorsDark();

    ImGuiStyle &style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale); // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.FontScaleDpi = main_scale; // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)

    ImGui_ImplSDL3_InitForOpenGL(window, glctx);
    ImGui_ImplOpenGL3_Init("#version 460");

    // UI Variables
    clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    lightPos = glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f));
    cubePos = glm::vec3(0.0f);
    cubeRot = glm::vec3(0.0f);
    cubeScale = 1.f;

    // Initiate Lua Scripting
    lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::math, sol::lib::table, sol::lib::string, sol::lib::io, sol::lib::os, sol::lib::jit);
    ScriptComponent::BindFunctions(lua);
    ScriptComponent::SetupConstants(lua);
    lua["FEngine"] = lua.create_table();
    lua.script("print('[Sol3] Lua Scripting Loaded')");

    // Initialize Camera
    camera = new Camera(window);

    // Initialize Inputs
    inputs = new Inputs(window);

    lua["FEngine"]["Inputs"] = inputs;
    lua["FEngine"]["Camera"] = camera;

    // Load Scripts
    {
        auto files = ScriptComponent::GetScriptsInFolder(ASSETS("scripts"));
        for (auto &file : files)
        {
            loadedScripts.push_back(std::make_unique<ScriptComponent>(lua, file));
        }
    }

    SDL_GetWindowSize(window, &winWidth, &winHeight);

    // Dark blue background
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it is closer to the camera than the former one
    glDepthFunc(GL_LESS);

    glEnable(GL_CULL_FACE);

    // Camera matrix
    View = glm::lookAt(
        glm::vec3(4, 3, 3), // Camera is at (4,3,3), in World Space
        glm::vec3(0, 0, 0), // and looks at the origin
        glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
    );

    // Create and compile our GLSL program from the shaders
    // programID = LoadShaders("../res/shaders/NormalMappingShader.vert", "../res/shaders/NormalMappingShader.frag");
    programID = Utils::LoadSPIRV(ASSETS("shaders/StandardShader.vert.spv"), ASSETS("shaders/StandardShader.frag.spv"));

    camera->BindToShader();
    lightID = glGetUniformLocation(programID, "LightDirection_worldspace");

    {
        DefaultModelConfig.fileName = ASSETS("cube.obj");
        DefaultModelConfig.prog = &programID;
    }

    cube.AddModels("../res/cube.obj");

    // LuaScript Start
    for (auto &script : loadedScripts)
        script->Start();

    return SDL_APP_CONTINUE;
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{

    if (camera->showUI)
    {
        ImGui_ImplSDL3_ProcessEvent(event);
    }

    if (event->type == SDL_EVENT_QUIT)
    {
        return SDL_APP_SUCCESS; /* end the program, reporting success to the OS. */
    }
    return SDL_APP_CONTINUE;
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void *appstate)
{
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);

    // Resize Window
    int w, h;
    SDL_GetWindowSizeInPixels(window, &w, &h);
    if (w != winWidth || h != winHeight)
    {
        winWidth = w;
        winHeight = h;

        glViewport(0, 0, winWidth, winHeight);
        camera->resizeView(winWidth, winHeight);
    }

    // Delta Time
    const Uint64 currentTime = SDL_GetPerformanceCounter();
    float deltaTime = (double)(currentTime - lastTime) / (double)SDL_GetPerformanceFrequency();
    lastTime = currentTime;

    camera->Update(deltaTime);

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
    ImGuiIO &io = ImGui::GetIO();

    ImGui::SetMouseCursor((camera->showUI ? ImGuiMouseCursor_Arrow : ImGuiMouseCursor_None));

    // Main Menu
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Reload", "F5")) // Reload Engine
            {
                menuFlags |= (1ULL << 1); // Fast Reload Flag to 1
                SDL_Event e;
                e.type = SDL_EVENT_QUIT;
                SDL_PushEvent(&e);
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Create"))
            {
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View"))
        {
            if (ImGui::MenuItem("Show/Hide FPS"))
            {
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("?"))
        {
            if (ImGui::MenuItem("Repository"))
            {
                std::system("start \"\" \"https://github.com/Kiliken/FrancumEngine\"");
            }
            if (ImGui::MenuItem("Documentation"))
            {
                std::system("start \"\" \"https://kiliken.github.io/mdRenderer.html?mdurl=./data/worksMd/FrancumEn.md\"");
            }
            ImGui::EndMenu();
        }

        ImGui::Text("(%.1f FPS)", io.Framerate);
        ImGui::EndMainMenuBar();
    }

    if (camera->showUI)
    {

        // Standard Window
        ImGui::Begin("Francum Engine");

        ImGui::Text("This is some useful text."); // Display some text (you can use a format strings too)
        ImGui::DragFloat3("Light Direction", glm::value_ptr(lightPos));

        ImGui::ColorEdit3("clear color", (float *)&clear_color); // Edit 3 floats representing a colorwd

        ImGui::Text("Cube Transform");
        ImGui::DragFloat3("Cube Pos", glm::value_ptr(cubePos));
        ImGui::DragFloat3("Cube Rot", glm::value_ptr(cubeRot));
        ImGui::SliderFloat("Cube Scale", // The text label for the slider
                           &cubeScale,   // Address of the variable to link
                           0.0f,         // Minimum value (v_min)
                           10.0f,        // Maximum value (v_max)
                           "%.1f");      // Display format (e.g., one decimal place)
        ImGui::End();
    }

    cube.SetPosition(cubePos.x, cubePos.y, cubePos.z);
    cube.SetRotation(cubeRot.x, cubeRot.y, cubeRot.z);
    cube.SetScale(cubeScale, cubeScale, cubeScale);

    // Camera matrix
    View = glm::lookAt(
        camera->position,                     // Camera is here
        camera->position + camera->direction, // and looks here : at the same position, plus "direction"
        camera->up                            // Head is up (set to 0,-1,0 to look upside-down)
    );

    cube.Update(deltaTime);

    // LuaScript Update
    for (auto &script : loadedScripts)
        script->Update(deltaTime);

    /* Render here */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindBuffer(GL_UNIFORM_BUFFER, camera->UBOID);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(CameraUBO), &camera->UBOdata);

    glUniform3f(lightID, lightPos.x, lightPos.y, lightPos.z);

    cube.Draw();

    // LuaScript Draw
    for (auto &script : loadedScripts)
        script->Draw();

    // Rendering
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    SDL_GL_SwapWindow(window);

    return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{

    SDL_DestroyWindow(window);
    window = nullptr;

    SDL_GL_DestroyContext(glctx);
    glctx = nullptr;

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    delete (inputs);
    inputs = nullptr;

    delete (camera);
    camera = nullptr;

    glDeleteProgram(programID);
    glDeleteBuffers(1, &lightID);

    if (menuFlags & (1ULL << 1)) // Fast Reload Flag Check
    {
        std::system("start \"\" Francum.exe");
    }
}