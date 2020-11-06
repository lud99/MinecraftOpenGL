#version 330 core

layout (location = 0) out vec4 color;

vec4 fogColor

vec4 calcFog(vec3 pos, vec4 colour, Fog fog)
{
    float distance = length(pos);
    float fogFactor = 1.0 / exp( (distance * fog.density)* (distance * fog.density));
    fogFactor = clamp( fogFactor, 0.0, 1.0 );

    vec3 resultColour = mix(fog.colour, colour.xyz, fogFactor);
    return vec4(resultColour.xyz, colour.w);
}

void main()
{
	calcFog(vec3(0.5, 0.5, 0.5), 0.15);

	color = vec4(0.5, 0.5, 0.5, 1.0);

	if (color.a == 0.0)
		discard;
}