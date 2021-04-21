#version 440 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColour;
layout (location = 2) in vec2 aTexCoord;

//vertex attributes
out vec4 vertexColour;
out vec4 vertex_Position;

//light attributes
out vec3 out_lightPosition;
out vec4 out_lightColour;
uniform vec4 lightColour;
uniform vec3 lightPosition;

//texture attributes
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 offsets[15];

void main()
{
   //calculate vertex positions (world space)
   vec3 offset = offsets[gl_InstanceID]+iter;

   vertex_Position = model * vec4(aPos+offset, 1.0f);
   
   //calculate vertex positions (view space)
   gl_Position =vertex_Position;//vec4(aPos, 1.0f);// projection * view * vertex_Position;

   //pass vertex colours to fshader
   vertexColour = aColour;

   //pass vertex colours to fshader
   out_lightPosition = lightPosition;
   out_lightColour = lightColour;

   //set Texture coordinates
   TexCoord = aTexCoord;
};