#ifndef BUILD_SHAPES_HPP
#define BUILD_SHAPES_HPP

#include "../classes/basic_shape.hpp"

//Loads a texture and returns an identifier for that texture given a string input 
// representing the filepath/name for the texture file input.
unsigned int GetTexture (std::string texture_path, bool flipped = false);

//Creates and returns a BasicShape rectangle given a VAO, a bottom left point, a width,
// a height, and a texture size used to cause a wrapping effect
BasicShape GetTexturedRectangle (VAOStruct vao, glm::vec3 bottom_left, float width, float height,float texture_size = 1.0, bool flipped = false);

//Creates and returns a BasicShape rectangle given a VAO, a bottom left point, a width,
// a height, and boolean indicating if the surface normal should be flipped or not.
BasicShape GetRectangle (VAOStruct vao, glm::vec3 bottom_left, float width, float height,bool flipped = false);

//Creates and returns a BasicShape triangle given a VAO and three points
BasicShape GetTriangle (VAOStruct vao,glm::vec3 bottom_left, 
                                      glm::vec3 bottom_right, glm::vec3 top);

//Creates and returns a BasicShape triangle given a VAO and three points
BasicShape GetTexturedTriangle (VAOStruct vao,glm::vec3 bottom_left, 
                                      glm::vec3 bottom_right, glm::vec3 top);

//Creates and returns a BasicShape ship hull given a VAO, a lower left point, a width, and a height
BasicShape GetHull (VAOStruct vao,glm::vec3 lower_left, float width, float height);

//Creates and returns a BasicShape ship hull given a VAO, a lower left point, a width, and a height
BasicShape GetTexturedHull (VAOStruct vao,glm::vec3 lower_left, float width, float height, float texture_size = 1.0);


//Creates and returns a BasicShape circle aligned to the xy plane given a VAO, a float radius, the number of points to render, and a location (centered)
BasicShape GetCircle (VAOStruct vao, float radius=1.0, int points= 40, glm::vec3 location = glm::vec3(0.0));

//Creates and returns a BasicShape circle aligned to the xy plane given a VAO, a float radius, the number of points to render, and a location (centered)
BasicShape GetTexturedCircle (VAOStruct vao, float radius=1.0, int points= 40, glm::vec3 location = glm::vec3(0.0));


//Creates and returns a BasicShape star field given the number of random numbers to display
BasicShape GetStars (VAOStruct vao, int number_stars);

//Build a cube with a VAO, origin, and distance
BasicShape GetCube (VAOStruct vao, glm::vec3 origin, float distance = 1.0);

#endif //BUILD_SHAPES_HPP
