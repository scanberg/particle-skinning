#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstdio>

#define SUCCESS 1
#define WIDTH 	640
#define HEIGHT 	480

int 	initGL(int width, int height);

GLFWwindow * g_window;

int main()
{
	if(initGL(WIDTH, HEIGHT) != SUCCESS)
		return -1;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(g_window) &&
        !glfwGetKey(g_window, GLFW_KEY_Q) &&
        !glfwGetKey(g_window, GLFW_KEY_ESCAPE))
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

int initGL(int width, int height)
{
    /* Initialize the library */
    if (!glfwInit())
        return -1;

#ifdef __APPLE__
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    /* Create a windowed mode window and its OpenGL context */
    g_window = glfwCreateWindow(width, height, "Hello World", NULL, NULL);
    if (!g_window)
    {
		printf("glfw error: window could not be created \n");
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(g_window);

    glewExperimental = GL_TRUE;
	GLenum error = glewInit();
	if(GLEW_OK != error)
	{
		printf("glew error: init returned an error \n");
		glfwTerminate();
		return -1;
	}

    printf("Successfully created OpenGL %i.%i context \n",
        glfwGetWindowAttrib(g_window, GLFW_CONTEXT_VERSION_MAJOR),
        glfwGetWindowAttrib(g_window, GLFW_CONTEXT_VERSION_MINOR));

    printf("GLSL version %s \n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	return SUCCESS;
}