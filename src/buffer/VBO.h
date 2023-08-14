#ifndef VBO_H
#define VBO_H
#include "buffer.h"
class VBO: public Buffer{
    public:
    VBO();
    VBO(GLsizeiptr pByteSize, GLfloat * pPtrElement, GLuint pDrawType);
    void Init(GLsizeiptr pByteSize, GLfloat * pPtrElement, GLuint pDrawType);
    void Bind() override;
    void Unbind() override;
    void Delete() override;
    void SubBufferData(GLsizeiptr pByteSize, GLfloat * pPtrElement);
    void BufferData(GLsizeiptr pByteSize, GLfloat * pPtrElement, GLuint pDrawType);
};
#endif