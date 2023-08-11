#include "VBO.h"
#include "buffer.h"
#include <GL/glew.h>

VBO::VBO(){}

void VBO::Init(GLsizeiptr pByteSize, GLfloat * pPtrElement, GLuint pDrawType) {
    glGenBuffers(1, &aID);
    Bind();
    glBufferData(GL_ARRAY_BUFFER, pByteSize, pPtrElement, pDrawType);
}
VBO::VBO(GLsizeiptr pByteSize, GLfloat * pPtrElement, GLuint pDrawType) {
    Init(pByteSize, pPtrElement, pDrawType);
}

void VBO::Bind() {
    glBindBuffer(GL_ARRAY_BUFFER, aID);
}

void VBO::Unbind() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VBO::Delete() {
    glDeleteBuffers(GL_ARRAY_BUFFER, &aID);
    Buffer::Delete();
}

void VBO::BufferData(GLsizeiptr pByteSize, GLfloat * pPtrElement, GLuint pDrawType) {
    Bind();
    glBufferData(GL_ARRAY_BUFFER, pByteSize, pPtrElement, pDrawType);
    Unbind();
}

void VBO::SubBufferData(GLsizeiptr pByteSize, GLfloat * pPtrElement) {
    Bind();
    glBufferSubData(GL_ARRAY_BUFFER, 0, pByteSize, pPtrElement);
    Unbind();
}