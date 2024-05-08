#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include "rapidjson/document.h"
#include <glm/glm.hpp>
#include <fstream>      // For std::ifstream
#include <sstream>      // For std::stringstream
#include <cmath>

class Joycon{
	public:
		Joycon();
		bool get_zr();
		bool get_a();
		int get_analog_x();
		int get_analog_y();
		float get_gyro_x();
		float get_gyro_y();
		float get_gyro_z();
		int get_accel_x();
		int get_accel_y();
		int get_accel_z();
		glm::vec3 get_angular_velocity();
		glm::vec3 get_velocity();
		void ProcessInput(float current_time);
		void print_controller_values();
		void reset();
		bool get_roll_registered();
		int get_updates();
		//number between -1 and 1
		float get_angular_velocity_norm();
		float get_velocity_norm();
		std::chrono::time_point<std::chrono::steady_clock> get_last_update_time();
	private:
		std::chrono::time_point<std::chrono::steady_clock> last_update_time;
		bool zr;
		bool a;
		int analog_x;
		int analog_y;
		float gyro_x;
		float gyro_y;
		float gyro_z;
		int accel_x;
		int accel_y;
		int accel_z;
		bool ZR_pressed = false;
		bool release = false;
		float updates = 0;
		glm::vec3 total_angular_velocity = glm::vec3(0, 0, 0);
		glm::vec3 angular_velocity = glm::vec3(0, 0, 0);
		glm::vec3 velocity = glm::vec3(0, 0, 0);
		bool roll_registered = false;
		float current_time_delta;
		glm::vec3 total_change_in_velocity = glm::vec3(0, 0, 0);
		float sigmoid(float x);
		float s_curve(float x);
		void read_controller_values();
		void update_controller_values();
		float linear_interpolation_angular(float x);
		float linear_interpolation_velocity(float x);

};
