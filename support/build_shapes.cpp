#include "build_shapes.hpp"
#include "stb_image_implementation.hpp"

#include <vector>
#include <cmath>

unsigned int GetTexture (std::string texture_path, bool flipped)
{
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (flipped) {
        stbi_set_flip_vertically_on_load(true);
    } else {
        stbi_set_flip_vertically_on_load(false);
    }

    // load and generate the texture
    int width, height, nrChannels;
    unsigned char *data = stbi_load(texture_path.c_str(), &width, &height, &nrChannels, 0);
    int channel_type = GL_RGB;
    if (nrChannels == 4) {
        channel_type = GL_RGBA;
    }

    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, channel_type, width, height, 0, channel_type, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture: " << texture_path<<std::endl;
        return 0;
    }
    stbi_image_free(data);
    return texture;
}

BasicShape GetTexturedRectangle (VAOStruct vao, glm::vec3 bottom_left, float width, 
                                 float height,float texture_size, bool flipped)
{   
    

    glm::vec3 bottom_right = glm::vec3(bottom_left.x+width,bottom_left.y,bottom_left.z);
    glm::vec3 top_left = glm::vec3(bottom_left.x,bottom_left.y+height,bottom_left.z);

    glm::vec3 vec_one = bottom_right - bottom_left;
    glm::vec3 vec_two = top_left - bottom_left;
    glm::vec3 sn = glm::normalize(glm::cross(vec_one,vec_two));
    if(flipped) {
        sn = -sn;
    }
    float rect_vert[] {
       //X, Y, Z, S, T
       bottom_left.x, bottom_left.y, bottom_left.z, sn.x,sn.y,sn.z, 0.0, 0.0,
       bottom_left.x+width, bottom_left.y, bottom_left.z, sn.x,sn.y,sn.z,texture_size, 0.0,
       bottom_left.x+width, bottom_left.y+height, bottom_left.z, sn.x,sn.y,sn.z,texture_size, texture_size,
       bottom_left.x, bottom_left.y+height, bottom_left.z, sn.x,sn.y,sn.z,0.0, texture_size
    };
    
    BasicShape new_shape;
    new_shape.Initialize(vao,rect_vert,sizeof(rect_vert),4,GL_TRIANGLE_FAN);

    unsigned int indices[] = {0,1,2,3};
    //Set the EBO for the shape here.
    new_shape.InitializeEBO(indices,sizeof(indices),4);


    return new_shape;
}

BasicShape GetRectangle (VAOStruct vao, glm::vec3 bottom_left, float width, 
                                 float height,bool flipped)
{   
    

    glm::vec3 bottom_right = glm::vec3(bottom_left.x+width,bottom_left.y,bottom_left.z);
    glm::vec3 top_left = glm::vec3(bottom_left.x,bottom_left.y+height,bottom_left.z);

    glm::vec3 vec_one = bottom_right - bottom_left;
    glm::vec3 vec_two = top_left - bottom_left;
    glm::vec3 sn = glm::normalize(glm::cross(vec_one,vec_two));
    if(flipped) {
        sn = -sn;
    }
    float rect_vert[] {
       //X, Y, Z, S, T
       bottom_left.x, bottom_left.y, bottom_left.z, sn.x,sn.y,sn.z,
       bottom_left.x+width, bottom_left.y, bottom_left.z, sn.x,sn.y,sn.z,
       bottom_left.x+width, bottom_left.y+height, bottom_left.z, sn.x,sn.y,sn.z,
       bottom_left.x, bottom_left.y+height, bottom_left.z, sn.x,sn.y,sn.z
    };
    
    BasicShape new_shape;
    new_shape.Initialize(vao,rect_vert,sizeof(rect_vert),4,GL_TRIANGLE_FAN);

    unsigned int indices[] = {0,1,2,3};
    //Set the EBO for the shape here.
    new_shape.InitializeEBO(indices,sizeof(indices),4);


    return new_shape;
}

BasicShape GetTriangle (VAOStruct vao,glm::vec3 bottom_left, 
                                      glm::vec3 bottom_right, glm::vec3 top) {
    
    float tri_vert[] {
       bottom_left.x,bottom_left.y,bottom_left.z,//bottom left vertex
       bottom_right.x,bottom_right.y,bottom_right.z,  //bottom right vertex
       top.x,top.y,top.z  //top vertex
    };
    
    BasicShape new_shape;
    new_shape.Initialize(vao,tri_vert,sizeof(tri_vert),3,GL_TRIANGLES);

    unsigned int indices[] = {0,1,2};
    //Set the EBO for the shape here.
    new_shape.InitializeEBO(indices,sizeof(indices),3);


    return new_shape;
}

BasicShape GetTexturedTriangle (VAOStruct vao,glm::vec3 bottom_left, 
                                      glm::vec3 bottom_right, glm::vec3 top) {
    
    glm::vec3 vec_one = bottom_right - bottom_left;
    glm::vec3 vec_two = top - bottom_left;
    glm::vec3 sn = glm::cross(vec_one,vec_two);
    sn = glm::normalize(sn);
    //original approach didn't work (thanks Kevin Fernandez!)
    //  because...mapping across the x dimension doesn't help when the 
    //  bottom "left" and bottom "right" only change in y
    //fix?  below is one way...we want len(vec_two)*because dot(norm(vec_one),norm(vec_two))
    float cos_angle = glm::dot(glm::normalize(vec_one),glm::normalize(vec_two));




    float tri_vert[] {
       bottom_left.x,bottom_left.y,bottom_left.z, sn.x,sn.y,sn.z, 0.0, 0.0, //bottom left vertex
       bottom_right.x,bottom_right.y,bottom_right.z,sn.x,sn.y,sn.z, 1.0, 0.0,  //bottom right vertex
       top.x,top.y,top.z, sn.x,sn.y,sn.z, cos_angle,1.0  //top vertex
    };
    
    
    BasicShape new_shape;
    new_shape.Initialize(vao,tri_vert,sizeof(tri_vert),3,GL_TRIANGLES);

    unsigned int indices[] = {0,1,2};
    //Set the EBO for the shape here.
    new_shape.InitializeEBO(indices,sizeof(indices),3);


    return new_shape;
}

BasicShape GetHull (VAOStruct vao,glm::vec3 lower_left, float width, float height)
{
    float data[] = {
        lower_left.x, lower_left.y, 0.0f,//lower left
        lower_left.x+width, lower_left.y, 0.0f,//lower right
        lower_left.x+width, lower_left.y+height, 0.0f,//upper right
        lower_left.x+0.9f*width, lower_left.y+height+0.05f,0.0f,
        lower_left.x+0.8f*width, lower_left.y+height+0.09f,0.0f,
        lower_left.x+0.7f*width, lower_left.y+height+0.11f,0.0f,
        lower_left.x+0.6f*width, lower_left.y+height+0.13f,0.0f,
        lower_left.x+0.5f*width, lower_left.y+height+0.15f,0.0f,
        lower_left.x+0.4f*width, lower_left.y+height+0.13f,0.0f,
        lower_left.x+0.3f*width, lower_left.y+height+0.11f,0.0f,
        lower_left.x+0.2f*width, lower_left.y+height+0.09f,0.0f,
        lower_left.x+0.1f*width, lower_left.y+height+0.05f,0.0f,
        lower_left.x, lower_left.y+height, 0.0f
    };

    BasicShape new_shape;
    new_shape.Initialize(vao,data,sizeof(data),13,GL_TRIANGLE_FAN);

    unsigned int indices[] = {0,1,2,3,4,5,6,7,8,9,10,11,12};
    //Set the EBO for the shape here.
    new_shape.InitializeEBO(indices,sizeof(indices),13);


    return new_shape;
}

BasicShape GetTexturedHull (VAOStruct vao,glm::vec3 lower_left, float width, float height,float texture_size)
{
    float data[] = {
        // X,         Y,            Z,                           S,   T,
        lower_left.x, lower_left.y, 0.0f,                        0.0f, 0.0f, //lower left
        lower_left.x+width, lower_left.y, 0.0f,                  1.0f, 0.0f,//lower right
        lower_left.x+width, lower_left.y+height, 0.0f,           1.0f, 0.85f,//upper right
        lower_left.x+0.9f*width, lower_left.y+height+0.05f,0.0f, 0.9f, 0.90f,
        lower_left.x+0.8f*width, lower_left.y+height+0.09f,0.0f, 0.8f, 0.94f,
        lower_left.x+0.7f*width, lower_left.y+height+0.11f,0.0f, 0.7f, 0.96f,
        lower_left.x+0.6f*width, lower_left.y+height+0.13f,0.0f, 0.6f, 0.98f,
        lower_left.x+0.5f*width, lower_left.y+height+0.15f,0.0f, 0.5f, 1.0f,
        lower_left.x+0.4f*width, lower_left.y+height+0.13f,0.0f, 0.4f, 0.98f,
        lower_left.x+0.3f*width, lower_left.y+height+0.11f,0.0f, 0.3f, 0.96f,
        lower_left.x+0.2f*width, lower_left.y+height+0.09f,0.0f, 0.2f, 0.94f,
        lower_left.x+0.1f*width, lower_left.y+height+0.05f,0.0f, 0.1f, 0.90f,
        lower_left.x, lower_left.y+height, 0.0f,                 0.0f, 0.85f
    };

    BasicShape new_shape;
    new_shape.Initialize(vao,data,sizeof(data),13,GL_TRIANGLE_FAN);

    unsigned int indices[] = {0,1,2,3,4,5,6,7,8,9,10,11,12};
    //Set the EBO for the shape here.
    new_shape.InitializeEBO(indices,sizeof(indices),13);


    return new_shape;
}

BasicShape GetCircle (VAOStruct vao, float radius, int points, glm::vec3 location) {
    float circ_dat[points*3]{};
    float angle = 0.0;
    for(int i = 0; i<points; i++) {
        circ_dat[i*3] = location.x + radius*cos(glm::radians(angle));
        circ_dat[i*3+1] = location.y + radius*sin(glm::radians(angle));
        circ_dat[i*3+2] = 0.0f;
        angle+=360.0f/points;
    }
    
    BasicShape new_shape;
    new_shape.Initialize(vao,circ_dat,sizeof(circ_dat),points,GL_TRIANGLE_FAN);

    //for use with the EBO
    unsigned int indices[points]{};
    for(int i = 0; i<points; i++)
    {
        indices[i] = i;
    }
    //set the ebo for the shape here
    new_shape.InitializeEBO(indices,sizeof(indices),points);
    return new_shape;
}

BasicShape GetTexturedCircle (VAOStruct vao, float radius, int points, glm::vec3 location) {
    float circ_dat[points*5]{};
    float angle = 0.0;
    for(int i = 0; i<points; i++) {
        circ_dat[i*5] = location.x + radius*cos(glm::radians(angle));
        circ_dat[i*5+1] = location.y + radius*sin(glm::radians(angle));
        circ_dat[i*5+2] = 0.0f;
        circ_dat[i*5+3] = (1 + cos(glm::radians(angle)))/2.0;
        circ_dat[i*5+4] = (1 + sin(glm::radians(angle)))/2.0;
        angle+=360.0f/points;
    }
    
    BasicShape new_shape;
    new_shape.Initialize(vao,circ_dat,sizeof(circ_dat),points,GL_TRIANGLE_FAN);

    //for use with the EBO
    unsigned int indices[points]{};
    for(int i = 0; i<points; i++)
    {
        indices[i] = i;
    }
    //set the ebo for the shape here
    new_shape.InitializeEBO(indices,sizeof(indices),points);
    return new_shape;
}


BasicShape GetStars (VAOStruct vao, int number_stars)
{

    float data[number_stars*3]{0.0};

    for (int i = 0; i < number_stars; i+=3) {
        data[i] = (((rand() % 100)-50)/50.0);
        data[i+1] = (((rand()%100)-50)/50.0);
    }

    BasicShape new_shape;
    new_shape.Initialize(vao,data,sizeof(data),number_stars,GL_POINTS);
    return new_shape;


    return new_shape;

}

//Accepts a vector of strings as input and a boolean value (for flipping the texture on load...default is false) and returns an unsigned integer
//unsigned GetCubeMap (const std::vector<std::string>& faces, bool flipped = false);
unsigned GetCubeMap (const std::vector<std::string>& faces, bool flipped)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	
	if (flipped) {
		stbi_set_flip_vertically_on_load(true);
	} else {
		stbi_set_flip_vertically_on_load(false);
	}

	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
					0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
				    );
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}

	//reset flipping
	stbi_set_flip_vertically_on_load(false);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

BasicShape GetCube(VAOStruct vao){
	BasicShape new_shape;
	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};
	//Initializes a BasicShape object given a VAOStruct, a pointer to vertex data, and integer for the number
        //of bytes in the data, an int for the number of vertices, and an int for the primitive used (default is GL_TRIANGLES) 
        //void Initialize(VAOStruct vao, float* vertices, int vertices_bytes, int num_vertices, GLuint prim = GL_TRIANGLES);

	new_shape.Initialize(vao, skyboxVertices, sizeof(skyboxVertices), sizeof(skyboxVertices)/sizeof(skyboxVertices[0]));
	return new_shape;
};
