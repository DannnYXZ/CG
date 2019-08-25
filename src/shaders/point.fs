#version 330 core

out vec4 FragColor;

in  vec4 vertexColor;

void main()
{
    vec2 circCoord = 2 * gl_PointCoord - 1;
    if (dot(circCoord, circCoord) > 1)
        discard;
    else
        FragColor = vec4(vertexColor.rgb, 1);
}