#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "support/environment_setup.hpp"
#include "support/build_shapes.hpp"
#include "classes/ship.hpp"
#include "classes/camera.hpp"
#include "fonts/Font.hpp"
#include "classes/import_object.hpp"

/*ProcessInput
 * Accepts a GLFWwindow pointer as input and processes 
 * user key and mouse button inpbut.  
 * Returns nothing.
 */
void ProcessInput(GLFWwindow *window);

/*DrawPyramid
 * Given a vector of BasicShape pointers, a vector of texture ids, a location
 * and a pointer to a shader object, draw the shapes using the given textures
 * and shader.  Assume the shader is already in use.
 */
void DrawPyramid(std::vector<BasicShape*> shapes, 
                std::vector<unsigned int> textures,
                glm::vec3 location,
                Shader *shader);

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

float angle_z = 0.0;
float angle_x = 0.0;
float rotate_speed = 90.0;

//Global Variables
//-------------
bool first_mouse{true};
float last_x{0.0};
float last_y{0.0};

//Camera Object
Camera camera(glm::vec3(0.0f,1.0f,25.0f),glm::vec3(0.0f,1.0f,0.0f),-90.0f,0.0f);
float delta_time{0.001};
float last_frame{0.0};

//light switches
glm::vec4 light_position (0.0,20.0,20.0,1.0);

Font arial_font ("fonts/ArialBlackLarge.bmp","fonts/ArialBlack.csv",0.1,0.15);

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

    //BasicShape die = importer.loadFiles("models/die",import_vao);
    //int die_texture = importer.getTexture();
    BasicShape release_assembly_arm = importer.loadFiles("models/ReleaseAssemblyArm",import_vao);
    BasicShape light_cube = GetCube(vao, light_position, 1.0);

    arial_font.initialize(texture_vao);

    //Create the shapes
    //-------------------------
    
    glm::mat4 model(1.0);
    glm::mat4 view(1.0);
    glm::mat4 projection(1.0);
    
    shader_program.use();

    glm::mat4 identity_matrix (1.0);
    
    model = glm::rotate(model,glm::radians(-90.0f),glm::vec3(1.0,0.0,0.0));
    shader_program.setMat4("model",model);
    view = glm::translate(view,glm::vec3(0.0,10.0,0.0));
    view = glm::rotate(view,glm::radians(-90.0f),glm::vec3(1.0,0.0,0.0));
    shader_program.setMat4("view",view);
    projection = glm::perspective(glm::radians(45.0f),(1.0f*SCR_WIDTH)/(1.0f*SCR_HEIGHT),0.1f,100.0f);
    shader_program.setMat4("projection",projection);

    //Set up the depth test
    glEnable(GL_DEPTH_TEST);

    //lighting 
    glm::vec3 light_color (1.0);
    glm::vec3 ambient_color = 0.1f*light_color;
    
    shader_program.setVec4("point_light.ambient",glm::vec4(0.1f*light_color,1.0));
    shader_program.setVec4("point_light.diffuse",glm::vec4(light_color,1.0f));
    shader_program.setVec4("point_light.specular",glm::vec4(0.5f*light_color,1.0f));
    shader_program.setVec4("point_light.position",light_position);
    shader_program.setBool("point_light.on",true);

    shader_program.setVec4("view_position",glm::vec4(camera.Position,1.0));

    //font shader settings
    font_program.use();
    font_program.setMat4("transform",identity_matrix);
    font_program.setMat4("model",identity_matrix);
    font_program.setMat4("view",identity_matrix);
    font_program.setMat4("projection",glm::ortho(-1.0,1.0,-1.0,1.0,-1.0,1.0));
    font_program.setVec4("transparentColor", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    font_program.setFloat("alpha", 0.3);
    font_program.setInt("texture1", 0);

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

        // render
        // ------
        glClearColor(clear_color.r,clear_color.g,clear_color.b,clear_color.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader_program.use();

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
	shader_program.setVec4("set_color",glm::vec4(1.0,1.0,1.0,1.0));
	light_cube.Draw();
	shader_program.setInt("light_cube",false);

        //Draw the realease assembly arm
        shader_program.setBool("is_textured",false);
        shader_program.setBool("imported_material",true);
        glm::mat4 avatar_transform(1.0);
        glm::mat4 avatar_model(1.0);
        avatar_transform = glm::translate(avatar_transform,glm::vec3(0.0,1.0,0.0));
        avatar_transform = glm::rotate(avatar_transform,glm::radians(180.0f),glm::vec3(0.0,1.0,0.0));
        shader_program.setMat4("model",avatar_model);
        shader_program.setMat4("transform",avatar_transform);
	release_assembly_arm.Draw();
        shader_program.setBool("imported_material",false);

        shader_program.setBool("is_textured",true);
        shader_program.setMat4("model",model);
        shader_program.setMat4("transform",identity_matrix);
        
        if (camera.Position.y < 0.5) {
            camera.Position.y = 0.5;
        }

        //Draw the text so that it stays with the camera
        font_program.use();
        //arial_font.DrawCharacter('*',glm::vec2(-0.05,0.0),font_program);
        std::string display_string = "Camera (";
        std::string cam_x = std::to_string(camera.Position.x);
        std::string cam_y = std::to_string(camera.Position.y);
        std::string cam_z = std::to_string(camera.Position.z);

        display_string += cam_x.substr(0,cam_x.find(".")+3) +",";
        display_string += cam_y.substr(0,cam_y.find(".")+3) +",";
        display_string += cam_z.substr(0,cam_z.find(".")+3) +")";
        
        arial_font.DrawText(display_string,glm::vec2(-0.1,0.75),font_program);

    
       
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
    

    if (glfwGetKey(window,GLFW_KEY_RIGHT)==GLFW_PRESS) {
        angle_z -= rotate_speed * delta_time;
    }

    if (glfwGetKey(window,GLFW_KEY_LEFT)==GLFW_PRESS) {
        angle_z += rotate_speed * delta_time;
    }

    if (glfwGetKey(window,GLFW_KEY_W)==GLFW_PRESS) {
        camera.ProcessKeyboard(FORWARD,delta_time);
    }

    if (glfwGetKey(window,GLFW_KEY_S)==GLFW_PRESS) {
        camera.ProcessKeyboard(BACKWARD,delta_time);
    }

    if (glfwGetKey(window,GLFW_KEY_A)==GLFW_PRESS) {
        camera.ProcessKeyboard(LEFT,delta_time);
    }

    if (glfwGetKey(window,GLFW_KEY_D)==GLFW_PRESS) {
        camera.ProcessKeyboard(RIGHT,delta_time);
    }


}

void DrawPyramid(std::vector<BasicShape*> shapes, 
                std::vector<unsigned int> textures,
                glm::vec3 location,
                Shader *shader)
{
    if (shapes.size() != textures.size()) {
        std::cout<<"DrawPyramid: Number shapes does not match number of textures."<<std::endl;
        return;
    }
    shader->setBool("is_textured",true);

    //shader_program.setMat4("transform",identity_matrix);
    glm::mat4 pyramid_transform(1.0);
    pyramid_transform = glm::translate(pyramid_transform,location);
    pyramid_transform = glm::rotate(pyramid_transform,glm::radians(angle_x),
                                    glm::vec3(1.0,0.0,0.0));
    pyramid_transform = glm::rotate(pyramid_transform,glm::radians(angle_z),
                                    glm::vec3(0.0,0.0,1.0));
    shader->setMat4("transform",pyramid_transform);

    for (int i = 0; i < shapes.size(); i++){
        glBindTexture(GL_TEXTURE_2D,textures[i]);
        shapes[i]->Draw();
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
