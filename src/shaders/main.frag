#version 460 core

layout (location = 0) out vec4 FragColor;

in float vs_depth;

void main(){
    float col = .2 / vs_depth;
    FragColor = vec4(vec3(col), 1.0);
}
