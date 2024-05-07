#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "basic_shape.hpp"
#include "import_object.hpp"

class RotationArrow{
	private:
		float angle;
		glm::vec3 position;
		BasicShape arrow_shape;
		VAOStruct* arrow_vao;
		Shader* arrow_shader;
		float rotation_speed = 15.0f;

	public:
		RotationArrow(VAOStruct* vao, Shader* shader_program, glm::vec3 position);
		void Draw();
		void ProcessInput(GLFWwindow* window, float deltaTime);
		float get_angle_x();
};
