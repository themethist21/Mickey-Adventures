#include "Hitbox.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

Hitbox::Hitbox(const glm::vec2& topLeftPosition, const glm::vec2& size, ShaderProgram* program)
{
    // Almacenar posición y tamaño
    position = topLeftPosition;
    this->size = size;

    // Definir los vértices del quad (posiciones)
    float vertices[] = {
        // Posiciones
        0.0f, 0.0f,           // Superior izquierda
        1.0f, 0.0f,           // Superior derecha
        1.0f, 1.0f,           // Inferior derecha
        0.0f, 1.0f            // Inferior izquierda
    };

    // Índices para los dos triángulos que forman el quad
    unsigned int indices[] = {
        0, 1, 2,  // Primer triángulo
        2, 3, 0   // Segundo triángulo
    };

    // Generar y enlazar el Vertex Array Object
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Generar y enlazar el Vertex Buffer Object
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Generar y enlazar el Element Buffer Object
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Especificar el layout de los datos de los vértices
    GLint posAttrib = program->bindVertexAttribute("position", 2, 2 * sizeof(float), 0);
    // Habilitar el atributo
    glEnableVertexAttribArray(posAttrib);

    // Desenlazar el VAO
    glBindVertexArray(0);

    // Almacenar el programa de shader
    shaderProgram = program;

    // Inicializar la matriz modelo
    updateModelMatrix();
}


void Hitbox::render()
{
    // Usar el programa de shader
    shaderProgram->use();

    // Establecer el color uniforme a rojo semitransparente
    shaderProgram->setUniform4f("color", 1.0f, 0.0f, 0.0f, 0.5f); // RGBA

    shaderProgram->setUniformMatrix4f("modelview", modelMatrix);

    // Bind del Vertex Array Object
    glBindVertexArray(vao);

    // Dibujar el quad
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // Desenlazar el VAO
    glBindVertexArray(0);
}



void Hitbox::setPosition(const glm::vec2& topLeftPosition)
{
    position = topLeftPosition;
    updateModelMatrix();
}

void Hitbox::setSize(const glm::vec2& newSize)
{
    size = newSize;
    updateModelMatrix();
}

void Hitbox::updateModelMatrix()
{
    // La hitbox originalmente está definida en el rango [0,1], así que escalamos por el tamaño y trasladamos a la posición
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(position, 0.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(size, 1.0f));

}

glm::vec2 Hitbox::getPosition()
{
	return position;
}

glm::vec2 Hitbox::getSize()
{
	return size;
}

