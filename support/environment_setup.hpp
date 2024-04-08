#ifndef ENVIRONMENT_SETUP_HPP
#define ENVIRONMENT_SETUP_HPP

#include "../classes/basic_shape.hpp"

#include <glm/glm.hpp>

#include <iostream>



/* framebuffer_size_callback
 * Accepts a GLFWwindow pointer, integer width, and integer height as input.
 * Resizes the viewport when the window is resized (as long as the callback
 * is set within the main program). 
 * Returns nothing.
 */
void framebuffer_size_callback(GLFWwindow* window, int width, int height);



/* InitializeEnvironment
 * Accepts an integer screen width, integer screen height, and a string for the 
 * window title.  Initializes glfw, creates a window, sets the current context,
 * and initializes GLAD.  Returns NULL if GLFW fails and otherwise returns a pointer
 * to the created window.
 */
GLFWwindow* InitializeEnvironment(int screen_width, int screen_height,
                                  std::string window_title)
{

// glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(screen_width, screen_height, window_title.c_str(), NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return NULL;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); 


    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return NULL;
    }
    return window;

}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}





#endif //ENVIRONMENT_SETUP_HPP