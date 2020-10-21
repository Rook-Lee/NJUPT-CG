#version 330 core
in vec2 textcoords;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float Time;
out vec4 color;
void main()
{
color=mix(texture(texture1,textcoords), texture(texture2, vec2(1- textcoords.x, textcoords.y)),Time);
}