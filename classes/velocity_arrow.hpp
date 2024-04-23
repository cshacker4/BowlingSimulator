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
		VelocityArrow(VAOStruct* vao, Shader* shader_program, glm::vec3 position);
		void ProcessInput(GLFWwindow* window, float deltatime);
		void Draw();
		float get_angle_y();
	private:
		BasicShape arrow_shape;
		VAOStruct* arrow_vao;
		Shader* arrow_shader;
		glm::vec3 arrow_position;
		float angular_speed = 15.0f;
		float angle_y = 0.0f;
		glm::mat4 get_model_matrix();
};
