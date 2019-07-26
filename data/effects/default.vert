#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform SceneUBO {
    mat4 view;
    mat4 proj;
} scene;

layout(set = 1, binding = 0) uniform MaterialUBO {
    mat4 modl;
} material;

layout(location = 0) in vec3 iPosition;
layout(location = 1) in vec2 iTexCoord;
layout(location = 2) in vec3 iNormal;
layout(location = 3) in vec3 iTangent;
layout(location = 4) in vec3 iBinormal;
layout(location = 5) in vec4 iColor;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 uv;

void main() {
    gl_Position = scene.proj * scene.view * material.modl * vec4(iPosition, 1.0);
    uv = iTexCoord;
    fragColor = iColor;
}