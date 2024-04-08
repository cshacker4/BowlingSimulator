#include "vertex_attribute.hpp"

AttributePointer BuildAttribute(int number_vals_per, int data_type, bool is_normalized, int stride, int offset)
{
    AttributePointer attribute;
    attribute.number_per_vertex = number_vals_per;
    attribute.type_data = data_type;
    attribute.normalize_data = is_normalized;
    attribute.stride_bytes = stride;
    attribute.offset_bytes = offset;
    return attribute;
}

void BindVAO (VAOStruct vao, unsigned int vbo, int buffer_type) 
{
    glBindVertexArray(vao.id);
    glBindBuffer(buffer_type,vbo);
    for(int i = 0; i < vao.attributes.size(); i++) {
        glVertexAttribPointer(i,vao.attributes[i].number_per_vertex,
                                vao.attributes[i].type_data,
                                vao.attributes[i].normalize_data,
                                vao.attributes[i].stride_bytes,
                                (void*)(intptr_t)vao.attributes[i].offset_bytes);
        glEnableVertexAttribArray(i);
    }
}