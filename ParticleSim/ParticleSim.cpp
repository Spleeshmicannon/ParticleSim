// ParticleSim.cpp : Defines the functions for the static library.
//
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include "ParticleSim.h"

#include <string>


namespace ps
{
	struct Window
	{
		uint16_t width, height;
		GLuint program, vao, vbo;
		GLFWwindow* glfw_window;
		SIM_TYPE sim_type;
	};

	ErrorData init(uint16_t width, uint16_t height, Window* window)
	{
		// GLFW initialisation
		glfwInit();
		
		window = new Window;

		window->width = width;
		window->height = height;
		
		window->glfw_window = glfwCreateWindow(width, height, "Particle Simulation", NULL, NULL);
		
		if (!window->glfw_window)
		{
			delete window;
			return { ERROR::WINDOW, "glfw window could not be created" };
		}

		glfwMakeContextCurrent(window->glfw_window);

		// OpenGL initialisation
		const GLint res = glewInit();

		if (res != GLEW_OK)
		{
			glfwDestroyWindow(window->glfw_window);
			delete window;
			return { ERROR::GLEW, (const char*)glewGetErrorString(res) };
		}

		return { ERROR::SUCCESS, "" };
	}

	ErrorData configure_opengl(Window* window, SIM_TYPE sim_type)
	{
		if (sim_type == SIM_TYPE::TEXTURED_QUADS)
		{
			// Shader initialisation
			const std::string vertex_shader =
				"#version 330 core"
				"layout(location = 0) in vec2 aPos;"
				"layout(location = 1) in vec3 aColor;"
				"layout(location = 2) in vec2 in_tex_coords;"
				"out vec4 color;"
				"out vec2 tex_coords;"
				"uniform mat4 matrix;"
				"void main()"
				"{"
				"	gl_Position = vec4(aPos, 0f, 1.0f) * matrix;"
				"	color = vec4(aColor, 1.0);"
				"	tex_coords = in_tex_coords;"
				"}";

			const std::string fragment_shader =
				"#version 330 core"
				"out vec4 FragColor;"
				"in vec4 color;"
				"in vec2 tex_coords;"
				"uniform sampler2D texture_data;"
				"void main()"
				"{"
				"	FragColor = texture(texture_data, tex_coords) * color;"
				"}";

			GLuint id_vertex_shader, id_fragment_shader;

			compile_shader(vertex_shader, &id_vertex_shader, GL_VERTEX_SHADER);
			compile_shader(fragment_shader, &id_fragment_shader, GL_FRAGMENT_SHADER);

			window->program = glCreateProgram();

			glAttachShader(window->program, id_vertex_shader);
			glAttachShader(window->program, id_fragment_shader);

			glLinkProgram(window->program);

			GLint isLinked = 0;
			glGetProgramiv(window->program, GL_LINK_STATUS, (int*)&isLinked);

			if (isLinked == GL_FALSE)
			{
				glDeleteProgram(window->program);
				return { ERROR::SHADER_ERROR, "Failed to compile or link shaders, likely a bug" };
			}

			glUseProgram(window->program);

			// configuring OpenGL
			glGenVertexArrays(1, &window->vao);
			glGenBuffers(1, &window->vbo);

			glBindVertexArray(window->vao);
			glBindBuffer(GL_ARRAY_BUFFER, window->vbo);

			glm::mat4 matrix =
				glm::ortho<float>
				(
					0, window->width,
					0, window->height
					) *
				glm::lookAt
				(
					glm::vec3(-1.5, -1, 2),
					glm::vec3(-0.75, -1, -1),
					glm::vec3(0, 1, 0)
				);

			glUniformMatrix4fv(
				glGetUniformLocation(window->program, "matrix"),
				1,
				GL_FALSE,
				(GLfloat*)&matrix[0][0]
			);

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
		else
		{
			glGenBuffers(1, &window->vbo);
			glBindBuffer(GL_ARRAY_BUFFER, window->vbo);
			gluOrtho2D(0, window->width, 0, window->height);
		}

		window->sim_type = sim_type;

		return { ERROR::SUCCESS, "" };
	}

	ErrorData loop()
	{

	}
	
	ErrorData shutdown(Window* window)
	{
		glfwDestroyWindow(window->glfw_window);
		delete window;
	}

	void compile_shader(const std::string& name, GLuint* shader, GLuint type)
	{
		*shader = glCreateShader(type);
		const char* shaders[1] = { name.c_str() };
		const int size = name.size();
		glShaderSource(*shader, 1, shaders, &size);
		glCompileShader(*shader);

		GLint success = 0;
		glGetShaderiv(*shader, GL_COMPILE_STATUS, &success);
	}
}