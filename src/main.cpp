#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
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
//#include "SkySphere.h"
#include "ScriptComponent.h"

// Force on Dedicated GPU
extern "C" {
    __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

static uint64_t menuFlags = 0;
/*
Flags:
    1 - Fast Reload

*/

ModelConfig DefaultModelConfig;

int main(void)
{
    GLFWwindow *window = nullptr;

    // Initialize GLFW
    if (!glfwInit())
    {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Window Settings
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    /* Create a windowed mode window and its OpenGL context */
    float ImGuiMainScale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor()); // Valid on GLFW 3.3+ only
    window = glfwCreateWindow((int)(1280 * ImGuiMainScale), (int)(800 * ImGuiMainScale), "FrancumEngine", NULL, NULL);
    if (!window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    int version = gladLoadGL(glfwGetProcAddress);
    if (version == 0)
    {
        std::cout << "Failed to initialize OpenGL context" << std::endl;
        return -1;
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
    style.ScaleAllSizes(ImGuiMainScale); // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.FontScaleDpi = ImGuiMainScale; // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // UI Variables
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    glm::vec3 lightPos = glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f));
    glm::vec3 cubePos = glm::vec3(0.0f);
    glm::vec3 cubeRot = glm::vec3(0.0f);
    float cubeScale = 1.f;

    // Initiate Lua Scripting
    sol::state lua;
    lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::math, sol::lib::table, sol::lib::string, sol::lib::io, sol::lib::os);
    BindFunctions(lua);
    SetupConstants(lua);
    lua["FEngine"] = lua.create_table();
    lua.script("print('[Sol3] Lua Scripting Loaded')");

    // Initialize Camera
    Camera camera(window);

    // Initialize Inputs
    Inputs inputs(window);
    lua["FEngine"]["Inputs"] = &inputs;

    // Load Scripts
    std::vector<std::unique_ptr<ScriptComponent>> loadedScripts;
    {
        auto files = GetScriptsInFolder("../res/scripts/");
        for (auto &file : files)
        {
            loadedScripts.push_back(std::make_unique<ScriptComponent>(lua, file));
        }
    }

    int winWidth, winHeight;
    glfwGetWindowSize(window, &winWidth, &winHeight);

    // Dark blue background
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it is closer to the camera than the former one
    glDepthFunc(GL_LESS);

    glEnable(GL_CULL_FACE);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    // Camera Projections
    glm::mat4 Projection;
    if (true) // perspective
        Projection = glm::perspective(glm::radians(60.0f), 16.0f / 9.0f, 0.1f, 100.0f);
    else // orthographic
        Projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.0f, 100.0f);

    // Camera matrix
    glm::mat4 View = glm::lookAt(
        glm::vec3(4, 3, 3), // Camera is at (4,3,3), in World Space
        glm::vec3(0, 0, 0), // and looks at the origin
        glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
    );

    // Create and compile our GLSL program from the shaders
    //GLuint programID = LoadShaders("../res/shaders/NormalMappingShader.vert", "../res/shaders/NormalMappingShader.frag");
    GLuint programID = Utils::LoadSPIRV("../res/shaders/StandardShader.vert.spv", "../res/shaders/StandardShader.frag.spv");

    {
        DefaultModelConfig.fileName = "../res/cube.obj";
        DefaultModelConfig.prog = &programID;
        DefaultModelConfig.View = &View;
        DefaultModelConfig.camera = &Projection;
        DefaultModelConfig.lightPos = &lightPos;
    }

    Object cube;
    cube.AddModels("../res/cube.obj");

    float lastTime = 0.0f;

    // LuaScript Start
    for (auto &script : loadedScripts)
        script->Start();

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);

        // Resize Window
        int w,h;
        glfwGetFramebufferSize(window, &w, &h);
        if(w != winWidth || h != winHeight){
            winWidth = w;
            winHeight = h;

            glViewport(0,0,winWidth,winHeight);
            camera.resizeView(winWidth,winHeight);
        }


        // Delta Time
        double currentTime = glfwGetTime();
        float deltaTime = float(currentTime - lastTime);
        lastTime = currentTime;

        camera.Update(deltaTime);

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::SetMouseCursor((camera.showUI ? ImGuiMouseCursor_Arrow : ImGuiMouseCursor_None));
        

        // Main Menu
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Reload", "F5")) //Reload Engine
                {
                    menuFlags |= (1ULL << 1); // Fast Reload Flag to 1
                    glfwSetWindowShouldClose(window, GLFW_TRUE);
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

        if (camera.showUI)
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
            camera.position,                    // Camera is here
            camera.position + camera.direction, // and looks here : at the same position, plus "direction"
            camera.up                           // Head is up (set to 0,-1,0 to look upside-down)
        );

        cube.Update(deltaTime);

        // LuaScript Update
        for (auto &script : loadedScripts)
            script->Update(deltaTime);

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        cube.Draw();

        // LuaScript Draw
        for (auto &script : loadedScripts)
            script->Draw();


        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteProgram(programID);

    // ImGui Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    if (menuFlags & (1ULL << 1)) // Fast Reload Flag Check
    {
        std::system("start \"\" Francum.exe");
    }

    return 0;
}