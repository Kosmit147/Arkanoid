#include "gl.h"

#include <stdlib.h>

#include "helpers.h"
#include "log.h"

#include "defines.h"

#ifdef _DEBUG
void rendererGLDebugCallback(GLenum unused(source), GLenum unused(type), GLuint unused(id), GLenum severity,
    GLsizei unused(length), const GLchar* message, const void* unused(userParam))
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

unsigned int genVA()
{
    unsigned int VA;
    glGenVertexArrays(1, &VA);
    glBindVertexArray(VA);

    return VA;
}

unsigned int genVB()
{
    unsigned int VB;
    glGenBuffers(1, &VB);
    glBindBuffer(GL_ARRAY_BUFFER, VB);

    return VB;
}

unsigned int genIB()
{
    unsigned int IB;
    glGenBuffers(1, &IB);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);

    return IB;
}

int retrieveUniformLocation(unsigned int shader, const char* name)
{
    int location = glGetUniformLocation(shader, name);

#ifdef _DEBUG
    if (location == -1)
        logWarning("[OpenGL Warning]: Uniform %s in shader %d does not exist!\n", name, shader);
#endif

    return location;
}

void drawVertices(unsigned int VA, int count, GLenum IBType)
{
    glBindVertexArray(VA);
    glDrawElements(GL_TRIANGLES, count, IBType, NULL);
}

void moveDataWithinGLBuffer(GLenum bufferType, unsigned int buffer, GLintptr dstOffset,
    GLintptr srcOffset, GLsizeiptr size)
{
    void* tmpData = malloc((size_t)size);

    glBindBuffer(bufferType, buffer);
    glGetBufferSubData(bufferType, srcOffset, size, tmpData);
    glBufferSubData(bufferType, dstOffset, size, tmpData);

    free(tmpData);
}

void moveObjectsWithinGLBuffer(GLenum bufferType, unsigned int buffer, size_t dstIndex,
    size_t srcIndex, size_t count, size_t objSize)
{
    GLintptr srcOffset = (GLintptr)(objSize * srcIndex);
    GLintptr dstOffset = (GLintptr)(objSize * dstIndex);
    GLsizeiptr dataSize = (GLsizeiptr)(objSize * count);

    moveDataWithinGLBuffer(bufferType, buffer, dstOffset, srcOffset, dataSize);
}

void eraseObjectFromGLBuffer(GLenum bufferType, unsigned int buffer, size_t index, size_t objectCount, size_t objSize)
{
    size_t objectsToMove = objectCount - index - 1;
    moveObjectsWithinGLBuffer(bufferType, buffer, index, index + 1, objectsToMove, objSize);
}

unsigned int createQuadIB(size_t count, GLenum usage)
{
    unsigned int IB = genIB();

    size_t dataSize = sizeof(unsigned short) * 2 * 3 * count;
    unsigned short* indices = malloc(dataSize);

    unsigned short vertexOffset = 0;

    for (unsigned short i = 0; i < count * 6; i += 6)
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

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizei)dataSize, indices, usage);

    free(indices);

    return IB;
}

void freeGLQuad(const GLQuad* quad)
{
    glDeleteVertexArrays(1, &quad->VA);
    glDeleteBuffers(1, &quad->VB);
}
