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
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

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

//Television Screen state
//---------------
glm::mat4 television_transform = glm::translate(glm::mat4(1.0),glm::vec3(0.0f,0.0f,1.0f));

//Camera Object
Camera camera(glm::vec3(0.0f,1.0f,-3.0f),glm::vec3(0.0f,1.0f,0.0f),90.0f,0.0f);
float delta_time{0.001};
float last_frame{0.0};

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
struct Spotlight {
	glm::vec4 position;
	glm::vec4 direction;
	glm::vec4 color;
	float cut_off;
};

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
//TODO Make a function to handle collision between any two objects
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
			std::cout<<"Collision detected"<<std::endl;
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
		std::cout<<"Collision detected"<<std::endl;
	}
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
    
    ImportOBJ importer;

    BasicShape bowling_lane = importer.loadFiles("models/BowlingLane",import_vao);
    int bowling_lane_texture = importer.getTexture();
    BowlingBall bowling_ball(&import_vao, &shader_program);
    std::cout<<bowling_ball.get_position().x<<std::endl;
    VelocityArrow velocity_arrow(&import_vao, &shader_program, bowling_ball.get_position());
    ReleaseAssemblyArm release_assembly_arm(&import_vao, &shader_program);

    BasicShape television_shape = importer.loadFiles("models/Television",import_vao);


    //Create bowling pins
    //-------------------------
    float bowling_pins_length = 0.15;
    glm::vec3 bowling_pins_position = glm::vec3(0.0, 0.0, 1.0);
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

    BasicShape blue_light_cube = GetCube(vao, blue_light_position, 0.1);
    BasicShape white_light_cube = GetCube(vao, white_light_position, 0.1);
    BasicShape origin_cube = GetCube(vao, glm::vec4(0.0,0.0,0.0,1.0), 0.1);

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

    point_light_list[0] = blue_light;
    point_light_list[1] = white_light;

    int num_lights = sizeof(point_light_list)/sizeof(PointLight);
    for (int i= 0; i < num_lights; i++) {
	    shader_program.setVec4("point_light_list["+std::to_string(i)+"].ambient",point_light_list[i].ambient);
	    shader_program.setVec4("point_light_list["+std::to_string(i)+"].diffuse",point_light_list[i].diffuse);
	    shader_program.setVec4("point_light_list["+std::to_string(i)+"].specular",point_light_list[i].specular);
	    shader_program.setVec4("point_light_list["+std::to_string(i)+"].position",point_light_list[i].position);
	    shader_program.setBool("point_light_list["+std::to_string(i)+"].on", point_light_list[i].on);
    }

    Spotlight spotlight = {glm::vec4(0.0,3.0,19.0, 1.0),
	    glm::vec4(0.0,-1.0, 0.0, 1.0),
	    glm::vec4(1.0,1.0,1.0,1.0),
	    12.5};

/*
    shader_program.setVec4("blue_light.ambient",blue_light.ambient);
    shader_program.setVec4("blue_light.diffuse",blue_light.diffuse);
    shader_program.setVec4("blue_light.specular",blue_light.specular);
    shader_program.setVec4("blue_light.position",blue_light.position);
    shader_program.setBool("blue_light.on", blue_light.on);

    shader_program.setVec4("white_light.ambient", white_light.ambient);
    shader_program.setVec4("white_light.diffuse", white_light.diffuse);
    shader_program.setVec4("white_light.specular", white_light.specular);
    shader_program.setVec4("white_light.position", white_light.position);
    shader_program.setBool("white_light.on", white_light.on);
*/

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

    // render loop
    // -----------

    while (!glfwWindowShouldClose(window))
    {
        // adjust for real time
        float current_frame = glfwGetTime();
        delta_time = current_frame - last_frame;
        last_frame = current_frame; 
        // input
        // -----
        ProcessInput(window);

	//Handle light toggle
	//-------------------
	

        // render
        // ------
        glClearColor(clear_color.r,clear_color.g,clear_color.b,clear_color.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shader_program.use();

	//Take care of the light toggle
	if (glfwGetKey(window, GLFW_KEY_L)) {
		lKeyPressed = true;
		if (!lightToggle){
			std::cout<<"Light switch"<<std::endl;
			blue_light.on = !blue_light.on;
			std::cout<<"Blue light on: "<<blue_light.on<<std::endl;
			shader_program.setBool("blue_light.on",blue_light.on);
			lightToggle = true;
		}
	}
	else if (glfwGetKey(window, GLFW_KEY_L) == GLFW_RELEASE && lKeyPressed) {
		lKeyPressed = false;
		lightToggle = false;
	}

	view = camera.GetViewMatrix();
	shader_program.setMat4("view",view);
	shader_program.setVec4("view_position",glm::vec4(camera.Position,1.0));


	//Draw the light cube
	//-------------------
	shader_program.setInt("light_cube",true);
	shader_program.setBool("is_textured",false);
	shader_program.setBool("imported_material",false);
	glm::mat4 transform(1.0);
	glm::mat4 model(1.0);
	shader_program.setMat4("transform",transform);
	shader_program.setMat4("model",model);
	shader_program.setVec4("set_color",glm::vec4(0.0,0.0,1.0,1.0));
	//blue_light_cube.Draw();
	shader_program.setVec4("set_color",glm::vec4(1.0,1.0,1.0,1.0));
	//origin_cube.Draw();
	white_light_cube.Draw();
	shader_program.setInt("light_cube",false);

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
	release_assembly_arm.ProcessInput(window, delta_time);
       	release_assembly_arm.Draw();

	//Draw the arrow
	//----------------------
	velocity_arrow.ProcessInput(window, delta_time);
	velocity_arrow.Draw();

	//Draw the bowling ball
	//----------------------
	float velocity_arrow_angle = velocity_arrow.get_angle_y();
	bowling_ball.ProcessInput(window, delta_time, velocity_arrow_angle);
	bowling_ball.Draw();

	//Draw the bowling pin
	//----------------------
	for (int i = 0; i < 10; i++) {
		handle_ball_pin_collision(&bowling_ball, &bowling_pins[i]);
		bowling_pins[i].ProcessInput(window, delta_time);
		bowling_pins[i].Draw();
	}
	//Fun part
	for (int i = 0; i < 10; i++) {
		for (int j = i+1; j < 10; j++) {
			handle_pin_pin_collision(&bowling_pins[i], &bowling_pins[j]);
		}
	}

	//Draw the television
	//--------------------
	model = glm::mat4(1.0);
	shader_program.setMat4("model",model);
	shader_program.setMat4("transform",television_transform);
	shader_program.setBool("imported_material",true);
	television_shape.Draw(shader_program);
	shader_program.setBool("imported_material",false);
	shader_program.setMat4("transform",identity_matrix);

	//Draw the text
        shader_program.setBool("is_textured",true);
        shader_program.setMat4("model",model);
        shader_program.setMat4("transform",identity_matrix);
        
        //Draw the text so that it stays with the camera
	if (display_text) {

		font_program.use();
		std::string camera_position_str = "Camera (";
		std::string cam_x = std::to_string(camera.Position.x);
		std::string cam_y = std::to_string(camera.Position.y);
		std::string cam_z = std::to_string(camera.Position.z);

		camera_position_str += cam_x.substr(0,cam_x.find(".")+3) +",";
		camera_position_str += cam_y.substr(0,cam_y.find(".")+3) +",";
		camera_position_str += cam_z.substr(0,cam_z.find(".")+3) +")";

		std::string ball_position_str = "Ball (";
		std::string ball_x = std::to_string(bowling_ball.get_position().x);
		std::string ball_y = std::to_string(bowling_ball.get_position().y);
		std::string ball_z = std::to_string(bowling_ball.get_position().z);

		ball_position_str += ball_x.substr(0,ball_x.find(".")+3) +",";
		ball_position_str += ball_y.substr(0,ball_y.find(".")+3) +",";
		ball_position_str += ball_z.substr(0,ball_z.find(".")+3) +")";

		std::string pins_standing_str = "Pins standing: ";
		int pins_standing = 0;
		for (int i = 0; i < 10; i++) {
			if (!bowling_pins[i].get_is_hit()) {
				pins_standing++;
			}
		}
		pins_standing_str += std::to_string(pins_standing);


	
		//Draw semi-transparent box where the text will be displayed
		//Draws a rectangle at a given x and y coordinate (lower left hand)
		//void DrawRect(glm::vec2 loc, Shader sProgram, float depth_change = 0);
		arial_font.DrawRect(font_program);
		arial_font.DrawText(camera_position_str,glm::vec2(-1.0f, 0.8f),font_program);
		arial_font.DrawText(ball_position_str,glm::vec2(-1.0f, 0.6f),font_program);
		arial_font.DrawText(pins_standing_str,glm::vec2(-1.0f, 0.4f),font_program);

	} 
       
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


    // Upon C key press, the camera will change
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && camera.firstPress) {
	    //toggle the camera
	    camera.ChangeCameraMode(); 
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
