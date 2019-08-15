#version 330 core
out vec4 FragColor;
in  vec3 vertexColor;
in vec2 TexCoord;

uniform sampler2D texSampler0;
uniform sampler2D texSampler1;

void main()
{
    //FragColor = vec4(vertexColor, 1.0);
    FragColor = mix(texture(texSampler0, TexCoord), texture(texSampler1, TexCoord), texture(texSampler1, TexCoord).a * 0.7);
}