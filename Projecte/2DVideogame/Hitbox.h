// Hitbox.h

#ifndef HITBOX_H
#define HITBOX_H

#include <glm/glm.hpp>
#include <GL/glew.h>
#include "ShaderProgram.h"

class Hitbox {
public:
    // Constructor: inicializa la hitbox con la posici�n superior izquierda y el tama�o
    Hitbox(const glm::vec2& topLeftPosition, const glm::vec2& size, ShaderProgram* program);
    void setPosition(const glm::vec2& topLeftPosition);
    void setSize(const glm::vec2& newSize);
	glm::vec2 getPosition();
	glm::vec2 getSize();

    // M�todo para renderizar la hitbox
    void render();

private:
    void updateModelMatrix();


    // Identificadores de OpenGL
    GLuint vao;            // Vertex Array Object
    GLuint vbo;            // Vertex Buffer Object
    GLuint ebo;            // Element Buffer Object

	glm::mat4 modelMatrix; // Matriz modelo

    // Programa de shader
    ShaderProgram* shaderProgram;

    // Atributos de la hitbox
    glm::vec2 position;    // Posici�n superior izquierda
    glm::vec2 size;        // Tama�o de la hitbox

};

#endif // HITBOX_H
