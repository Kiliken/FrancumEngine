
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>


#include "Renderer.cpp"
#include "VertexBuffer.cpp"
#include "IndexBuffer.cpp"
#include "VertexArray.cpp"
#include "Shader.cpp"

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);

    if(glewInit() != GLEW_OK)
        return -1;

    //Program Start
    {
    float positions[] = {
        -0.5f, -0.5f,
         0.5f, -0.5f,
         0.5f,  0.5f,
        -0.5f,  0.5f,
    };

    unsigned int indices[] = {
        0,1,2,
        2,3,0,
    };
    
    VertexArray va;
    VertexBuffer vb(positions, 4*2*sizeof(float));
    VertexBufferLayout layout;
    layout.Push<float>(2);
    va.AddBuffer(vb, layout);

    IndexBuffer ib(indices, 6);

    Shader shader("res/shaders/basicShaders.glsl");
    shader.Bind();
    shader.SetUnisform4f("u_Color",0.0f, 0.0f, 0.0f, 1.0f);

    Renderer renderer;

    float r = 0.0f;
    float increament = 0.05f;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        renderer.Clear();

        shader.SetUnisform4f("u_Color",r, 0.0f, 0.0f, 1.0f);
        
        renderer.Draw(va,ib,shader);

        if (r > 1.0f)
            increament = -0.05f;
        else if (r < 0.0f)
            increament = 0.05f;

        r += increament;

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }
    }   
    glfwTerminate();
    return 0;
}