#version 330 core
layout(location = 0) in vec2 aPos;  // ← 2D positions!

void main()
{
    gl_Position = vec4(aPos, 0.0, 1.0);  // z=0, w=1 for 2D overlay
}