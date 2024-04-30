#version 330 core

out vec4 FragColor;

in vec2 texture_coordinates;

uniform sampler2D aTexture;
uniform vec4 transparentColor;

uniform float alpha;
uniform bool is_textured;
uniform vec4 color;

void main()
{
	if (is_textured == false) {
		FragColor = color;
	}
	else{
		vec4 tempColor = texture(aTexture,texture_coordinates);
		if (tempColor == transparentColor) {
			tempColor = vec4(1.0,1.0,1.0,alpha);
		}

		FragColor = tempColor;

	}
}
