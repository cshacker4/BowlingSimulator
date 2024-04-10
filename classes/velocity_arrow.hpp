#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "basic_shape.hpp"
#include "import_object.hpp"

class VelocityArrow
{
	public:
		//VelocityArrow(ImportObject* obj);
		VelocityArrow(VAOStruct* vao, Shader* shader_program);
		void ProcessInput(GLFWwindow* window);
		void Draw();
	private:
		BasicShape arrow_shape;
		VAOStruct* arrow_vao;
		Shader* arrow_shader;
		float angle_z = 0.0f;
};
