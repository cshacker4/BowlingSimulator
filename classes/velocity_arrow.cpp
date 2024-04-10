#include "velocity_arrow.hpp"

VelocityArrow :: VelocityArrow(VAOStruct* vao, Shader* shader_program)
{
	std::cout << "velocity_arrow constructor called" << std::endl;
	ImportOBJ importer;
	arrow_vao = vao;
	arrow_shader = shader_program;
	arrow_shape = importer.loadFiles("models/VelocityArrow", *vao);

}
void VelocityArrow :: ProcessInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		angle_z += 0.1f;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		angle_z -= 0.1f;
	}
	std::cout << "angle_z: " << angle_z << std::endl;
}
void VelocityArrow :: Draw()
{
	arrow_shape.Draw(*arrow_shader);
	std::cout << "Drawing velocity_arrow" << std::endl;
}
