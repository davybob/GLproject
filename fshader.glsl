#version 440 core
out vec4 FragColour;

in vec4 vertexColour;
in vec4 vertex_Position;

in vec3 out_lightPosition;
in vec4 out_lightColour;

uniform sampler2D ourTexture;
in vec2 TexCoord;

float intensity(in vec3 center, in vec3 vertexPosition) {
	//Find normal vector to face
	vec3 difference = vertexPosition - center;

	float u = dot(difference, difference);
	if (u < 1) {
		return 1.0f;
	}
	return 1/(u);
};


void main()
{
   FragColour = vertexColour;//*out_lightColour;//*intensity(out_lightPosition, vertex_Position.xyz);
   FragColour = FragColour;// * texture(ourTexture, TexCoord);
};