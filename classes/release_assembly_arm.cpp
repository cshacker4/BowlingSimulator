#include "release_assembly_arm.hpp"

ReleaseAssemblyArm::ReleaseAssemblyArm(VAOStruct* vao, Shader* shader){
	arm_shader = shader;
	arm_vao = vao;
	//Convert form Blender to OpenGL
	ImportOBJ importer;
	arm_shape = importer.loadFiles("models/ReleaseAssemblyArm", *arm_vao);
}
void ReleaseAssemblyArm::Draw(){
	arm_shader->setBool("is_textured",false);
	arm_shader->setBool("imported_material",true);
	glm::mat4 avatar_transform(1.0);
	glm::mat4 translate_to_origin = glm::translate(glm::mat4(1.0), -1.0f * position);
	glm::mat4 rotate = glm::rotate(glm::mat4(1.0), glm::radians(angle_x), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 translate_back = glm::translate(glm::mat4(1.0), position);
	glm::mat4 avatar_model =  translate_back * rotate * translate_to_origin * glm::mat4(1.0);
	arm_shader->setMat4("model",avatar_model);
	arm_shader->setMat4("transform",avatar_transform);
	arm_shape.Draw(*arm_shader);
	arm_shader->setBool("imported_material",false);

}
void ReleaseAssemblyArm::ProcessInput(GLFWwindow* window, float deltaTime, GameState* game_state){
	if (game_state->get_state() == LOWER_ARM ){
		if (angle_x > 0.0f){
			angle_x -= angular_velocity * deltaTime;
		}
		else{
			game_state->change_state(RESET);
		}
	}
	if (game_state->get_state() == RAISE_ARM){
		if (angle_x <= 90.0f){
			angle_x += angular_velocity * deltaTime;
		}
		else{
			game_state->change_state(VELOCITY_ARROW);
		}
	}
}
