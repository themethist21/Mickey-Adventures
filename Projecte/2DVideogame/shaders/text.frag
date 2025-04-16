#version 330 core

in vec2 TexCoords;
out vec4 outColor;

uniform sampler2D text;     // Textura del glifo
uniform vec4 textColor;     // Color del texto

void main() {
    // Obtiene el valor de opacidad desde el canal rojo
    float alpha = texture(text, TexCoords).r; // Usa el canal rojo como alfa
    if (alpha < 0.7f)
        discard; // Descarta fragmentos casi transparentes para mejor apariencia del borde del texto

    // Aplica el color del texto multiplicado por el alfa del glifo
    outColor = textColor * vec4(1.0, 1.0, 1.0, alpha);
}
