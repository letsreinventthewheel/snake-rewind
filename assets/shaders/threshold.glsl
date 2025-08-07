#version 330 core

uniform sampler2D texture0;

in vec2 fragTexCoord;

out vec4 FragColor;

void main() {
    vec4 color = texture(texture0, fragTexCoord);
    float brightness = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));
    FragColor = brightness > 0.3 ? color : vec4(0);
}
