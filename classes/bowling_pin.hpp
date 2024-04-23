#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "basic_shape.hpp"
#include "import_object.hpp"

class BowlingPin{
	public:
		BowlingPin(VAOStruct* vao, Shader* shader_program, glm::vec3 position);
		void Draw();
		void ProcessInput(GLFWwindow* window, float delta_time);
		glm::vec3 get_position();
		float get_radius();
		bool get_is_hit();
		void set_is_hit(bool hit);
		float get_weight();
		void set_velocity(glm::vec3 velocity);
		glm::vec3 get_velocity();
	private:
		Shader* pin_shader_program;
		VAOStruct* pin_vao;
		BasicShape pin_shape;
		glm::vec3 pin_position;
		glm::vec3 pin_velocity = glm::vec3(0.0f, 0.0f, 0.0f);
		float pin_radius;
		float pin_weight = 1.5f;
		bool is_hit;
		float angle;
		glm::mat4 get_transform_matrix();
};

