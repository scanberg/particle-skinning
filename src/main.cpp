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
#define TIMESTAMPS 3

int initGL(int width, int height);
void initGeom(unsigned int &vao, unsigned int &vbo);
void setWindowTitle(unsigned int * queryID, double dt);

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

    // queries for accurate profiling of opengl calls.
    unsigned int queryID[TIMESTAMPS];
    glGenQueries(TIMESTAMPS, queryID);

    /* Loop until the user closes the window */
	while (!glfwWindowShouldClose(g_window) &&
		!glfwGetKey(g_window, GLFW_KEY_Q) &&
		!glfwGetKey(g_window, GLFW_KEY_ESCAPE))
	{
		int loc;

		double dt = calcDT();
        static double t = 0;
        t += dt;

		//if (glfwGetKey(g_window, GLFW_KEY_SPACE))
            bob.rotate(glm::vec3(0,0,3*glm::cos(2.0*t)*dt));

		//bob.addRandomImpulse(5.0f);

        glQueryCounter(queryID[0], GL_TIMESTAMP);
        bob.update((float)dt);
        glQueryCounter(queryID[1], GL_TIMESTAMP);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, 640, 480);

        glEnable(GL_DEPTH_TEST);
        //glEnable(GL_CULL_FACE);

        basicShader.bind();

        loc = basicShader.getUniformLocation("viewMatrix");
        glUniformMatrix4fv(loc, 1, false, glm::value_ptr(camera.getViewMatrix()));

        loc = basicShader.getUniformLocation("projMatrix");
        glUniformMatrix4fv(loc, 1, false, glm::value_ptr(camera.getProjMatrix()));

        /* Render here */
        bob.draw();

        glQueryCounter(queryID[2], GL_TIMESTAMP);

        /* Swap front and back buffers */
        glfwSwapBuffers(g_window);

        setWindowTitle(queryID, dt);

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
	//glfwSwapInterval(1);

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

void setWindowTitle(unsigned int * queryID, double dt)
{
    static double t = 0.0;
    static unsigned int f = 0;
    t += dt;
    ++f;

    if (t > 0.5)
    {
        GLuint64 timeStamp[3];

        for(int i=0; i<TIMESTAMPS; ++i)
            glGetQueryObjectui64v(queryID[i], GL_QUERY_RESULT, &timeStamp[i]);

        double physics  = (timeStamp[1] - timeStamp[0]) / 1000000.0;
        double render   = (timeStamp[2] - timeStamp[1]) / 1000000.0;

        char title[256];
        sprintf(title, "FPS %i, time (ms): physics %.5f, rander %.5f", (int)(f / t), physics, render);
        glfwSetWindowTitle(g_window, title);

        t = 0.0;
        f = 0;
    }
}