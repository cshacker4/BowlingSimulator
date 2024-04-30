#include "bowling_pin.hpp"
BowlingPin::BowlingPin(VAOStruct* vao, Shader* shader_program, glm::vec3 position){
	pin_vao = vao;
	pin_shader_program = shader_program;
	ImportOBJ importer;
	pin_shape = importer.loadFiles("models/Pin", *vao);
	pin_position = position;
	is_hit = false;
}
void BowlingPin::Draw(){
	// Draw the pin
	pin_shader_program->setBool("is_textured",false);
	pin_shader_program->setBool("imported_material",true);
	//ignore transform matrix
	glm::mat4 avatar_transform = glm::mat4(1.0f);
	glm::mat4 trans = glm::translate(glm::mat4(1.0f),pin_position);
	glm::mat4 rotate = glm::rotate(glm::mat4(1.0), glm::radians(angle), glm::vec3(1.0f,0.0f,0.0f));
	glm::mat4 translate_up = glm::translate(glm::mat4(1.0f),glm::vec3(0.0f,glm::sin(glm::radians(angle))*pin_radius, 0.0f));
	glm::mat4 model = translate_up * trans * rotate;

	pin_shader_program->setMat4("model", model);
	pin_shader_program->setMat4("transform",avatar_transform);
	pin_shape.Draw(*pin_shader_program);
	pin_shader_program->setBool("imported_material",false);

}
void BowlingPin::ProcessInput(GLFWwindow* window, float delta_time){
	//move
	pin_position += pin_velocity * delta_time;
	//rotate
	if (is_hit && angle < 90.0f){
		angular_velocity += angular_acceleration * delta_time;
		angle += angular_velocity * delta_time;

	}
}

glm::vec3 BowlingPin::get_position(){
	return pin_position;
}
float BowlingPin::get_radius(){
	return pin_radius;
}
bool BowlingPin::get_is_hit(){
	return is_hit;
}
void BowlingPin::set_is_hit(bool hit){
	is_hit = hit;
}
float BowlingPin::get_weight(){
	return pin_weight;
}
void BowlingPin::set_velocity(glm::vec3 velocity){
	// Do nothing
	pin_velocity = velocity;
}
glm::vec3 BowlingPin::get_velocity(){
	return pin_velocity;
}
