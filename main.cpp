#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "support/environment_setup.hpp"
#include "support/build_shapes.hpp"
#include "classes/camera.hpp"
#include "fonts/Font.hpp"
#include "classes/import_object.hpp"
#include "classes/velocity_arrow.hpp"
#include "classes/release_assembly_arm.hpp"
#include "classes/bowling_ball.hpp"
#include "classes/bowling_pin.hpp"
#include "classes/game_state.hpp"
#include "classes/rotation_arrow.hpp"
#include "joycon/joycon_cpp/joycon.hpp"

/*ProcessInput
 * Accepts a GLFWwindow pointer as input and processes 
 * user key and mouse button inpbut.  
 * Returns nothing.
 */
void ProcessInput(GLFWwindow *window);

/*mouse_callback
 *Accepts a GLFWwindow pointer, and a double for the xpos and the ypos.  
 *Invoked when the mouse is moved once it is set using the glfwSetCursorPosCallback
 * function.  
 */
void mouse_callback(GLFWwindow* window, double xpos, double ypos);


//Global Variables
//----------------
//const unsigned int SCR_WIDTH = 800;
//const unsigned int SCR_HEIGHT = 800;
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

glm::vec4 shape_color (0.0,0.0,0.0,1.0);
glm::vec4 clear_color (0.0,0.0,0.0,1.0);

glm::vec2 world_min (-1.0,-1.0);
glm::vec2 world_max (1.0,1.0);

float rotate_speed = 90.0;

//Global Variables
//-------------
bool first_mouse{true};
float last_x{0.0};
float last_y{0.0};
bool cameraToggle = false;
bool lKeyPressed = false;
bool lightToggle = false;
//display text with toggle key
bool display_text = false;
bool display_first_press = true;
//toggle boolean for the arrow input
bool firstAToggle = true;

//Television Screen state
//---------------
glm::mat4 television_transform = glm::translate(glm::mat4(1.0),glm::vec3(0.0f,0.0f,1.0f));

//Camera Object
Camera camera(glm::vec3(0.0f,1.0f,-3.0f),glm::vec3(0.0f,1.0f,0.0f),90.0f,0.0f);

float delta_time{0.001};
float last_frame{0.0};

//Joycon
Joycon joycon;

//light switches
glm::vec4 blue_light_position (0.0,5.0,10.0,1.0);
glm::vec4 white_light_position (0.0,5.0,-3.0,1.0);


Font arial_font ("fonts/ArialBlackLarge.bmp","fonts/ArialBlack.csv",0.1,0.15);

//Point light structure
struct PointLight {
	glm::vec4 position;
	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;

    //add coefficients for attenuation here
    bool on;
};

PointLight blue_light{
	blue_light_position,
		glm::vec4(0.0f, 0.0f, 0.1f, 1.0f), 
		glm::vec4(0.2f, 0.2f, 1.0f, 1.0f),
		glm::vec4(0.0f, 0.0f, 1.0f,1.0f),
		true
};
PointLight white_light{
	white_light_position,
		glm::vec4(0.0f, 0.0f, 0.1f, 1.0f), 
		glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
		glm::vec4(1.0f, 1.0f, 1.0f,1.0f),
		true
};

struct Spotlight {
	glm::vec4 position;
	glm::vec4 direction;
	glm::vec4 color;
	float cut_off;
};

Spotlight spotlight = {glm::vec4(0.0,3.0,19.0, 1.0),
	glm::vec4(0.0,-1.0, 0.0, 1.0),
	glm::vec4(1.0,1.0,1.0,1.0),
	12.5};


//Handle Collision
//----------------
//Created with the assistance of ChatGPT: https://chat.openai.com/share/1f6799d8-e332-458d-a9f5-bd91ba19cb59
void calculate_ball_pin_velocity(BowlingBall* ball, BowlingPin* pin) {
	// Calculate new velocities using conservation of momentum and kinetic energy
	float pin_weight = pin->get_weight();
	float ball_weight = ball->get_weight();
	glm::vec3 pin_position = pin->get_position();
	glm::vec3 ball_position = ball->get_position();
	//ball y position is the same as pin y position
	ball_position.y = pin_position.y;
	//Vector normal to the collision plane
	glm::vec3 normal = glm::normalize(pin_position - ball_position);
	// Relative velocity of the ball with respect to the pin
	glm::vec3 relative_velocity = pin->get_velocity() - ball->get_velocity();
	// Calculate final velocities using conservation of momentum
	glm::vec3 ball_velocity = ball->get_velocity() + (2 * pin_weight / (pin_weight + ball_weight)) * glm::dot(relative_velocity, normal) * normal;
	glm::vec3 pin_velocity = pin->get_velocity() - (2 * ball_weight / (pin_weight + ball_weight)) * glm::dot(relative_velocity, normal) * normal;
	// Update velocities of both objects
	ball->set_velocity(ball_velocity);
	pin->set_velocity(pin_velocity);
}
// Make a function to handle collision between any two objects
void calculate_velocity_after_collision(BowlingPin* pin1, BowlingPin* pin2) {
	// Calculate new velocities using conservation of momentum and kinetic energy
	float pin1_weight = pin1->get_weight();
	float pin2_weight = pin2->get_weight();
	glm::vec3 pin1_position = pin1->get_position();
	glm::vec3 pin2_position = pin2->get_position();
	//Vector normal to the collision plane
	glm::vec3 normal = glm::normalize(pin1_position - pin2_position);
	// Relative velocity of the ball with respect to the pin
	glm::vec3 relative_velocity = pin2->get_velocity() - pin1->get_velocity();
	// Calculate final velocities using conservation of momentum
	glm::vec3 pin1_velocity = pin1->get_velocity() + (2 * pin2_weight / (pin1_weight + pin2_weight)) * glm::dot(relative_velocity, normal) * normal;
	glm::vec3 pin2_velocity = pin2->get_velocity() - (2 * pin1_weight / (pin1_weight + pin2_weight)) * glm::dot(relative_velocity, normal) * normal;
	// Update velocities of both objects
	pin1->set_velocity(pin1_velocity);
	pin2->set_velocity(pin2_velocity);
}

void handle_ball_pin_collision(BowlingBall* ball, BowlingPin* pin) {
	glm::vec3 ball_position = ball->get_position();
	glm::vec3 pin_position = pin->get_position();
	float ball_radius = ball->get_radius();
	float pin_radius = pin->get_radius();
	float distance = glm::distance(ball_position, pin_position);

	if (distance < ball_radius + pin_radius) {
		if (!pin->get_is_hit()) {
			pin->set_is_hit(true);
			calculate_ball_pin_velocity(ball, pin);
		}
		pin->set_is_hit(true);
	}
}

void handle_pin_pin_collision(BowlingPin* pin1, BowlingPin* pin2) {
	glm::vec3 pin1_position = pin1->get_position();
	glm::vec3 pin2_position = pin2->get_position();
	float pin1_radius = pin1->get_radius();
	float pin2_radius = pin2->get_radius();
	float distance = glm::distance(pin1_position, pin2_position);

	if (distance < pin1_radius + pin2_radius) {
		calculate_velocity_after_collision(pin1, pin2);
		pin1->set_is_hit(true);
		pin2->set_is_hit(true);
	}
}

//Draw text on screen
void draw_text(Shader* shader, Font* font, glm::vec3 camera_position, glm::vec3 ball_position, int pins_standing) {
	glm::mat4 model = glm::mat4(1.0);
	shader->use();
	shader->setMat4("model", model);
	std::string camera_position_str = "Camera (";
	std::string cam_x = std::to_string(camera_position.x);
	std::string cam_y = std::to_string(camera_position.y);
	std::string cam_z = std::to_string(camera_position.z);

	camera_position_str += cam_x.substr(0,cam_x.find(".")+3) +",";
	camera_position_str += cam_y.substr(0,cam_y.find(".")+3) +",";
	camera_position_str += cam_z.substr(0,cam_z.find(".")+3) +")";

	std::string ball_position_str = "Ball (";
	std::string ball_x = std::to_string(ball_position.x);
	std::string ball_y = std::to_string(ball_position.y);
	std::string ball_z = std::to_string(ball_position.z);

	ball_position_str += ball_x.substr(0,ball_x.find(".")+3) +",";
	ball_position_str += ball_y.substr(0,ball_y.find(".")+3) +",";
	ball_position_str += ball_z.substr(0,ball_z.find(".")+3) +")";

	std::string pins_standing_str = "Pins standing: ";
	pins_standing_str += std::to_string(pins_standing);

	arial_font.DrawRect(*shader);
	arial_font.DrawText(camera_position_str,glm::vec2(-1.0f, 0.8f),*shader);
	arial_font.DrawText(ball_position_str,glm::vec2(-1.0f, 0.6f),*shader);
	arial_font.DrawText(pins_standing_str,glm::vec2(-1.0f, 0.4f),*shader);

}

int main()
{
    GLFWwindow *window = InitializeEnvironment(SCR_WIDTH,SCR_HEIGHT,"CS473");
    if (window == NULL) {
        std::cout<<"Failed to initialize GLFWwindow"<<std::endl;
        return -1;
    }

    last_x = (1.0*SCR_WIDTH)/2.0f;
    last_y = (1.0*SCR_HEIGHT)/2.0f;

    glfwSetCursorPosCallback(window, mouse_callback); 
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  
    //Create the shader program
    //-------------------------
    Shader shader_program(".//shaders//vertex.glsl",".//shaders//fragment.glsl");
    Shader font_program(".//shaders//vertex.glsl",".//shaders//fontFragmentShader.glsl");
    Shader skybox_shader(".//shaders//skyBoxVertex.glsl",".//shaders//skyBoxFragment.glsl");

    // Create Game State
    GameState game_state;

    //Create the VAO
    //-------------------------

    VAOStruct vao; 
    //Handles location (x,y,z) and surface normal (snx,sny,snz)
    glGenVertexArrays(1,&(vao.id));
    vao.attributes.push_back(BuildAttribute(3,GL_FLOAT,false,6*sizeof(float),0));
    vao.attributes.push_back(BuildAttribute(3,GL_FLOAT,false,6*sizeof(float),3*sizeof(float)));
    

    VAOStruct texture_vao;
    //Handles:
    //  location (x,y,z)
    //  surface normal (snx,sny,snz)
    //  texture coordinates (s,t)
    glGenVertexArrays(1,&(texture_vao.id));
    texture_vao.attributes.push_back(BuildAttribute(3,GL_FLOAT,false,8*sizeof(float),0));
    texture_vao.attributes.push_back(BuildAttribute(3,GL_FLOAT,false,8*sizeof(float),3*sizeof(float)));
    texture_vao.attributes.push_back(BuildAttribute(2,GL_FLOAT,false,8*sizeof(float),6*sizeof(float)));

    VAOStruct import_vao;
    //Handles:
    //  location (x,y,z)
    //  surface normal (snx,sny,snz)
    //  texture coordinates (s,t)
    //  diffuse color (r,g,b)
    //  specular color (r,g,b)
    glGenVertexArrays(1,&(import_vao.id));
    import_vao.attributes.push_back(BuildAttribute(3,GL_FLOAT,false,14*sizeof(float),0));
    import_vao.attributes.push_back(BuildAttribute(3,GL_FLOAT,false,14*sizeof(float),3*sizeof(float)));
    import_vao.attributes.push_back(BuildAttribute(2,GL_FLOAT,false,14*sizeof(float),6*sizeof(float)));
    import_vao.attributes.push_back(BuildAttribute(3,GL_FLOAT,false,14*sizeof(float),8*sizeof(float)));
    import_vao.attributes.push_back(BuildAttribute(3,GL_FLOAT,false,14*sizeof(float),11*sizeof(float)));

    //Create the skybox VAO
    VAOStruct skybox_vao;
    //Handles location (x,y,z)
    glGenVertexArrays(1,&(skybox_vao.id));
    skybox_vao.attributes.push_back(BuildAttribute(3,GL_FLOAT,false,3*sizeof(float),0));

    ImportOBJ importer;

    BasicShape bowling_lane = importer.loadFiles("models/BowlingLane",import_vao);
    int bowling_lane_texture = importer.getTexture();
    BowlingBall bowling_ball(&import_vao, &shader_program);
    VelocityArrow velocity_arrow(&import_vao, &shader_program, bowling_ball.get_position());
    RotationArrow rotation_arrow(&import_vao, &shader_program, bowling_ball.get_position());
    ReleaseAssemblyArm release_assembly_arm(&import_vao, &shader_program);
    BasicShape television_shape = importer.loadFiles("models/Television",import_vao);


    //Create bowling pins
    //-------------------------
    float bowling_pins_length = 0.15;
    glm::vec3 bowling_pins_position = glm::vec3(0.0, 0.0, 19.0);
    BowlingPin bowling_pins[10]{
	    BowlingPin(&import_vao, &shader_program, bowling_pins_position),
	    BowlingPin(&import_vao, &shader_program, bowling_pins_position + glm::vec3(bowling_pins_length, 0.0, bowling_pins_length)),
	    BowlingPin(&import_vao, &shader_program, bowling_pins_position + glm::vec3(-bowling_pins_length, 0.0, bowling_pins_length)),
	    BowlingPin(&import_vao, &shader_program, bowling_pins_position + glm::vec3(2*bowling_pins_length, 0.0, 2* bowling_pins_length)),
	    BowlingPin(&import_vao, &shader_program, bowling_pins_position + glm::vec3(0.0, 0.0, 2*bowling_pins_length)),
	    BowlingPin(&import_vao, &shader_program, bowling_pins_position + glm::vec3(-2*bowling_pins_length, 0.0, 2*bowling_pins_length)),
	    BowlingPin(&import_vao, &shader_program, bowling_pins_position + glm::vec3(3*bowling_pins_length, 0.0, 3*bowling_pins_length)),
	    BowlingPin(&import_vao, &shader_program, bowling_pins_position + glm::vec3(bowling_pins_length, 0.0, 3*bowling_pins_length)),
	    BowlingPin(&import_vao, &shader_program, bowling_pins_position + glm::vec3(-bowling_pins_length, 0.0, 3*bowling_pins_length)),
	    BowlingPin(&import_vao, &shader_program, bowling_pins_position + glm::vec3(-3*bowling_pins_length, 0.0, 3*bowling_pins_length))
    };
    int pins_standing = 10;

    arial_font.initialize(texture_vao);

    //Create the shapes
    //-------------------------
    
    glm::mat4 model(1.0);
    glm::mat4 view(1.0);
    glm::mat4 projection(1.0);
    glm::mat4 identity_matrix(1.0);
    
    shader_program.use();

    shader_program.setMat4("model",model);
    shader_program.setMat4("view",view);
    projection = glm::perspective(glm::radians(45.0f),(1.0f*SCR_WIDTH)/(1.0f*SCR_HEIGHT),0.1f,100.0f);
    shader_program.setMat4("projection",projection);

    //Set up the depth test
    glEnable(GL_DEPTH_TEST);

    //lighting 
    //---------
    PointLight point_light_list[10];

    point_light_list[0] = blue_light;
    point_light_list[1] = white_light;

    int num_lights = sizeof(point_light_list)/sizeof(PointLight);

    shader_program.setVec4("spotlight.position", spotlight.position);
    shader_program.setVec4("spotlight.direction", spotlight.direction);
    shader_program.setVec4("spotlight.color", spotlight.color);
    shader_program.setFloat("spotlight.cut_off", spotlight.cut_off);

    shader_program.setVec4("view_position",glm::vec4(camera.Position,1.0));

    //font shader settings
    font_program.use();
    font_program.setMat4("transform",identity_matrix);
    font_program.setMat4("model",identity_matrix);
    font_program.setMat4("view",identity_matrix);
    font_program.setMat4("projection",glm::ortho(-1.0,1.0,-1.0,1.0,-1.0,1.0));
    font_program.setVec4("transparentColor", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    font_program.setFloat("alpha", 0.0f);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //Create the skybox
    //-------------------------
    std::vector<std::string> faces
    {
	    /*
	       "./textures/skybox/right.jpg",
	       "./textures/skybox/left.jpg",
	       "./textures/skybox/top.jpg",
	       "./textures/skybox/bottom.jpg",
	       "./textures/skybox/front.jpg",
	       "./textures/skybox/back.jpg"
	       */
	    "./textures/roombox/px.png",
		    "./textures/roombox/nx.png",
		    "./textures/roombox/py.png",
		    "./textures/roombox/ny.png",
		    "./textures/roombox/pz.png",
		    "./textures/roombox/nz.png"

    };
    unsigned int cubemapTexture = GetCubeMap(faces); 
    BasicShape skybox = GetCube(skybox_vao);

    //framebuffer settings
    //---------------------
    BasicShape screen_image = GetTexturedRectangle(texture_vao, glm::vec3(-1.424226, 1.479546, 1.513289), 2.848452, 1.469004,1.0f,false);

    //create fbo
    unsigned int fbo;
    glGenFramebuffers(1, &fbo);
    // bind the framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    //create fbo color attachment for the first texture
    unsigned int texture;
    glGenTextures(1, &texture);
    //bind the texture
    glBindTexture(GL_TEXTURE_2D, texture);
    //allocate memory for the texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // create a renderbuffer for possible depth and stencil attachments
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);

    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT); // use a single renderbuffer object for both a depth AND stencil buffer.

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    // check if the framebuffer is complete for both textures
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE){
	    // execute victory dance
	    std::cout << "Framebuffer is complete" << std::endl;
    }
    else{
	    std::cout << "Framebuffer is not complete" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    // render loop
    // -----------

    while (!glfwWindowShouldClose(window))
    {
	    for(unsigned int i = 0; i < 2; i++) {
		    // decide which texture to use
		    if (i == 0){
			    //handle texture
			    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		    }
		    //set viewport transformation
		    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		    glClearColor(clear_color.r,clear_color.g,clear_color.b,clear_color.a);
		    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		    // adjust for real time
		    float current_frame = glfwGetTime();
		    delta_time = current_frame - last_frame;
		    last_frame = current_frame; 
		    // input
		    // -----
		    ProcessInput(window);

		    shader_program.use();
		    shader_program.setBool("screen", false);
		    if (i == 0){
			    view = camera.GetViewMatrix(2);

		    }
		    else {
			    view = camera.GetCameraViewMatrix();
		    }
		    shader_program.setMat4("view",view);
		    shader_program.setVec4("view_position",glm::vec4(camera.Position,1.0));

		    //Set the light properties
		    //-------------------------
		    for (int i= 0; i < num_lights; i++) {
			    shader_program.setVec4("point_light_list["+std::to_string(i)+"].ambient",point_light_list[i].ambient);
			    shader_program.setVec4("point_light_list["+std::to_string(i)+"].diffuse",point_light_list[i].diffuse);
			    shader_program.setVec4("point_light_list["+std::to_string(i)+"].specular",point_light_list[i].specular);
			    shader_program.setVec4("point_light_list["+std::to_string(i)+"].position",point_light_list[i].position);
			    shader_program.setBool("point_light_list["+std::to_string(i)+"].on", point_light_list[i].on);
		    }	

		    //Draw the bowling lane
		    //---------------------
		    shader_program.setBool("is_textured",true);
		    shader_program.setBool("imported_material",false);
		    glm::mat4 lane_transform(1.0);
		    glm::mat4 lane_model(1.0);
		    shader_program.setMat4("model",lane_model);
		    shader_program.setMat4("transform",lane_transform);
		    glBindTexture(GL_TEXTURE_2D,bowling_lane_texture);
		    bowling_lane.Draw();
		    glBindTexture(GL_TEXTURE_2D,0);

		    //Draw the realease assembly arm
		    //---------------------
		    release_assembly_arm.ProcessInput(window, delta_time, &game_state);
		    release_assembly_arm.Draw();

		    //TODO Handle Joycon input
		    //---------------------
		
		    auto current_time = std::chrono::steady_clock::now();
		    auto time_delta = std::chrono::duration<float>(current_time - joycon.get_last_update_time()).count();

		    joycon.ProcessInput(time_delta);

		    if (joycon.get_roll_registered() && game_state.get_state() == VELOCITY_ARROW) {
			    float velocity_angle = -1.0 * glm::pi<float>()/64.0f * joycon.get_velocity_norm();
			    glm::vec3 velocity = glm::vec3(glm::rotate(glm::mat4(1.0), velocity_angle, glm::vec3(0.0,1.0,0.0)) *glm::vec4(0.0, 0.0, 1.0, 1.0));
			    bowling_ball.set_velocity(velocity);
			    bowling_ball.set_rotational_velocity(joycon.get_angular_velocity_norm());
			    joycon.reset();
			    game_state.change_state(ROLLING);
			    bowling_ball.set_release(true);
		    }

		    //Draw the velocity arrow
		    //----------------------
		    if (game_state.get_state() == VELOCITY_ARROW) {
			    velocity_arrow.ProcessInput(window, delta_time);
			    velocity_arrow.Draw();
		    }

		    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && firstAToggle && game_state.get_state() == VELOCITY_ARROW) {
			    firstAToggle = false;
			    //in degrees
			    float angle = velocity_arrow.get_angle_y();
			    glm::vec3 velocity = glm::vec3(glm::rotate(glm::mat4(1.0), glm::radians(angle), glm::vec3(0.0,1.0,0.0)) *glm::vec4(0.0, 0.0, 1.0, 1.0));
			    bowling_ball.set_velocity(velocity);
			    game_state.change_state(ROTATION_ARROW);
			    std::cout << "Game state changed to ROTATION_ARROW" << std::endl;
		    }
		    else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
			    firstAToggle = true;
		    }


		    // Draw the rotation arrow
		    //----------------------
		    rotation_arrow.ProcessInput(window, delta_time);

		    if (game_state.get_state() == ROTATION_ARROW) {

			    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && firstAToggle) {
				    firstAToggle = false;
				    //in degrees
				    float angle = rotation_arrow.get_angle_x();
				    //convert angle to rotational velocity
				    std::cout << "Angle: " << angle << std::endl;
				    float rotational_velocity = angle/90.0f;
				    bowling_ball.set_rotational_velocity(rotational_velocity);
				    game_state.change_state(ROLLING);
				    bowling_ball.set_release(true);
			    }
			    else{
				    rotation_arrow.ProcessInput(window, delta_time);
				    rotation_arrow.Draw();
			    }
		    }
		    else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
			    firstAToggle = true;
		    }

		    //std::cout << game_state.get_state() << std::endl;

		    // Draw the bowling ball
		    //----------------------
		    //
		    bowling_ball.ProcessInput(window, delta_time);

		    if (bowling_ball.get_in_lane()) {
			    bowling_ball.Draw();
		    }
		    if (game_state.get_state() == ROLLING && !bowling_ball.get_in_lane()){ 
			    game_state.change_state(LOWER_ARM);
		    }

		    //RESET objects
		    //----------------------
		    if (game_state.get_state() == RESET) {
			    //reset the ball
			    bowling_ball.reset();
			    //reset the pins
			    for (int i = 0; i < 10; i++) {
				    bowling_pins[i].reset();
			    }
			    game_state.change_state(RAISE_ARM);
		    }

		    //Draw the bowling pin and count the number of pins that are still standing
		    int new_pins_standing = 0;
		    //----------------------
		    for (int i = 0; i < 10; i++) {
			    if (bowling_pins[i].get_in_lane()){ 
				    handle_ball_pin_collision(&bowling_ball, &bowling_pins[i]);
				    if (!bowling_pins[i].get_is_hit()) new_pins_standing ++;
				    bowling_pins[i].ProcessInput(window, delta_time);
				    bowling_pins[i].Draw();
			    }
		    }
		    pins_standing = new_pins_standing;

		    //Fun part
		    for (int i = 0; i < 10; i++) {
			    if (bowling_pins[i].get_in_lane()) {
				    for (int j = i+1; j < 10; j++) {
					    if (bowling_pins[j].get_in_lane())
						    handle_pin_pin_collision(&bowling_pins[i], &bowling_pins[j]);
				    }
			    }
		    }

		    glBindFramebuffer(GL_FRAMEBUFFER, 0);
		    }

		    //Draw the television
		    //--------------------
		    shader_program.use();
		    shader_program.setBool("is_textured", false);
		    shader_program.setBool("imported_material", true);

		    model = glm::mat4(1.0);
		    shader_program.setMat4("model",model);
		    shader_program.setMat4("transform",television_transform);
		    television_shape.Draw(shader_program);

		    //Draw the text so that it stays with the camera
		    if (display_text) {
			    shader_program.setBool("imported_material",false);
			    shader_program.setBool("is_textured", true);

			    shader_program.setMat4("transform",identity_matrix);

			    shader_program.setMat4("model",model);
			    shader_program.setMat4("transform",identity_matrix);

			    //void draw_text(Shader* shader, Font* font, glm::vec3 camera_position, glm::vec3 ball_position, int pins_standing) {
			    draw_text(&font_program, &arial_font, camera.Position, bowling_ball.get_position(), pins_standing);
		    } 

		    //Draw skybox last
		    //----------------------------
		    skybox_shader.use();
		    //remove the translation component of the view matrix
		    view = glm::mat4(glm::mat3(camera.GetCameraViewMatrix()));
		    skybox_shader.setMat4("view",view);
		    skybox_shader.setMat4("projection",projection);
		    glDepthFunc(GL_EQUAL);
		    //bind skybox texture
		    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		    skybox.Draw();
		    glDepthFunc(GL_LESS);

		    shader_program.use();
		    shader_program.setBool("is_textured", true);
		    shader_program.setBool("screen", true);
		    glBindTexture(GL_TEXTURE_2D, texture);
		    screen_image.Draw();

		    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		    // -------------------------------------------------------------------------------
		    glfwSwapBuffers(window);
		    glfwPollEvents();

	    }



    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &(vao.id));
    glDeleteVertexArrays(1,&(import_vao.id));
    glDeleteVertexArrays(1,&(texture_vao.id));

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void ProcessInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window,GLFW_KEY_SPACE)==GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    
    if (glfwGetKey(window,GLFW_KEY_SPACE)==GLFW_RELEASE)
        glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

    //Take care of the light toggle
    if (glfwGetKey(window, GLFW_KEY_L)) {
	    lKeyPressed = true;
	    if (!lightToggle){
		    std::cout<<"Light switch"<<std::endl;
		    blue_light.on = !blue_light.on;
		    std::cout<<"Blue light on: "<<blue_light.on<<std::endl;
		    lightToggle = true;
	    }
    }
    else if (glfwGetKey(window, GLFW_KEY_L) == GLFW_RELEASE && lKeyPressed) {
	    lKeyPressed = false;
	    lightToggle = false;
    }

    // Upon C key press, the camera will change
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && camera.firstPress) {
	    //toggle the camera
	    camera.ChangeCameraMode((camera.cameraMode + 1) % 3); 
	    camera.firstPress = false;
	    std::cout<<"Camera fixed"<<std::endl;
    }
    else if (glfwGetKey(window, GLFW_KEY_C) == GLFW_RELEASE) {
	    camera.firstPress = true;
    } 
    //Upon H key press, the text will be displayed
    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS && display_first_press) {
	    display_text = !display_text;
	    display_first_press = false;
	    std::cout<<"Display text"<<std::endl;
    }
    else if (glfwGetKey(window, GLFW_KEY_H) == GLFW_RELEASE) {
	    display_first_press = true;
    }

    if (glfwGetKey(window,GLFW_KEY_W)==GLFW_PRESS && !camera.fixedCamera) {
	    camera.ProcessKeyboard(FORWARD,delta_time);
    }

    if (glfwGetKey(window,GLFW_KEY_S)==GLFW_PRESS && !camera.fixedCamera) {
	    camera.ProcessKeyboard(BACKWARD,delta_time);
    }

    if (glfwGetKey(window,GLFW_KEY_A)==GLFW_PRESS && !camera.fixedCamera) {
	    camera.ProcessKeyboard(LEFT,delta_time);
    }

    if (glfwGetKey(window,GLFW_KEY_D)==GLFW_PRESS && !camera.fixedCamera) {
	    camera.ProcessKeyboard(RIGHT,delta_time);
    }
    
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{

	if (first_mouse)
	{
		last_x = xpos;
		last_y = ypos;
		first_mouse = false;
	}

	float xoffset = xpos - last_x;
	float yoffset = last_y - ypos; 
	last_x = xpos;
	last_y = ypos;

	float sensitivity = 0.7f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	camera.ProcessMouseMovement(xoffset,yoffset);


}  
