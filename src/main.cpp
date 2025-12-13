#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "loadShader.h"
#include "loadDDS.h"
#include "loadOBJ.h"
#include "Inputs.h"
#include "vboIndexer.h"

int main(void)
{
    GLFWwindow *window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(1280, 720, "OpenGL", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    // Initialize Inputs
    Inputs inputs(window);

    int winWidth, winHeight;
    glfwGetWindowSize(window, &winWidth, &winHeight);

    if (glewInit() == GLEW_OK)
        std::cout << glGetString(GL_VERSION) << std::endl;

    // Dark blue background
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

    // Enable depth test
    // glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    // Accept fragment if it is closer to the camera than the former one
    glDepthFunc(GL_LESS);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    // Read our .obj file
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals; 
    std::vector<unsigned short> indices;
    bool res = loadOBJ("../res/cube.obj", vertices, uvs, normals);

    // fill "indices" as needed
    std::vector<glm::vec3> idxVertices;
    std::vector<glm::vec2> idxUvs;
    std::vector<glm::vec3> idxNormals; 
    indexVBO(vertices,uvs,normals,indices,idxVertices,idxUvs,idxNormals);
    


    // GL Stuffs

    // Generate a buffer for the indices
    GLuint elementbuffer;
    glGenBuffers(1, &elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, idxVertices.size() * sizeof(glm::vec3), &idxVertices[0], GL_STATIC_DRAW);

    GLuint uvsbuffer;
    glGenBuffers(1, &uvsbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvsbuffer);
    glBufferData(GL_ARRAY_BUFFER, idxUvs.size() * sizeof(glm::vec3), &idxUvs[0], GL_STATIC_DRAW);

    GLuint normalbuffer;
    glGenBuffers(1, &normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, idxNormals.size() * sizeof(glm::vec3), &idxNormals[0], GL_STATIC_DRAW);

    GLuint Texture = loadDDS("../res/uvtemplate.dds");

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

    // Model matrix: an identity matrix (model will be at the origin)
    glm::mat4 Model = glm::mat4(1.0f);
    // Our ModelViewProjection: multiplication of our 3 matrices
    glm::mat4 mvp = Projection * View * Model; // Remember, matrix multiplication is the other way around

    // Create and compile our GLSL program from the shaders
    GLuint programID = LoadShaders("../res/shaders/StandardShadingShader.vert", "../res/shaders/StandardShadingShader.frag");

    // Get a handle for our uniforms
    GLuint MatrixID = glGetUniformLocation(programID, "MVP");
    GLuint ModelMatrixID = glGetUniformLocation(programID, "M");
    GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
    GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");

    float lastTime = 0.0f;
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        double currentTime = glfwGetTime();
        float deltaTime = float(currentTime - lastTime);
        lastTime = currentTime;

        inputs.Update(deltaTime);

        // Projection matrix : 45&deg; Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
        Projection = glm::perspective(glm::radians(60.0f), 16.0f / 9.0f, 0.1f, 100.0f);
        // Camera matrix
        View = glm::lookAt(
            inputs.position,                    // Camera is here
            inputs.position + inputs.direction, // and looks here : at the same position, plus "direction"
            inputs.up                           // Head is up (set to 0,-1,0 to look upside-down)
        );

        mvp = Projection * View * Model;

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 1st attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(
            0,        // attribute 0. No particular reason for 0, but must match the layout in the shader.
            3,        // size
            GL_FLOAT, // type
            GL_FALSE, // normalized?
            0,        // stride
            (void *)0 // array buffer offset
        );

        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, uvsbuffer);
        glVertexAttribPointer(
            1,        // attribute. No particular reason for 1, but must match the layout in the shader.
            2,        // size
            GL_FLOAT, // type
            GL_FALSE, // normalized?
            0,        // stride
            (void *)0 // array buffer offset
        );

        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
        glVertexAttribPointer(
            2,        // attribute
            3,        // size
            GL_FLOAT, // type
            GL_FALSE, // normalized?
            0,        // stride
            (void *)0 // array buffer offset
        );

        // This is done in the main loop since each model will have a different MVP matrix (At least for the M part)
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);
        glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &Model[0][0]);
        glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &View[0][0]);

        glm::vec3 lightPos = glm::vec3(4, 4, 4);
        glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);

        glUseProgram(programID);

        // Draw the triangles !
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
        glDrawElements(
            GL_TRIANGLES,    // mode
            indices.size(),  // count
            GL_UNSIGNED_SHORT, // type
            (void *)0        // element array buffer offset
        );

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &uvsbuffer);
    glDeleteBuffers(1, &normalbuffer);
    glDeleteProgram(programID);
    glDeleteTextures(1, &Texture);
    glDeleteVertexArrays(1, &VertexArrayID);

    glfwTerminate();
    return 0;
}