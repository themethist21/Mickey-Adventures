#version 330

layout(location = 0) in vec2 position;

uniform mat4 modelview;
uniform mat4 projection;

void main()
{
	// Transform position from pixel coordinates to clipping coordinates
	gl_Position = projection * modelview * vec4(position, 0.0, 1.0);
}
