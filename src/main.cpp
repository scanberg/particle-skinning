#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cstdio>

#include "Body.h"
#include "Model.h"
#include "Shader.h"
#include "Camera.h"
#include "GPUParticleSystem.h"
#include "ParticleSkinnedModel.h"

#define SUCCESS 1
#define WIDTH 	640
#define HEIGHT 	480

int initGL(int width, int height);
void initGeom(unsigned int &vao, unsigned int &vbo);

double calcDT();

GLFWwindow * g_window;

int main()
{
	if(initGL(WIDTH, HEIGHT) != SUCCESS)
		return -1;

    Shader basicShader;
    basicShader.attachShader(GL_VERTEX_SHADER, "data/shaders/basic.vert");
    basicShader.attachShader(GL_FRAGMENT_SHADER, "data/shaders/basic.frag");
    basicShader.bindAttribLocation(0, "in_position");
    basicShader.bindFragDataLocation(0, "out_frag0");

    basicShader.link();

    Shader particleShader;
    particleShader.attachShader(GL_VERTEX_SHADER, "data/shaders/particle.vert");
    particleShader.bindAttribLocation(0, "in_position");
    particleShader.bindAttribLocation(1, "in_oldPosition");
    particleShader.bindAttribLocation(2, "in_mass");
    const char* varyings[3] = { "out_position", "out_oldPosition", "out_mass" };
    glTransformFeedbackVaryings(particleShader.getProgram(), 3, varyings, GL_INTERLEAVED_ATTRIBS);

    particleShader.link();

    Body bobMesh( "data/bob/bob.md5mesh", "data/bob/bob.md5anim" );

    ParticleSkinnedModel bob( &particleShader, &bobMesh );

	Camera camera;

    camera.setPosition(glm::vec3(0,0,5));

    bob.rotate(glm::vec3(-90,0,0));
    bob.setScale(glm::vec3(0.1));
    bob.setPosition(glm::vec3(0,-3,0));

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(g_window) &&
        !glfwGetKey(g_window, GLFW_KEY_Q) &&
        !glfwGetKey(g_window, GLFW_KEY_ESCAPE))
    {
		int loc;

        double dt = calcDT();

        bob.rotate(glm::vec3(0,0,0.01));

        bob.update((float)dt);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, 640, 480);

        glEnable(GL_DEPTH_TEST);
        //glEnable(GL_CULL_FACE);

        basicShader.bind();

        loc = basicShader.getUniformLocation("viewMatrix");
        glUniformMatrix4fv(loc, 1, false, glm::value_ptr(camera.getViewMatrix() * bob.getTransform().getMat4()));

        loc = basicShader.getUniformLocation("projMatrix");
        glUniformMatrix4fv(loc, 1, false, glm::value_ptr(camera.getProjMatrix()));

        /* Render here */
        bob.draw();

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
	glfwSwapInterval(1);

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

double calcDT()
{
    static double oldT = 0.0;
    double t = glfwGetTime();
    double dt = t - oldT;
    oldT = t;

    return dt;
}