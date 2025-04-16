#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <map>
#include <vector>
#include <iostream>
#include <string>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <ft2build.h>
#include FT_FREETYPE_H


#include "ShaderProgram.h" // Asegúrate de tener una clase Shader para manejar los shaders

struct Character {
    GLuint TextureID;   // ID de la textura del carácter
    glm::ivec2 Size;    // Tamaño del carácter
    glm::ivec2 Bearing; // Desplazamiento desde la línea de base
    GLuint Advance;     // Espacio para el siguiente carácter
};

class TextRenderer {
public:
    // Constructor que inicializa FreeType y carga la fuente
    TextRenderer(const std::string& font, GLuint screenWidth, GLuint screenHeight, int fontSize);

    // Destructor para limpiar los recursos
    ~TextRenderer();

    // Renderiza el texto en pantalla
    void renderText(const std::string& text, GLfloat x, GLfloat y, GLfloat scale, glm::vec4 color);

private:
    std::map<GLchar, Character> Characters; // Mapa de caracteres
    GLuint VAO, VBO;                        // VAO y VBO para el texto
    ShaderProgram shader;                          // Shader para renderizar texto
    //glm::mat4 projection;                   // Matriz de proyección

    // Carga la fuente usando FreeType
    void loadFont(const std::string& font, int fontSize);
    void initShader();
    // Configura VAO y VBO para el renderizado
    void initRenderData();
};

#endif
#pragma once
