#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTex;

out vec3 color;
out vec2 texCoord; // Changed from textCoord to texCoord

uniform float scale;

void main()
{
   gl_Position = vec4(aPos * (1.0 + scale), 1.0); // Simplified scaling
   color = aColor;
   texCoord = aTex; // Consistent name with fragment shader
}
