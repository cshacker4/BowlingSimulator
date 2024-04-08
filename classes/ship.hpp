#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "basic_shape.hpp"
#include "../support/build_shapes.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


class Ship {
    protected:
        //Initialized boolean
        bool initialized = false;

        //VAO for the untextured data
        VAOStruct vao;

        //VAO for textured data
        VAOStruct texture_vao;

        //Pointer to a shader object (assumes a single shader is used)
        Shader *shader = NULL;
        
        //BasicShape object for the hull
        BasicShape hull;

        //BasicShape object for the view_screen
        BasicShape view_screen;

        //BasicShape object for the wing
        BasicShape wing;

        //BasicShape object for the tail fin
        BasicShape tail_fin;

        //Texture for the hull (could have this for all the shapes!)
        unsigned int hull_texture{0};
        
        //ship location
        glm::vec3 location{0.0};

        //ship velocity
        glm::vec3 velocity{0.0};

        //ship heading
        float angle_z{0};

        //ship scale
        glm::vec3 ship_scale{1.0};

        //ship thrusters fired flag
        bool thruster_fired = false;

        //ship speed each thrust
        float speed = 0.0001;

        //world boundary (format min_x,max_x,min_y,max_y)  
        //Note: default value is normalized device coordinates
        glm::vec4 boundary{-1.0,1.0,-1.0,1.0};

    public:

        //Basic constructor for a Ship object
        Ship();

        //Initializes the ship given an untextured and texture data vao, a pointer
        //  to a shader object, and an unsigned int for a texture of the hull.  
        //  Initializes the ship using the build_shapes.hpp functions.
        void Initialize(VAOStruct base_vao, VAOStruct text_vao, Shader *shader, unsigned int hull_text);

        //Renders the ship at the current location, scale, and heading using the 
        //  provided shader.
        void Draw();

        //Processes user input for the ship.
        void ProcessInput(GLFWwindow *window);

        //Sets the world boundary for the ship.
        void SetBounds (glm::vec4 bounds_minxy_maxxy);

        //Checks to see if the ship will go out of bounds and adjusts the 
        //  velocity accordingly given bounds. 
        void BoundCheck ();

        //Return the location of the ship as a glm::vec3
        glm::vec3 Location();

        //Destructor for the ship.  Note that this will delete associated
        //  VBOs for the ship (you would re-use these in multiple ships you may
        //  want to make a separate DeallocateShip function to avoid having 
        //  VBOs automatically delete when the ship goes out of scope).
        ~Ship();
};