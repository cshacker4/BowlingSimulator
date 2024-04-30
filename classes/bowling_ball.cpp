#include "bowling_ball.hpp"

BowlingBall::BowlingBall(VAOStruct* vao, Shader* shader_program)
{
	ImportOBJ importer;
	bowling_ball_vao = vao;
	ball_shader = shader_program;
	bowling_ball_shape = importer.loadFiles("models/BowlingBall", *vao);

}
void BowlingBall::ProcessInput(GLFWwindow* window, float deltatime, float angle_y)
{
	//Upon mouse click, the bowling ball will be released
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && !mousePressed) {
		std::cout<<"Mouse pressed"<<std::endl;
		mousePressed = true;
		ball_velocity = glm::rotate(glm::mat4(1.0), glm::radians(angle_y), glm::vec3(0.0,8.0,0.0)) * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	}
	ball_position += (ball_velocity * deltatime);
}
void BowlingBall::Draw()
{
	ball_shader->setBool("is_textured",false);
	ball_shader->setBool("imported_material",true);
	glm::mat4 ball_transform(1.0);
	glm::translate(ball_transform, ball_position);
	glm::mat4 ball_model(1.0);
	ball_transform = glm::translate(ball_transform, ball_position);
	ball_shader->setMat4("model",ball_model);
	ball_shader->setMat4("transform",ball_transform);
	bowling_ball_shape.Draw(*ball_shader);
	ball_shader->setBool("imported_material",false);
}
glm::vec3 BowlingBall::get_position()
{
	return ball_position;
}
glm::vec3 BowlingBall::get_velocity()
{
	return ball_velocity;
}
float BowlingBall::get_radius()
{
	return ball_radius;
}
float BowlingBall::get_weight()
{
	return ball_weight;
}
void BowlingBall::set_velocity(glm::vec3 velocity)
{
	ball_velocity = velocity;
}
