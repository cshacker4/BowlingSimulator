#version 330 core

out vec4 FragColor;

in vec2 texture_coordinates;

uniform sampler2D aTexture;
uniform vec4 transparentColor;

uniform float alpha;

void main()
{
   vec4 tempColor = texture(aTexture,texture_coordinates);
   if (tempColor == transparentColor) {
       tempColor = vec4(1.0,0.0,0.0,alpha);
   }

   FragColor = tempColor;
}
