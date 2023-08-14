#ifndef EBO_H
#define EBO_H
#include "buffer.h"
class EBO: public Buffer{
    public:
    EBO();
    EBO(GLuint pIndicesSize, GLuint * pIndices,GLenum pDrawType, GLenum pDrawMode);
    void Init(GLuint pIndicesSize, GLuint * pIndices, GLenum pDrawType, GLenum pDrawMode);
    void Bind() override;
    void Unbind() override;
    void Delete() override;
    void Draw();
    GLuint GetLength();
    GLuint GetDrawMode();
    protected:
    GLuint aLength;
    GLenum aDrawMode;
};
#endif