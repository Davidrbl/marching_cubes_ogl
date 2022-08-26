#version 460 core

layout (location = 0) in vec3 a_pos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out float vs_depth;
flat out int vs_id;

void main(){
    gl_Position = projection * view * model * vec4(a_pos, 1.0);
    vs_depth = length((view * model * vec4(a_pos, 1.0)).xyz);
    vs_id = gl_VertexID;
    // gl_Position = vec4(a_pos, 1.0) * model * view * projection;
}