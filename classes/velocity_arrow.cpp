#include "velocity_arrow.hpp"

VelocityArrow :: VelocityArrow(VAOStruct* vao, Shader* shader_program)
{
	std::cout << "velocity_arrow constructor called" << std::endl;
	ImportOBJ importer;
	arrow_vao = vao;
	arrow_shader = shader_program;
	arrow_shape = importer.loadFiles("models/VelocityArrow", *vao);

}
void VelocityArrow :: ProcessInput(GLFWwindow* window, float deltatime)
{
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		angle_y += deltatime * angular_speed;
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		angle_y -= deltatime * angular_speed;
	}
}
void VelocityArrow :: Draw()
{
	arrow_shader->setBool("is_textured",false);
	arrow_shader->setBool("imported_material",true);
	glm::mat4 arrow_transform(1.0);
	arrow_shader->setMat4("model", get_model_matrix());
	arrow_shader->setMat4("transform",arrow_transform);
	arrow_shape.Draw(*arrow_shader);
	arrow_shader->setBool("imported_material",false);

}
glm::mat4 VelocityArrow :: get_model_matrix()
{
	glm::mat4 model(1.0);
	glm::vec3 arrow_translate = -1.0f * arrow_position;
	model = glm::translate(model, arrow_translate);
	model = glm::rotate(model, glm::radians(angle_y), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::translate(model, arrow_position);
	return model;
}
