#version 330 core       //定点着色器
layout(location=0) in vec3 position;
layout(location=1) in vec2 textCoords;
out vec2 textcoords;
void main()
{
gl_Position=vec4(position, 1.0f);
textcoords=vec2(textCoords.x,1-textCoords.y);
};
