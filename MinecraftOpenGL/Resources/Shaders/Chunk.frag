#version 330 core

layout (location = 0) out vec4 color;

in vec2 passTextureCoord;
in vec3 passNormal;
in vec3 passWorldPos;
in float passLightLevel;
in float chunk;

uniform sampler2D tex;
uniform int u_Rebuilding = 0;
uniform int u_Dirty = 0;
uniform int u_ShouldBeRemoved = 0;

uniform vec3 u_LightPosition;
uniform vec3 u_CameraPosition;

void main()
{
	vec4 textureColor = texture(tex, passTextureCoord);

	float c = chunk;
	if (chunk != 0)
		c = (1 / chunk) * 100.0;
	else
		c += 0.5;

	//color = mix(textureColor, vec4(c, c, c, 1.0), 0.5);

	/*if (u_Dirty == 1)
		color = mix(textureColor, vec4(0, 0, 1, 1), 0.3);
	if (u_ShouldBeRemoved == 1)
		color = mix(textureColor, vec4(1, 0, 0, 1), 0.5);
	if (u_Dirty == 0&& u_Rebuilding == 0 && u_ShouldBeRemoved == 0)
		color = textureColor * vec4(passLightLevel, passLightLevel, passLightLevel, 1);*/

	// Light calculations

	vec3 lightColor = vec3(1.0, 0.6, 0.6);
	float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * lightColor;

	// If model matrix is not identity: https://learnopengl.com/Lighting/Basic-Lighting
	// Normal = mat3(transpose(inverse(model))) * passNormal;

	vec3 normalizedNormal = normalize(passNormal);
	vec3 lightDir = normalize(u_LightPosition - passWorldPos);


	float diff = max(dot(normalizedNormal, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	// specular
	float specularStrength = 0.5;
	vec3 viewDir = normalize(u_CameraPosition - passWorldPos);
	vec3 reflectDir = reflect(-lightDir, normalizedNormal);  

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrength * spec * lightColor;  

	color = vec4(ambient + diffuse + specular, 1.0) * textureColor;
		
	if (color.a == 0.0)
		discard;
}