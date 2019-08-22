#version 330 core

out vec4 FragColor;

in  vec4 vertexColor;

void main()
{
    vec2 circCoord = 2.0 * gl_PointCoord - 1.0;
    if (dot(circCoord, circCoord) > 1.0)
        discard;
    else
        FragColor = vec4(vertexColor.rgb, 1);
}