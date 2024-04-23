#include "release_assembly_arm.hpp"

ReleaseAssemblyArm::ReleaseAssemblyArm(VAOStruct* vao, Shader* shader){
	angle_x = 0.0f;
	angular_velocity = 10.0f;
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
	glm::mat4 avatar_model(1.0);
	arm_shader->setMat4("model",get_model_matrix());
	arm_shader->setMat4("transform",avatar_transform);
	arm_shape.Draw(*arm_shader);
	arm_shader->setBool("imported_material",false);

}
void ReleaseAssemblyArm::ProcessInput(GLFWwindow* window, float deltaTime){
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		angle_x += angular_velocity * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		angle_x -= angular_velocity * deltaTime;
}
glm::mat4 ReleaseAssemblyArm::get_model_matrix(){
	glm::mat4 model(1.0);
	glm::vec3 translate_to_origin = -1.0f * position;
	model = glm::translate(model, position);
	model = glm::rotate(model, glm::radians(angle_x), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::translate(model, translate_to_origin);
	return model;
}
