#version 330 core

layout(location = 0) in vec4 vertex; // vertex.xy: posición, vertex.zw: coordenadas de textura
out vec2 TexCoords;

uniform mat4 projection; // Matriz de proyección ortográfica para transformar a coordenadas de pantalla

void main() {
    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0); // Transforma la posición del vértice
    TexCoords = vertex.zw; // Pasa las coordenadas de textura al fragment shader
}
