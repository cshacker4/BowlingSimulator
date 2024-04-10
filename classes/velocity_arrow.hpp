#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "basic_shape.hpp"
#include "import_object.hpp"

class VelocityArrow
{
	public:
		//VelocityArrow(ImportObject* obj);
		VelocityArrow(VAOStruct* vao, Shader* shader_program);
		void ProcessInput(GLFWwindow* window, float deltatime);
		void Draw();
	private:
		BasicShape arrow_shape;
		VAOStruct* arrow_vao;
		Shader* arrow_shader;
		glm::vec3 arrow_position = glm::vec3(-0.0019, -0.0799, 0.0759);
		float angular_speed = 2.0f;
		float angle_y = 0.0f;
		glm::mat4 get_model_matrix();
};
