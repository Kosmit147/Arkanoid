#include "gl.h"

#include <stdlib.h>
#include <assert.h>

#include "helpers.h"
#include "log.h"
#include "entities.h"
#include "texture.h"

#include "defines.h"

#ifdef _DEBUG
void rendererGLDebugCallback(GLenum UNUSED(source), GLenum UNUSED(type), GLuint UNUSED(id), GLenum severity,
    GLsizei UNUSED(length), const GLchar* message, const void* UNUSED(userParam))
{
    if (ARKANOID_GL_DEBUG_MESSAGE_MIN_SEVERITY != GL_DEBUG_SEVERITY_NOTIFICATION &&
        severity > ARKANOID_GL_DEBUG_MESSAGE_MIN_SEVERITY)
    {
        return;
    }

    switch (severity)
    {
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        logNotification("[OpenGL Notification]: %s.\n", message);
        break;
    case GL_DEBUG_SEVERITY_LOW:
    case GL_DEBUG_SEVERITY_MEDIUM:
        logWarning("[OpenGL Warning]: %s.\n", message);
        break;
    case GL_DEBUG_SEVERITY_HIGH:
        logError("[OpenGL Error]: %s.\n", message);
        break;
    }
}
#endif

GLuint genVA()
{
    GLuint VA;
    glGenVertexArrays(1, &VA);
    glBindVertexArray(VA);

    return VA;
}

GLuint genVB()
{
    GLuint VB;
    glGenBuffers(1, &VB);
    glBindBuffer(GL_ARRAY_BUFFER, VB);

    return VB;
}

GLuint genIB()
{
    GLuint IB;
    glGenBuffers(1, &IB);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);

    return IB;
}

static RectBounds getTextRendererCharTexCoords(char character, const BitmapFont* font)
{
    character -= font->offset;

    unsigned int col = (unsigned int)character % font->cols;
    unsigned int row = (unsigned int)character / font->cols;

    float offsetPerCol = 1.0f / (float)font->cols;
    float offsetPerRow = 1.0f / (float)font->rows;

    return (RectBounds) {
        .topLeft = (Vec2) {
            .x = 0.0f + (float)col * offsetPerCol,
            .y = 1.0f - (float)row * offsetPerRow,
        },
        .bottomRight = (Vec2) {
            .x = 1.0f - (float)(font->cols - col - 1) * offsetPerCol,
            .y = 0.0f + (float)(font->rows - row - 1) * offsetPerRow,
        },
    };
}

static void getTextRendererCharVertices(char character, const BitmapFont* font, float* vertices,
    Vec2 position, float charWidth, float charHeight)
{
    static_assert(TEXT_RENDERER_FLOATS_PER_CHAR_VERTEX == 4,
        "Expected TEXT_RENDERER_FLOATS_PER_CHAR_VERTEX == 4");

    float x1 = position.x;
    float x2 = position.x + charWidth;
    float y1 = position.y;
    float y2 = position.y - charHeight;

    RectBounds texCoords = getTextRendererCharTexCoords(character, font);

    vertices[0] = x1;                       vertices[1] = y1;
    vertices[2] = texCoords.topLeft.x;      vertices[3] = texCoords.topLeft.y;

    vertices[4] = x2;                       vertices[5] = y1;
    vertices[6] = texCoords.bottomRight.x;  vertices[7] = texCoords.topLeft.y;

    vertices[8] = x2;                       vertices[9] = y2;
    vertices[10] = texCoords.bottomRight.x; vertices[11] = texCoords.bottomRight.y;

    vertices[12] = x1;                      vertices[13] = y2;
    vertices[14] = texCoords.topLeft.x;     vertices[15] = texCoords.bottomRight.y;
}

static GLuint createTextRendererVB(const char* text, size_t textLength, const BitmapFont* font,
    Vec2 position, float charWidth, float charHeight)
{
    GLuint VB = genVB();

    float* vertices = malloc(TEXT_RENDERER_CHAR_VERTICES_SIZE * textLength);

    for (size_t i = 0; i < textLength; i++)
    {
        getTextRendererCharVertices(text[i], font, vertices + i * TEXT_RENDERER_FLOATS_PER_CHAR_VERTEX * 4,
            (Vec2){ .x = position.x + charWidth * (float)i, .y = position.y }, charWidth, charHeight);
    }

    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(TEXT_RENDERER_CHAR_VERTICES_SIZE * textLength), vertices,
        GL_STATIC_DRAW);

    free(vertices);

    return VB;
}

static void setTextRendererVertexAttributes()
{
    static_assert(TEXT_RENDERER_FLOATS_PER_CHAR_VERTEX == 4,
        "Expected TEXT_RENDERER_FLOATS_PER_CHAR_VERTEX == 4");

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * TEXT_RENDERER_FLOATS_PER_CHAR_VERTEX,
        NULL);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * TEXT_RENDERER_FLOATS_PER_CHAR_VERTEX,
        (void*)(sizeof(float) * 2));
    glEnableVertexAttribArray(1);
}

TextRenderer createTextRenderer(const char* text, size_t textLength, const BitmapFont* font, Vec2 position,
    float charWidth, float charHeight, unsigned int quadIB)
{
    TextRenderer renderer = {
        .VA = genVA(),
        .VB = createTextRendererVB(text, textLength, font, position, charWidth, charHeight),
        .charCount = textLength,
        .charWidth = charWidth,
        .charHeight = charHeight,
        .font = font,
    };

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadIB);
    setTextRendererVertexAttributes();

    return renderer;
}

void updateTextRenderer(TextRenderer* renderer, const char* newText, size_t newTextLength, Vec2 newPosition)
{
    glDeleteBuffers(1, &renderer->VB);

    renderer->VB = createTextRendererVB(newText, newTextLength, renderer->font, newPosition,
        renderer->charWidth, renderer->charHeight);
    renderer->charCount = newTextLength;

    setTextRendererVertexAttributes();
}

GLint retrieveUniformLocation(GLuint shader, const GLchar* name)
{
    GLint location = glGetUniformLocation(shader, name);

#ifdef _DEBUG
    if (location == -1)
        logWarning("[OpenGL Warning]: Uniform %s in shader %d does not exist!\n", name, shader);
#endif

    return location;
}

void drawElements(GLuint VA, GLsizei count, GLenum IBType)
{
    glBindVertexArray(VA);
    glDrawElements(GL_TRIANGLES, count, IBType, NULL);
}

void drawInstances(GLuint VA, GLsizei vertexCount, GLsizei instanceCount, GLenum IBType)
{
    glBindVertexArray(VA);
    glDrawElementsInstanced(GL_TRIANGLES, vertexCount, IBType, NULL, instanceCount);
}

// this function expects the appropriate shader to be bound
void renderText(const TextRenderer* renderer)
{
    bindTexture(renderer->font->textureID, GL_TEXTURE0);
    drawElements(renderer->VA, (GLsizei)renderer->charCount * 6, QUAD_IB_DATA_TYPE);
}

void moveDataWithinGLBuffer(GLenum bufferType, GLuint buffer, GLintptr dstOffset, GLintptr srcOffset,
    GLsizeiptr size)
{
    void* tmpData = malloc((size_t)size);

    glBindBuffer(bufferType, buffer);
    glGetBufferSubData(bufferType, srcOffset, size, tmpData);
    glBufferSubData(bufferType, dstOffset, size, tmpData);

    free(tmpData);
}

void moveObjectsWithinGLBuffer(GLenum bufferType, GLuint buffer, size_t dstIndex, size_t srcIndex,
    size_t count, size_t objSize)
{
    GLintptr srcOffset = (GLintptr)(objSize * srcIndex);
    GLintptr dstOffset = (GLintptr)(objSize * dstIndex);
    GLsizeiptr dataSize = (GLsizeiptr)(objSize * count);

    moveDataWithinGLBuffer(bufferType, buffer, dstOffset, srcOffset, dataSize);
}

void eraseObjectFromGLBuffer(GLenum bufferType, GLuint buffer, size_t index, size_t objectCount,
    size_t objSize)
{
    size_t objectsToMove = objectCount - index - 1;
    moveObjectsWithinGLBuffer(bufferType, buffer, index, index + 1, objectsToMove, objSize);
}

GLuint createQuadIB(size_t count, GLenum usage)
{
    static_assert(QUAD_IB_DATA_TYPE == GL_UNSIGNED_SHORT, "Expected QUAD_IB_DATA_TYPE == GL_UNSIGNED_SHORT");

    GLuint IB = genIB();

    GLsizeiptr dataSize = (GLsizeiptr)(sizeof(GLushort) * 2 * 3 * count);
    GLushort* indices = malloc((size_t)dataSize);

    GLushort vertexOffset = 0;

    for (GLushort i = 0; i < count * 6; i += 6)
    {
        // first triangle
        indices[i + 0] = vertexOffset + 0;
        indices[i + 1] = vertexOffset + 1;
        indices[i + 2] = vertexOffset + 2;

        // second triangle
        indices[i + 3] = vertexOffset + 0;
        indices[i + 4] = vertexOffset + 2;
        indices[i + 5] = vertexOffset + 3;

        vertexOffset += 4;
    }

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, dataSize, indices, usage);

    free(indices);

    return IB;
}

void freeQuadRenderer(const QuadRenderer* renderer)
{
    glDeleteVertexArrays(1, &renderer->VA);
    glDeleteBuffers(1, &renderer->VB);
}

void freeInstancedQuadRenderer(const InstancedQuadRenderer* renderer)
{
    glDeleteVertexArrays(1, &renderer->VA);
    glDeleteBuffers(1, &renderer->VB);
    glDeleteBuffers(1, &renderer->instanceBuffer);
}

void freeTextRenderer(const TextRenderer* renderer)
{
    glDeleteVertexArrays(1, &renderer->VA);
    glDeleteBuffers(1, &renderer->VB);
}
