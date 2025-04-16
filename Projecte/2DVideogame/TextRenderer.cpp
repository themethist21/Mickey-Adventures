#include "TextRenderer.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

TextRenderer::TextRenderer(const std::string& font, GLuint screenWidth, GLuint screenHeight, int fontSize){
    // Configura la matriz de proyección ortográfica
    // projection = glm::ortho(0.0f, static_cast<GLfloat>(screenWidth), 0.0f, static_cast<GLfloat>(screenHeight));
    //shader.use();
   // shader.setUniformMatrix4f("projection", projection);
	initShader();
	shader.use();
	shader.setUniformMatrix4f("projection", glm::ortho(0.0f, static_cast<GLfloat>(screenWidth), 0.0f, static_cast<GLfloat>(screenHeight)));
    // Carga la fuente y configura el VAO/VBO
    loadFont(font, fontSize);
    initRenderData();
}

TextRenderer::~TextRenderer() {
    // Liberar las texturas de cada carácter
    for (auto& pair : Characters) {
        glDeleteTextures(1, &pair.second.TextureID);
    }

    // Eliminar VAO y VBO
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void TextRenderer::loadFont(const std::string& font, int fontSize) {
    // Inicializa FreeType y carga la fuente

    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        std::cerr << "No se pudo inicializar FreeType Library" << std::endl;
        return;
    }

    FT_Face face;
    if (FT_New_Face(ft, font.c_str(), 0, &face)) {
        std::cerr << "No se pudo cargar la fuente" << std::endl;
        FT_Done_FreeType(ft);
        return;
    }
    FT_Set_Pixel_Sizes(face, 0, fontSize); // Tamaño de fuente

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Desactiva el empaquetado de bytes

    // Carga cada carácter y lo almacena en `Characters`
    for (GLubyte c = 0; c < 128; c++) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            std::cerr << "Error al cargar carácter " << c << std::endl;
            continue;
        }

        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );

        if (texture == 0) {
            std::cerr << "Error al crear textura para el carácter: " << c << std::endl;
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // Horizontal
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Vertical

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x
        };
        Characters.insert(std::pair<GLchar, Character>(c, character));
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

void TextRenderer::initRenderData() {
    // Configura VAO y VBO para el renderizado de texto
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void TextRenderer::renderText(const std::string& text, GLfloat x, GLfloat y, GLfloat scale, glm::vec4 color) {
    shader.use();
    shader.setUniform4f("textColor", color.x, color.y, color.z, color.w); // Color blanco
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    for (auto c = text.begin(); c != text.end(); c++) {
        Character ch = Characters[*c];
        GLfloat xpos = x + ch.Bearing.x * scale;
        GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        GLfloat w = ch.Size.x * scale;
        GLfloat h = ch.Size.y * scale;

        GLfloat vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };

        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        x += (ch.Advance >> 6) * scale;
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void TextRenderer::initShader()
{
    Shader vShader, fShader;

    // Shader para texturas (el existente)
    vShader.initFromFile(VERTEX_SHADER, "shaders/text.vert");
    if (!vShader.isCompiled())
    {
        cout << "Vertex Shader Error" << endl;
        cout << "" << vShader.log() << endl << endl;
    }
    fShader.initFromFile(FRAGMENT_SHADER, "shaders/text.frag");
    if (!fShader.isCompiled())
    {
        cout << "Fragment Shader Error" << endl;
        cout << "" << fShader.log() << endl << endl;
    }
    shader.init();
    shader.addShader(vShader);
    shader.addShader(fShader);
    shader.link();
    if (!shader.isLinked())
    {
        cout << "Shader Linking Error" << endl;
        cout << "" << shader.log() << endl << endl;
    }
    shader.bindFragmentOutput("outColor");
    vShader.free();
    fShader.free();
}