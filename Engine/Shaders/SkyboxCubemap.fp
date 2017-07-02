in vec4 v2f_color;
in vec3 v2f_texCoord;

out vec4 o_fragColor : FRAG_COLOR;

uniform vec4 tint;
uniform float exposure;

uniform samplerCube skyCubeMap;

void main() {
    vec3 color = tint.rgb * texCUBE(skyCubeMap, v2f_texCoord).xyz;
    color *= exposure
    o_fragColor = v2f_color * vec4(color, 1.0);
}
