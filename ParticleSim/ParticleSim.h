#pragma once
#include <cstdint>

namespace ps
{
	enum class ERROR
	{
		SHADER_ERROR,
		FILE_NOT_FOUND,
		GLEW,
		WINDOW,
		UNKNOWN,
		SUCCESS
	};

	enum class SIM_TYPE
	{
		POINTS,
		TEXTURED_QUADS
	};

	struct Window;
	struct ErrorData
	{
		ERROR error_type;
		const char* message;
	};

	/**
	* Initialises the GLFW window and OpenGL context 
	*/
	ErrorData init(uint16_t width, uint16_t height, Window* window);
	
	/**
	* Configue OpenGL for the kind of rendering desired
	*/
	ErrorData configure_opengl(Window* window, SIM_TYPE sim_type);

	ErrorData loop();
	ErrorData shutdown(Window* window);
}