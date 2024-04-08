#ifndef BASIC_SHAPE_HPP
#define BASIC_SHAPE_HPP

#include "vertex_attribute.hpp"
#include "Shader.hpp"

class BasicShape {
    protected:
        VAOStruct vao;
        unsigned int vbo = 0;
        int number_vertices = 0;
        GLuint primitive = GL_TRIANGLES;
        unsigned int ebo;
        int ebo_number_indices;
        int ebo_primitive;
    
    public:
        //Constructor for a BasicShape object (no inputs)
        BasicShape();

        //Initializes a BasicShape object given a VAOStruct, a pointer to vertex data, and integer for the number
        //of bytes in the data, an int for the number of vertices, and an int for the primitive used (default is GL_TRIANGLES) 
        void Initialize(VAOStruct vao, float* vertices, int vertices_bytes, int num_vertices, GLuint prim = GL_TRIANGLES);
        
        //Initializes a BasicShape's EBO given a pointer to EBO data as an unsigned int array, an int representing the number of 
        //  bytes in the EBO, an int representing the number of indices, and an int for the primitive used (default is line loop)
        void InitializeEBO(unsigned int *ebo_data, int ebo_bytes, int num_indices, int primitive=GL_LINE_LOOP);
        
        //Draws the shape using a given shader program.
        void Draw (Shader shader);

        //Draws a shape without first setting a shader program (assumes a shader program
        // is already in use).
        void Draw ();

        //Draws an EBO with a float input representing the desired line_width (default is 1.0)
        void DrawEBO(float line_width=1.0);

        //Returns the identifier (unsigned int) of a VBO for the shape.
        unsigned int GetVBO ();

        //Deletes the shape data
        void DeallocateShape();
};

#endif //BASIC_SHAPE_HPP
