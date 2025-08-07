#version 330 core

uniform sampler2D texture0;
uniform vec2 direction;
uniform float weight[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

in vec2 fragTexCoord;

out vec4 FragColor;

void main() {
    vec3 result = texture(texture0, fragTexCoord).rgb * weight[0];

    for (int i = 1; i < 5; i++) {
        vec2 offset = direction * float(i);
        result += texture(texture0, fragTexCoord + offset).rgb * weight[i];
        result += texture(texture0, fragTexCoord - offset).rgb * weight[i];
    }

    FragColor = vec4(result, 1.0);
}
