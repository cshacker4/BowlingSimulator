#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "basic_shape.hpp"
#include "import_object.hpp"
#include "game_state.hpp"

class ReleaseAssemblyArm{
	public:
		ReleaseAssemblyArm(VAOStruct* vao, Shader* shader);
		void Draw();
		void ProcessInput(GLFWwindow* window, float deltaTime, GameState* game_state);
	private:
		float angle_x = 90.0f;
		float angular_velocity = 20.0f;
		glm::vec3 position = glm::vec3(0.5f, 0.5f, 18.5f);
		Shader* arm_shader;
		VAOStruct* arm_vao;
		BasicShape arm_shape;
};

