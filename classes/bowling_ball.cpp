#include "bowling_ball.hpp"

BowlingBall::BowlingBall(VAOStruct* vao, Shader* shader_program)
{
	ImportOBJ importer;
	bowling_ball_vao = vao;
	ball_shader = shader_program;
	bowling_ball_shape = importer.loadFiles("models/BowlingBall", *vao);

}
void BowlingBall::ProcessInput(GLFWwindow* window, float deltatime)
{
	//std::cout << in_lane << release << std::endl;
	if (in_lane && release) {

		if (ball_position.z > oil_line) {
			//assume decceleration is negligible
			//negative due to world orientation
			ball_velocity.x -= (angular_velocity_normal * angular_speed * ball_radius * deltatime);
		}
		ball_position += ball_velocity * deltatime;
	}
	if (ball_position.x < -0.55f || ball_position.x > 0.55f || ball_position.z > 20.07f) {
		release = false;
		in_lane = false;
	}
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
	return ball_velocity_normal;
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
	ball_velocity_normal = velocity;
	ball_velocity = ball_velocity_normal * ball_speed;
}
bool BowlingBall::get_in_lane()
{
	return in_lane;
}
void BowlingBall::reset()
{
	in_lane = true;
	ball_position = initial_position;
	ball_velocity_normal = glm::vec3(0.0f, 0.0f, 0.0f);
}
void BowlingBall::set_release(bool released)
{
	release = released;
}
void BowlingBall::set_rotational_velocity(float velocity)
{
	angular_velocity_normal = velocity;
}
