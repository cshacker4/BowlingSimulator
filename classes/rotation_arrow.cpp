#include "rotation_arrow.hpp"

RotationArrow::RotationArrow(VAOStruct* vao, Shader* shader_program, glm::vec3 position){
	this->angle = 0.0f;
	this->position = position;
	ImportOBJ importer;
	this->arrow_shape = importer.loadFiles("models/RotationArrow", *vao);
	this->arrow_vao = vao;
	this->arrow_shader = shader_program;
}
void RotationArrow::Draw(){
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, this->position);
	model = glm::translate(model, glm::vec3(0.0f, 0.15f, 0.0f));
	model = glm::rotate(model, glm::radians(this->angle), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::translate(model, glm::vec3(0.0f, -0.15f, 0.0f));
	this->arrow_shader->use();
	this->arrow_shader->setMat4("model", model);
	this->arrow_shape.Draw();
}
void RotationArrow::ProcessInput(GLFWwindow* window, float deltaTime){
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS){
		this->angle -= rotation_speed * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS){
		this->angle += rotation_speed * deltaTime;
	}
}
float RotationArrow::get_angle_x(){
	return angle;
}
