#version 330 core


void main()
{
	gl_FragDepth = gl_FragCoord.z;
	gl_FragDepth += gl_FrontFacing ? 0.0001 : 0.0;
}