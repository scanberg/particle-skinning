#version 330

out vec4 out_frag0;

in vec2 TexCoord;
in vec3 Normal;

uniform sampler2D texture0;

void main(void)
{
	vec2 texCoord = TexCoord;
	vec3 normal = Normal;

	vec3 diffuse = texture(texture0, texCoord).rgb;
	out_frag0 = vec4(diffuse, 1);
}
