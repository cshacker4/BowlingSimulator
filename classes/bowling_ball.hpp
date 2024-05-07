#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "basic_shape.hpp"
#include "import_object.hpp"

class BowlingBall
{
	public:
		BowlingBall(VAOStruct* vao, Shader* shader_program);
		void ProcessInput(GLFWwindow* window, float deltatime);
		void RegisterClick();
		void Draw();
		glm::vec3 get_position();
		glm::vec3 get_velocity();
		float get_radius();
		float get_weight();
		void set_velocity(glm::vec3 velocity);
		bool get_in_lane();
		void reset();
		void set_release(bool release);
		void set_rotational_velocity(float velocity);

	private:
		bool mousePressed = false;
		bool in_lane = true;
		BasicShape bowling_ball_shape;
		VAOStruct* bowling_ball_vao;
		Shader* ball_shader;
		float ball_radius = 0.11f;
		float ball_weight = 12.0f;
		glm::vec3 initial_position = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 ball_position = initial_position;
		glm::vec3 ball_velocity_normal = glm::vec3(0.0f, 0.0f, 0.0f);
		float ball_speed = 5.0f;
		glm::vec3 ball_velocity = glm::vec3(0.0f, 0.0f, 0.0f); 
		float angular_velocity_normal = 0.0f;
		float angular_speed = 5.0f;
		bool release = false;
		float oil_line = 10.00f;

};
