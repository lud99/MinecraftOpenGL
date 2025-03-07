#version 410 core

void main()
{
    // This is done implicitly, so no need for any code
     gl_FragDepth = gl_FragCoord.z;
}