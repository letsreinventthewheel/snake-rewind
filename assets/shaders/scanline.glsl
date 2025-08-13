#version 330 core

uniform sampler2D texture0;
uniform float time;

in vec2 fragTexCoord;

out vec4 FragColor;

void main() {
    float distortion = sin(fragTexCoord.y * 2304978 * fract(time)) * 0.0007;
    vec2 distortedCoord = vec2(fragTexCoord.x + distortion, fragTexCoord.y);
    float scanline = cos((fract((fragTexCoord.y + time * 0.05) * 150 + 0.5) - 0.5) * 3.14);

    vec4 color = vec4(1.0);
    vec2 offset = vec2(distortion * 3, 0);
    color.r = texture(texture0, distortedCoord - offset).r;
    color.g = texture(texture0, distortedCoord).g;
    color.b = texture(texture0, distortedCoord + offset).b;

    float flicker = 0.95 + 0.05 * sin(time * 10); // 0.9 - 1.0
    FragColor = vec4(mix(color.rgb, vec3(scanline), 0.1) * flicker, 1.0);
}
