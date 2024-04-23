#include "bowling_pin.hpp"
BowlingPin::BowlingPin(VAOStruct* vao, Shader* shader_program, glm::vec3 position){
	pin_vao = vao;
	pin_shader_program = shader_program;
	ImportOBJ importer;
	pin_shape = importer.loadFiles("models/Pin", *vao);
	pin_position = position;
	pin_radius = 0.03f;
	is_hit = false;
	angle = 0.0f;
}
void BowlingPin::Draw(){
	// Draw the pin
	pin_shader_program->setBool("is_textured",false);
	pin_shader_program->setBool("imported_material",true);
	glm::mat4 avatar_transform(1.0);
	avatar_transform = glm::translate(avatar_transform,pin_position);
	glm::mat4 avatar_model(1.0);
	pin_shader_program->setMat4("model",avatar_model);
	pin_shader_program->setMat4("transform",avatar_transform);
	pin_shape.Draw(*pin_shader_program);
	pin_shader_program->setBool("imported_material",false);

}
void BowlingPin::ProcessInput(GLFWwindow* window, float delta_time){
	pin_position += pin_velocity * delta_time;
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
