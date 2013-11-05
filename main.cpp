#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstdio>

#define SUCCESS 1

int 	initGL();

GLFWwindow * g_window;

int main()
{
	if(initGL() != SUCCESS)
		return -1;
	
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(g_window))
    {
        /* Render here */

        /* Swap front and back buffers */
        glfwSwapBuffers(g_window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();

	return 0;
}

int initGL()
{
    GLFWwindow* g_window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    g_window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!g_window)
    {
        glfwTerminate();
        return -1;
    }

    glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		printf("GLEW fatal error \n");
		return -1;
	}

    /* Make the window's context current */
    glfwMakeContextCurrent(g_window);

	return SUCCESS;
}