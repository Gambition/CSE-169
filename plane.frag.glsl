#version 330 core
in vec3 mynormal;
in vec4 myvertex;
in vec2 TexCoords; 

out vec4 fragColor;
void main(){
	fragColor = vec4(0.9f,0.9f,0.9f,1.0f);
}