#include "texture.h"

static void setDefaultTextureGLOptions()
{
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

static void setTextureGLOptions(const TextureOptions* options)
{
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, options->horizontalWrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, options->verticalWrap);

    if (options->borderColor)
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, options->borderColor);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, options->minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, options->magFilter);

    if (options->swizzleMask)
        glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, options->swizzleMask);
}

GLuint createTexture(GLenum slot, GLsizei width, GLsizei height, void* data, GLenum dataType,
    GLenum dataFormat, GLint internalFormat, const TextureOptions* options)
{
    GLuint texture;
    glGenTextures(1, &texture);
    bindTexture(texture, slot);

    if (options)
        setTextureGLOptions(options);
    else
        setDefaultTextureGLOptions();

    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, dataType, data);

    return texture;
}

void bindTexture(GLuint texture, GLenum slot)
{
    glActiveTexture(slot);
    glBindTexture(GL_TEXTURE_2D, texture);
}

void freeTexture(GLuint texture)
{
    glDeleteTextures(1, &texture);
}
