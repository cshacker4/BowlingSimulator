#ifndef VERTEX_ATTRIBUTE_HPP
#define VERTEX_ATTRIBUTE_HPP
#include <glad/glad.h>
#include <GLFW/glfw3.h>


#include <vector>

struct AttributePointer
{
    int number_per_vertex;
    int type_data;
    bool normalize_data;
    int stride_bytes;
    int offset_bytes;
};

struct VAOStruct
{
    unsigned int id;
    std::vector<AttributePointer> attributes;

};

AttributePointer BuildAttribute(int number_vals_per, int data_type, bool is_normalized, int stride, int offset);
void BindVAO (VAOStruct vao, unsigned int vbo, int buffer_type = GL_ARRAY_BUFFER);



#endif //VERTEX_ATTRIBUTE_HPP
