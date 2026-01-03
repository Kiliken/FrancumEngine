#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include "loadShader.h"
#include "loadDDS.h"
#include "loadOBJ.h"
#include "Inputs.h"
#include "vboIndexer.h"
#include "Object.h"

int main(void)
{
    GLFWwindow *window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    float ImGuiMainScale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor()); // Valid on GLFW 3.3+ only
    window = glfwCreateWindow((int)(1280 * ImGuiMainScale), (int)(800 * ImGuiMainScale), "OpenGL", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync


    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(ImGuiMainScale); // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.FontScaleDpi = ImGuiMainScale; // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    //UI Variables
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    glm::vec3 lightPos = glm::vec3(0, 0, 4);
    glm::vec3 cubePos, cubeRot, cubeScale = glm::vec3(1,1,1);


    // Initialize Inputs
    Inputs inputs(window);

    int winWidth, winHeight;
    glfwGetWindowSize(window, &winWidth, &winHeight);

    if (glewInit() == GLEW_OK)
        std::cout << glGetString(GL_VERSION) << std::endl;

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
        Projection = glm::perspective(glm::radians(45.0f), (float)winWidth / (float)winHeight, 0.1f, 100.0f);
    else // orthographic
        Projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.0f, 100.0f);

    // Camera matrix
    glm::mat4 View = glm::lookAt(
        glm::vec3(4, 3, 3), // Camera is at (4,3,3), in World Space
        glm::vec3(0, 0, 0), // and looks at the origin
        glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
    );


    // Create and compile our GLSL program from the shaders
    GLuint programID = LoadShaders("../res/shaders/NormalMappingShader.vert", "../res/shaders/NormalMappingShader.frag");


    Object cube("../res/cube.obj",&programID,&View,Projection,&lightPos);
    Object cube2("../res/cube.obj",&programID,&View,Projection,&lightPos);
    

    float lastTime = 0.0f;

    

    
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {   
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);

        double currentTime = glfwGetTime();
        float deltaTime = float(currentTime - lastTime);
        lastTime = currentTime;

        inputs.Update(deltaTime);
        
        if (inputs.showUI)
        {
            // Start the Dear ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
        }
        
        

        if (inputs.showUI)
        {
            
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::InputFloat3("Position", glm::value_ptr(lightPos));

            //ImGui::SliderFloat("float", nullptr, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a colorwd

            ImGui::Text("Cube Transform");
            ImGui::InputFloat3("Cube Pos", glm::value_ptr(cubePos));
            ImGui::InputFloat3("Cube Rot", glm::value_ptr(cubeRot));
            ImGui::InputFloat3("Cube Scale", glm::value_ptr(cubeScale));


            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        }

        cube.Position(cubePos);
        cube.Rotation(cubeRot);
        cube.Scale(cubeScale);


        // Projection matrix : 45&deg; Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
        Projection = glm::perspective(glm::radians(60.0f), 16.0f / 9.0f, 0.1f, 100.0f);
        // Camera matrix
        View = glm::lookAt(
            inputs.position,                    // Camera is here
            inputs.position + inputs.direction, // and looks here : at the same position, plus "direction"
            inputs.up                           // Head is up (set to 0,-1,0 to look upside-down)
        );

        cube.Update(deltaTime);
        cube2.Update(deltaTime);

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        cube.Draw();
        cube2.Draw();

        if (inputs.showUI)
        {
            // Rendering
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }
        
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

    glfwTerminate();
    return 0;
}