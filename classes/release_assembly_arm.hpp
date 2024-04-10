#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "basic_shape.hpp"
#include "import_object.hpp"

class ReleaseAssemblyArm{
	public:
		ReleaseAssemblyArm(VAOStruct* vao, Shader* shader);
		void Draw();
		void ProcessInput(GLFWwindow* window, float deltaTime);
	private:
		float angle_x;
		float angular_velocity;
		glm::vec3 position;
		Shader* arm_shader;
		VAOStruct* arm_vao;
		BasicShape arm_shape;
		glm::mat4 get_model_matrix();

};
