#version 330 core

//Accounts for position data VBOs and those with position and texture 
//  coordinates.
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aCoord;
layout (location = 3) in vec3 dColor;
layout (location = 4) in vec3 sColor;

//pass the texture coordinates to the fragment shader.
out vec2 texture_coordinates;

//pass the normal to the fragment shader
out vec3 norm;

//pass imported data to the fragment shader
out vec3 diffuse_color;
out vec3 specular_color;

out vec3 fragment_position;
//"flat" is a key word that indicates this value stays the same 
//  and is not altered between vertices (like color and texture coordinates are)
flat out int textured;
flat out int vertex_material;

//A flag used to indicate that the drawn shape is texture-mapped.
uniform bool is_textured;
uniform bool imported_material;

//transformation matrices for the projection, view (camera), model (world), and 
//  transform (local).  
uniform mat4 projection, view, model, transform;

void main()
{
  //See chapter 6.2.2 for this operation
  norm = mat3(transpose(inverse(model*transform))) * aNorm;
  //if it is texture mapped with s and t coordinates, we need to pass those
  // to the next shader and tell the fragment shader to expect a texture 
  // (the latter is done by setting textured to 1.  You can't pass booleans
  // along).
  if (is_textured) {
    texture_coordinates = aCoord;
    textured = 1;
  } else {
    //not textured so tell the fragment shader by setting the textured int to 
    //  0 (not a textured object)
    textured = 0;
  }

  if (imported_material) {
    vertex_material = 1;
    diffuse_color = dColor;
    specular_color = sColor;
  } else {
    vertex_material = 0;
  }
  fragment_position = (model * transform * vec4(aPos.x, aPos.y, aPos.z,1.0)).xyz;
  //figure out this vertex's location after applying the necessary matrices.
  gl_Position = projection * view * vec4(fragment_position,1.0);
};