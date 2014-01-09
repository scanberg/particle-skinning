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
#include "Texture2D.h"

#define SUCCESS 1
#define WIDTH 	800
#define HEIGHT 	600
#define TIMESTAMPS 3

int initGL(int width, int height);
void initGeom(unsigned int &vao, unsigned int &vbo);
void setWindowTitle(double dt);

double calcDT();

GLFWwindow *	g_window;
GLuint			g_queryID[TIMESTAMPS];

int main(int argc, char* argv[])
{
	if(initGL(WIDTH, HEIGHT) != SUCCESS)
		return -1;

    Shader particleShader;
    particleShader.attachShader(GL_VERTEX_SHADER, "data/shaders/particle.vert");
    const char* ps_varyings[3] = { "out_position", "out_oldPosition", "out_mass_k_d" };
    glTransformFeedbackVaryings(particleShader.getProgram(), 3, ps_varyings, GL_INTERLEAVED_ATTRIBS);
    particleShader.link();

    Shader basicShader;
    basicShader.attachShader(GL_VERTEX_SHADER, "data/shaders/basic.vert");
    basicShader.attachShader(GL_FRAGMENT_SHADER, "data/shaders/basic.frag");
    basicShader.bindFragDataLocation(0, "out_frag0");
    basicShader.link();

	/* Take paths to md5mesh and md5anim as arguments */
	std::string filename, dir, animation;
	if(argc == 3){
		filename = std::string(argv[1]);
		dir = filename.substr(0,filename.rfind("/")+1);
		animation = std::string(argv[2]);
		printf("%s, %s\n", filename.c_str(), dir.c_str());
	} else {
		filename = "data/fatty/zfat.md5mesh";
		dir = filename.substr(0,filename.rfind("/")+1);
		animation = dir + "bellypain.md5anim";
	}

    Body body(filename.c_str(), 0.025f);
    body.addAnimation(animation.c_str(), "idle");

    const std::vector<Body::sMaterial>& matfiles = body.getMaterials();

    Texture2D diffuse( (dir + std::string(matfiles[0].diffuse.getStr()) ).c_str() );
    Texture2D normals( (dir + std::string(matfiles[0].normal.getStr()) ).c_str() );
    Texture2D height( (dir + std::string(matfiles[0].height.getStr()) ).c_str() );
    Texture2D specular( (dir + std::string(matfiles[0].specular.getStr()) ).c_str() );

	Material material(basicShader);
	material.setTexture(Material::TEX_DIFFUSE, &diffuse);
	material.setTexture(Material::TEX_NORMAL, &normals);
	material.setTexture(Material::TEX_HEIGHT, &height);
	material.setTexture(Material::TEX_SMOOTHNESS, &specular);

    ParticleSkinnedModel model( particleShader, &body );
	model.setAnimation("idle");
	model.play(1.0f);
	model.simulateOnGPU(false);

	Camera camera;

    camera.setPosition(glm::vec3(0,1.5,3.5));

    model.rotate(glm::vec3(0,0,0));
    model.setPosition(glm::vec3(0,0,0));
	model.resetParticlePositions();

    // queries for accurate profiling of opengl calls.
    
    glGenQueries(TIMESTAMPS, g_queryID);

    glEnable( GL_TEXTURE_2D );

    /* Loop until the user closes the window */
	while (!glfwWindowShouldClose(g_window) &&
		!glfwGetKey(g_window, GLFW_KEY_Q) &&
		!glfwGetKey(g_window, GLFW_KEY_ESCAPE))
	{
		int loc;

		double dt = calcDT();
        static double t = 0;
        t += dt;

        static bool hit = true;
		if (glfwGetKey(g_window, GLFW_KEY_SPACE))
        {
            if(!hit)
            {
                if(model.isPlaying())
                    model.pause();
                else
                    model.play();
                hit = true;
            }
        }
        else
            hit = false;

		if(glfwGetKey(g_window, GLFW_KEY_1))
			model.simulateOnGPU(true);

		if(glfwGetKey(g_window, GLFW_KEY_2))
			model.simulateOnGPU(false);

		if (glfwGetKey(g_window, GLFW_KEY_LEFT))
			model.rotate(glm::vec3(0,-dt*1,0));

		if (glfwGetKey(g_window, GLFW_KEY_RIGHT))
			model.rotate(glm::vec3(0,dt*1,0));

        glQueryCounter(g_queryID[0], GL_TIMESTAMP);
        model.update((float)dt);
        glQueryCounter(g_queryID[1], GL_TIMESTAMP);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, WIDTH, HEIGHT);

        glEnable(GL_DEPTH_TEST);
        //glEnable(GL_CULL_FACE);

        basicShader.bind();

		glm::mat4 MV = camera.getViewMatrix() * model.getTransform().getMat4();

        loc = basicShader.getUniformLocation("viewMatrix");
        glUniformMatrix4fv(loc, 1, false, glm::value_ptr(MV));

        loc = basicShader.getUniformLocation("projMatrix");
        glUniformMatrix4fv(loc, 1, false, glm::value_ptr(camera.getProjMatrix()));

        loc = basicShader.getUniformLocation("texture0");
        glUniform1i(loc, 0);

		material.bind();
        /* Render here */
        model.drawPart(0);

        glQueryCounter(g_queryID[2], GL_TIMESTAMP);

        /* Swap front and back buffers */
        glfwSwapBuffers(g_window);

        setWindowTitle(dt);

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

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

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

void setWindowTitle(double dt)
{
    static double t = 0.0;
    static unsigned int f = 0;
    t += dt;
    ++f;

    if (t > 0.5)
    {
        GLuint64 timeStamp[3];

        for(int i=0; i<TIMESTAMPS; ++i)
            glGetQueryObjectui64v(g_queryID[i], GL_QUERY_RESULT, &timeStamp[i]);

        double physics  = (timeStamp[1] - timeStamp[0]) / 1000000.0;
        double render   = (timeStamp[2] - timeStamp[1]) / 1000000.0;

        char title[256];
        sprintf(title, "FPS %i, time (ms): physics %.5f, render %.5f", (int)(f / t), physics, render);
        glfwSetWindowTitle(g_window, title);

        t = 0.0;
        f = 0;
    }
}
