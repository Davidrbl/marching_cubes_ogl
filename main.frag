#version 460 core

layout (location = 0) out vec4 FragColor;

in float vs_depth;
flat in int vs_id;

void main(){

    vec3 col = vec3(
        (vs_id * 958729) % 9 / 9,
        (vs_id * 758928) % 9 / 9,
        (vs_id * 158975) % 9 / 9
    );

    col += 0.2;

    FragColor = vec4(col * 0.5 / vs_depth, 1.0);
    // FragColor = vec4(col, 1.0);
}
