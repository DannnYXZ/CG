#version 330 core
out vec4 FragColor;
in  vec3 vertexColor;
in vec2 TexCoord;

uniform sampler2D texSampler0;
uniform sampler2D texSampler1;

void main()
{
    vec4 x = texture(texSampler1, vec2(TexCoord.s, TexCoord.t));
    //FragColor = vec4(vertexColor, 1.0);
    FragColor = mix(texture(texSampler0, TexCoord),
                    x,
                    x.a * 0.7) * vec4(vertexColor, 1.0);
}