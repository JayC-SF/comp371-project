#include "EBO.h"
EBO::EBO() {}

void EBO::Init(GLuint pIndicesSize, GLuint * pIndices,GLenum pDrawType, GLenum pDrawMode) {
    glGenBuffers(1, &aID);
    Bind();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, pIndicesSize*sizeof(GLuint), pIndices, pDrawType);
    Unbind();
    aLength = pIndicesSize;
    aDrawMode = pDrawMode;
}

EBO::EBO(GLuint pIndicesSize, GLuint * pIndices,GLuint pDrawType, GLuint pDrawMode){
    Init(pIndicesSize, pIndices, pDrawType, pDrawMode);
}

// Need to bind the EBO first
void EBO::Draw() {
    Bind();
    glDrawElements(aDrawMode, aLength, GL_UNSIGNED_INT, 0);
    Unbind();
}
void EBO::Bind() {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, aID);
}

void EBO::Unbind() {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void EBO::Delete() {
    glDeleteBuffers(1, &aID);
    Buffer::Delete();
    aDrawMode = 0, aLength = 0;
}

GLuint EBO::GetLength() {
    return aLength;
}
GLuint EBO::GetDrawMode() {
    return aDrawMode;
}