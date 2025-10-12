#version 330 core

in vec3 vNormal;
in vec2 vTexCoord;

out vec4 FragColor;

uniform sampler2D uTexture;
uniform vec3 uLightDir;
uniform vec3 uAmbient;

void main() {
    // 简单的漫反射光照
    vec3 normal = normalize(vNormal);
    float diff = max(dot(normal, uLightDir), 0.0);
    
    vec3 ambient = uAmbient;
    vec3 diffuse = vec3(diff);
    
    vec4 texColor = texture(uTexture, vTexCoord);
    vec3 result = (ambient + diffuse) * texColor.rgb;
    
    FragColor = vec4(result, texColor.a);
}
