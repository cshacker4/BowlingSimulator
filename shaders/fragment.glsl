#version 330 core
//Need to send the fragment color to the next step in the pipeline
out vec4 FragColor;

//bring in the texture image coordinates (if you have them)
in vec2 texture_coordinates;

//bring in the normal and the fragment position (world)
in vec3 norm;
in vec3 fragment_position;

//bring in the imported information
in vec3 diffuse_color;
in vec3 specular_color;

//bring in the integer value (0 or 1) telling us if the shape includes
//  texture data or not.
flat in int textured;
flat in int vertex_material;

//if it isn't textured, assume the value of set_color is set.
uniform vec4 set_color;

uniform bool screen;

//if it -is- textured, assume this 2D texture is mapped.
uniform sampler2D ourTexture;

// uniform float ambient_strength;
// uniform vec4 light_color;
// uniform vec4 light_position;
uniform vec4 view_position;

struct PointLight {
    vec4 position;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    bool on;
};

struct Spotlight {
	vec4 position;
	vec4 direction;
	vec4 color;
	float cut_off;
};

//lights automatically attenuate with distance
uniform PointLight point_light_list[10];

uniform PointLight blue_light;
uniform PointLight white_light;
uniform Spotlight spotlight;

vec4 CalcPointLight (PointLight light,vec3 norm,vec3 frag,vec3 eye);


void main()
{

	vec4 blue_light_color = CalcPointLight(blue_light,norm,
			fragment_position,view_position.xyz);
	vec4 white_light_color = CalcPointLight(white_light,norm,
			fragment_position,view_position.xyz);

	float white_light_distance = length(white_light.position.xyz - fragment_position);
	float white_light_attenuation = 1.0 / (1.0 + 0.045 * white_light_distance + 0.0075 * white_light_distance * white_light_distance);

	vec3 direction_to_spotlight = normalize(vec3(spotlight.position) - fragment_position);
    	float theta = dot(spotlight.direction.xyz, normalize(-direction_to_spotlight));

	//if it isn't textured, set the output color to the color we set.
	if (textured == 0) {
		FragColor = vec4(1.0,1.0,1.0,1.0);
	}
	//get the color from the texture
	else {
		//if it -is- textured set the color to the appropriate color in the texture
		//  according to the texture_coordinates.
		FragColor = texture(ourTexture,texture_coordinates);
	}

	//if the shape is a screen, set the color to the set color.
	if (screen == true) {
		return;
	}

	//handle the point lights
	vec4 final_color = vec4(0.0,0.0,0.0,1.0);
	int num_point_lights = point_light_list.length();
	for (int i = 0; i < num_point_lights; i++) {
		vec4 light_color = CalcPointLight(point_light_list[i],norm,
			fragment_position,view_position.xyz);
		float light_distance = length(point_light_list[i].position.xyz - fragment_position);
		float attenuation = 1.0 / (1.0 + 0.045 * light_distance + 0.0075 * light_distance * light_distance);
		final_color += light_color * attenuation;

	}
	FragColor = final_color * FragColor;
	//FragColor = (blue_light_color + white_light_attenuation * white_light_color) * FragColor;

	if (theta > cos(radians(spotlight.cut_off))) {
		float intensity = 1.0 / (1.0 + 0.045 * white_light_distance+ 0.0075 * white_light_distance * white_light_distance);
		FragColor = intensity * spotlight.color + FragColor;
	}


};

vec4 CalcPointLight (PointLight light,vec3 norm,vec3 frag,vec3 eye) {

    if (false == light.on) {
        return vec4(0.0,0.0,0.0,1.0);
    }
    vec3 light_direction = light.position.xyz - frag.xyz;
    light_direction = normalize(light_direction);
    vec3 normal = normalize(norm);
    float diffuse_coeff = max(dot(normal,light_direction),0.0);

    vec3 view_direction = normalize(view_position.xyz-frag);
    vec3 reflect_direction = reflect(-light_direction,normal);
    float spec_coeff = pow(max(dot(view_direction,reflect_direction),0.0),256.0);

    //handle materials
    if (vertex_material != 0) {
        return (light.ambient * vec4(diffuse_color,1.0) 
                + diffuse_coeff * light.diffuse * vec4(diffuse_color,1.0)
                + spec_coeff * light.specular * vec4(diffuse_color,1.0));
    } else {
        return (light.ambient 
            + diffuse_coeff * light.diffuse 
            + spec_coeff * light.specular);
    }




}
