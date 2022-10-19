#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoords;
layout(location = 2) in vec4 colors;

uniform mat4 u_MVP;


void main(){
	gl_Position = u_MVP * vec4(position.x, position.y, position.z, 1.0f);

}