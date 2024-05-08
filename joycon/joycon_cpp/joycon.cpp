#include "joycon.hpp"

Joycon::Joycon(){
	zr = false;
	a = false;
	analog_x = 0;
	analog_y = 0;
	gyro_x = 0;
	gyro_y = 0;
	gyro_z = 0;
	accel_x = 0;
	accel_y = 0;
	accel_z = 0;
	read_controller_values();
	last_update_time = std::chrono::steady_clock::now();
}

bool Joycon::get_zr(){
	return zr;
}
bool Joycon::get_a(){
	return a;
}
int Joycon::get_analog_x(){
	return analog_x;
}
int Joycon::get_analog_y(){
	return analog_y;
}
float Joycon::get_gyro_x(){
	return gyro_x;
}
float Joycon::get_gyro_y(){
	return gyro_y;
}
float Joycon::get_gyro_z(){
	return gyro_z;
}
int Joycon::get_accel_x(){
	return accel_x;
}
int Joycon::get_accel_y(){
	return accel_y;
}
int Joycon::get_accel_z(){
	return accel_z;
}
glm::vec3 Joycon::get_velocity(){
	return velocity;
}
glm::vec3 Joycon::get_angular_velocity(){
	return angular_velocity;
}
bool Joycon:: get_roll_registered(){
	return roll_registered;
}
int Joycon::get_updates(){
	return updates;
}
std::chrono::time_point<std::chrono::steady_clock> Joycon::get_last_update_time(){
	return last_update_time;
}
float Joycon::linear_interpolation_angular(float x){
	if (x < -1000.0f){
		return -1.0f;
	}
	if (x > -1000.0f && x < -200.0f){
		return 1/800.0f * x + 0.25f;
	}
	if (x > -200.0f && x < 200.0f){
		return 0.0f;
	}
	if (x > 200.0f && x < 1000.0f){
		return 1/800.0f * x - 0.25f;
	}
	if (x > 1000.0f){
		return 1.0f;
	}
	return 0.0f;
}

float Joycon::linear_interpolation_velocity(float x){
	if (x < -1000.0f){
		return -1.0f;
	}
	if (x > -1000.0f && x < -200.0f){
		return 1/800.0f * x + 0.25f;
	}
	if (x > -200.0f && x < 200.0f){
		return 0.0f;
	}
	if (x > 200.0f && x < 1000.0f){
		return 1/800.0f * x - 0.25f;
	}
	if (x > 1000.0f){
		return 1.0f;
	}
	return 0.0f;
}
float Joycon:: get_velocity_norm(){
	return linear_interpolation_velocity(velocity.y);
}
float Joycon:: get_angular_velocity_norm(){
	return linear_interpolation_angular(angular_velocity.x);
}

void Joycon::read_controller_values() {
    // Define the path to the memory-mapped file
    const char* shared_memory_path = "joycon/tmp/joycon_status.json";

    // Read the JSON string from the memory-mapped file
    std::ifstream file(shared_memory_path);
    if (!file.is_open()) {
	std::cerr << "Failed to open memory-mapped file\n";
	return;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string json_str = buffer.str();

    // Parse the JSON string using RapidJSON
    rapidjson::Document document;
    document.Parse(json_str.c_str());

    // Check if parsing was successful
    if (document.HasParseError()) {
	    std::cerr << "Failed to parse JSON: " << rapidjson::GetParseErrorFunc(document.GetParseError()) << std::endl;
	    return;
    } 

    // Access the JSON object
    if (document.IsObject()) {
	// Access JSON object members
	if (document.HasMember("buttons")){
		zr = document["buttons"]["right"]["zr"].GetInt();
		a = document["buttons"]["right"]["a"].GetInt();
		}
	if (document.HasMember("analog-sticks")){
		if (document["analog-sticks"].HasMember("right")){
			if (document["analog-sticks"]["right"].HasMember("x")){
				analog_x = document["analog-sticks"]["right"]["x"].GetInt();
			}
			if (document["analog-sticks"]["right"].HasMember("y")){
				analog_y = document["analog-sticks"]["right"]["y"].GetInt();

			}
		}
    	} 
	if (document.HasMember("gyro")){
		if (document["gyro"].HasMember("x")){
			gyro_x = document["gyro"]["x"].GetFloat();
		}
		if (document["gyro"].HasMember("y")){
			gyro_y = document["gyro"]["y"].GetFloat();
		}
		if (document["gyro"].HasMember("z")){
			gyro_z = document["gyro"]["z"].GetFloat();
		}
	}
	if (document.HasMember("accel")){
		if (document["accel"].HasMember("x")){
			accel_x = document["accel"]["x"].GetInt();

		}
		if (document["accel"].HasMember("y")){
			accel_y = document["accel"]["y"].GetInt();
		}
		if (document["accel"].HasMember("z")){
			accel_z = document["accel"]["z"].GetInt();
		}
	}
    }
	else {
	std::cerr << "JSON is not an object\n";
	//return;
    }
}

void Joycon::update_controller_values(){
	// arm back
	if (zr && !ZR_pressed){
		ZR_pressed = true;
	}
	// release
	if (!zr && ZR_pressed){
		ZR_pressed = false;
		release = true;
		velocity = total_change_in_velocity;
		angular_velocity = total_angular_velocity/updates;
		roll_registered = true;
	}
	//roll in progress
	if (zr && ZR_pressed){
		total_angular_velocity += glm::vec3(gyro_x, gyro_y, gyro_z);
		total_change_in_velocity += glm::vec3(accel_x, accel_y, accel_z) * current_time_delta;		
	}
}

void Joycon::print_controller_values(){
	std::cout << "ZR: " << zr << std::endl;
	std::cout << "A: " << a << std::endl;
	std::cout << "Analog X: " << analog_x << std::endl;
	std::cout << "Analog Y: " << analog_y << std::endl;
	std::cout << "Gyro X: " << gyro_x << std::endl;
	std::cout << "Gyro Y: " << gyro_y << std::endl;
	std::cout << "Gyro Z: " << gyro_z << std::endl;
	std::cout << "Accel X: " << accel_x << std::endl;
	std::cout << "Accel Y: " << accel_y << std::endl;
	std::cout << "Accel Z: " << accel_z << std::endl;
}

void Joycon::reset(){
	last_update_time = std::chrono::steady_clock::now();
	zr = false;
	a = false;
	analog_x = 0;
	analog_y = 0;
	gyro_x = 0;
	gyro_y = 0;
	gyro_z = 0;
	accel_x = 0;
	accel_y = 0;
	accel_z = 0;
	ZR_pressed = false;
	release = false;
	updates = 0;
	total_angular_velocity = glm::vec3(0, 0, 0);
	total_change_in_velocity = glm::vec3(0, 0, 0);
	velocity = glm::vec3(0, 0, 0);
	roll_registered = false;
}

//difference between current time and last update time
void Joycon::ProcessInput(float time_delta) {
	if (time_delta >= 0.015f) {
		current_time_delta = time_delta;
		read_controller_values();
		update_controller_values();
		last_update_time = std::chrono::steady_clock::now();
		updates++;
	}
}

/**
int main() {
	
	Joycon joycon;
	//print controller values for multiple rolls
	while(true){

		//Handle joycon input
		
		auto current_time = std::chrono::steady_clock::now();
		auto time_delta = std::chrono::duration<float>(current_time - joycon.get_last_update_time()).count();

		joycon.ProcessInput(time_delta);

		if (joycon.get_roll_registered()){
			std::cout << "Velocity y: " << joycon.get_velocity().y << std::endl;
			std::cout << "Angular Velocity x: " << joycon.get_angular_velocity().x << std::endl;
			std::cout << "Velocity: " << std::endl;
			std::cout << joycon.get_velocity_norm() << std::endl;
			std::cout << "Angular Velocity: " << std::endl;
			std::cout << joycon.get_angular_velocity_norm() << std::endl;
			joycon.reset();
		}
	}

}
**/
