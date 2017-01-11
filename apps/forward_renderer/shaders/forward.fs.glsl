#version 330

in vec3 vViewSpacePosition;
in vec3 vViewSpaceNormal;
in vec3 vTexCoords;

out vec3 fColor;

uniform mat4 uModelViewProjMatrix;

void main()
{
	fColor = vViewSpaceNormal;
}