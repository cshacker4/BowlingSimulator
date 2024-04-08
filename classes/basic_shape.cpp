#include "basic_shape.hpp"

BasicShape::BasicShape()
{
    this->number_vertices = 0;
    this->vbo = 0;
    this->primitive = GL_TRIANGLES;
}

void BasicShape::Initialize(VAOStruct vao, float* vertices, int vertices_bytes, int num_vertices, GLuint prim)
{
    this->vao = vao;
    this->number_vertices = num_vertices;
    this->primitive = prim;
    glGenBuffers(1,&(this->vbo));
    glBindBuffer(GL_ARRAY_BUFFER,this->vbo);
    glBufferData(GL_ARRAY_BUFFER,vertices_bytes,vertices,GL_STATIC_DRAW);

}

void BasicShape::InitializeEBO(unsigned int *ebo_data, int ebo_bytes, 
                            int num_indices, int primitive)
{
    this->ebo_number_indices = num_indices;
    this->ebo_primitive = primitive;
    glGenBuffers(1,&(this->ebo));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,this->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,ebo_bytes,ebo_data,GL_STATIC_DRAW);
}

void BasicShape::Draw (Shader shader)
{
    glUseProgram(shader.ID);
    this->Draw();

}

void BasicShape::DrawEBO (float line_width)
{
    if (this->ebo == 0)
    {
        std::cout<<"Attempted to draw ebo "<<this->ebo<<std::endl;
        return;
    }
    BindVAO(this->vao, this->vbo);
    glLineWidth(line_width);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,this->ebo);
    glDrawElements(this->ebo_primitive,this->ebo_number_indices,GL_UNSIGNED_INT,0);
    glLineWidth(1.0);
}

void BasicShape::Draw ()
{
    //Assumes the shader has already been set (more efficient)
    BindVAO(this->vao,this->vbo,GL_ARRAY_BUFFER);
    glDrawArrays(this->primitive,0,this->number_vertices);

}

void BasicShape::DeallocateShape()
{
    glDeleteBuffers(1,&(this->vbo));
    if (this->ebo > 0)
        glDeleteBuffers(1,&(this->ebo));
}

unsigned int BasicShape::GetVBO() {
    return this->vbo;
}