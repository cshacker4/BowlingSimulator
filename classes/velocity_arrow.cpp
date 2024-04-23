#include "velocity_arrow.hpp"

VelocityArrow :: VelocityArrow(VAOStruct* vao, Shader* shader_program, glm::vec3 position)
{
	std::cout << "velocity_arrow constructor called" << std::endl;
	ImportOBJ importer;
	arrow_vao = vao;
	arrow_shader = shader_program;
	arrow_position = position;
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
	glm::mat4 arrow_transform = glm::mat4(1.0f);
	arrow_transform = glm::mat4(1.0f);
	glm::mat4 arrow_model = get_model_matrix();
	arrow_shader->setMat4("model", arrow_model);
	arrow_shader->setMat4("transform",arrow_transform);
	arrow_shape.Draw(*arrow_shader);
	arrow_shader->setBool("imported_material",false);

}
glm::mat4 VelocityArrow :: get_model_matrix()
{
	glm::mat4 model(1.0f);
	//Translate to origin - if not at origin
	model = glm::translate(model, glm::vec3(0.0f, 0.1f, 0.0f));
	//Rotate
	model = glm::rotate(model, glm::radians(angle_y), glm::vec3(0.0f, 1.0f, 0.0f));
	//Translate
	model = glm::translate(model, arrow_position);
	std::cout << arrow_position.x << " " << arrow_position.y << " " << arrow_position.z << std::endl;
	return model;
}

float VelocityArrow :: get_angle_y()
{
	return angle_y;
}
