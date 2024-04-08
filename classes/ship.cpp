#include "ship.hpp"

Ship::Ship() {
  //just sets up the ship initially
}

void Ship::Initialize(VAOStruct base_vao, VAOStruct text_vao, Shader *shader, 
                       unsigned int hull_text) {
    //set initialized to true when the ship has been set up.
    this->initialized = true;

    //set up the data members for the ship based on the inputs.
    this->vao = base_vao;
    this->texture_vao = text_vao;
    this->shader = shader;
    this->hull_texture = hull_text;
    this->angle_z = 0.0;
    this->ship_scale = glm::vec3(0.2,0.2,1.0);

    //build the BasicShape objects used by the ship
    this->hull = GetTexturedHull(this->texture_vao,glm::vec3(-0.2,-0.5,0.0),0.4,0.5);
    this->view_screen = GetHull(this->vao,glm::vec3(-0.15,-0.15,0.0),0.3,0.1);
    this->wing = GetTriangle(this->vao,glm::vec3(-0.4,-0.5,0.0),
                                      glm::vec3(0.4, -0.5, 0.0),
                                      glm::vec3(0.0,-0.01,0.0));
    this->tail_fin = GetTriangle(this->vao,glm::vec3(-0.05,-0.5,0.0),
                                      glm::vec3(0.05, -0.5, 0.0),
                                      glm::vec3(0.0,-0.4,0.0));

}

void Ship::BoundCheck() 
{
    
    float sum_x {this->location.x + this->velocity.x};
    float sum_y {this->location.y + this->velocity.y};

    if ((sum_x>this->boundary[2]) || (sum_x < this->boundary[0])) {
        this->velocity.x = 0.0;
    }
    if ((sum_y>this->boundary[3]) || (sum_y < this->boundary[1])) {
        this->velocity.y = 0.0;
    }
}



void Ship::Draw()
{
    //Avoid trying to draw an uninitialized ship!
    if (!(this->initialized)) {
        std::cout<<"Attempted to draw uninitialized ship."<<std::endl;
        return;
    }
    //Avoid trying to draw if the shader is NULL.
    if (this->shader == NULL) {
        std::cout << "No shader program provided while drawing ship."<<std::endl;
        return;
    }

    //Ship transformation is 1) orient the model to the origin
    //                       2) place the model in the world
    glm::mat4 ship_transform (1.0);
    ship_transform = glm::translate(ship_transform,location);
    ship_transform = glm::rotate(ship_transform,glm::radians(this->angle_z),glm::vec3(0.0,0.0,1.0));
    ship_transform = glm::rotate(ship_transform,glm::radians(-90.0f),glm::vec3(0.0,0.0,1.0));
    ship_transform = glm::scale(ship_transform,this->ship_scale);
    ship_transform = glm::translate(ship_transform,glm::vec3(0.0,0.25,0.0));
    this->shader->setMat4("transform",ship_transform);

    //Draw the ship in the order, wing, hull, view screen, tail fin
    this->shader->setBool("is_textured",false);
    this->shader->setVec4("set_color",glm::vec4(0.4,0.3,0.3,1.0));
    this->wing.Draw();
    this->shader->setVec4("set_color",glm::vec4(0.0,0.0,0.0,1.0));
    this->wing.DrawEBO(3.0f);
    glBindTexture(GL_TEXTURE_2D,this->hull_texture);
    this->shader->setBool("is_textured",true);
    this->hull.Draw();
    this->shader->setBool("is_textured",false);
    this->hull.DrawEBO(3.0f);
    this->shader->setVec4("set_color",glm::vec4(0.2,0.2,0.6,1.0));
    this->view_screen.Draw();
    this->shader->setVec4("set_color",glm::vec4(0.0,0.0,0.0,1.0));
    this->view_screen.DrawEBO();
    this->shader->setVec4("set_color",glm::vec4(0.4,0.3,0.3,1.0));
    this->tail_fin.Draw();
    this->shader->setVec4("set_color",glm::vec4(0.0,0.0,0.0,1.0));
    this->tail_fin.DrawEBO();
    
}

void Ship::SetBounds (glm::vec4 bounds_minxy_maxxy) {

    this->boundary = bounds_minxy_maxxy;

}

glm::vec3 Ship::Location() {
    return this->location;
}

void Ship::ProcessInput(GLFWwindow *window) {
    
     if (glfwGetKey(window,GLFW_KEY_LEFT)== GLFW_PRESS) {
        this->angle_z += 0.1;
    }

    if (glfwGetKey(window,GLFW_KEY_RIGHT)== GLFW_PRESS) {
        this->angle_z -= 0.1;
    }

    if ((glfwGetKey(window, GLFW_KEY_UP)==GLFW_PRESS) && (!thruster_fired))
    {
        velocity += this->speed * glm::vec3(cos(glm::radians(this->angle_z)),
                                     sin(glm::radians(this->angle_z)),
                                     0.0);
        thruster_fired = true;
    }

    if (glfwGetKey(window,GLFW_KEY_UP)==GLFW_RELEASE) {
        thruster_fired = false;
    }

    //keep the ship in bounds!
    this->BoundCheck();

    //EACH iteration have the ship move according to its current velocity.
    location += velocity;
}

Ship::~Ship() {
 this->hull.DeallocateShape();
 this->wing.DeallocateShape();
 this->view_screen.DeallocateShape();
 this->tail_fin.DeallocateShape();   
}