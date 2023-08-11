#ifndef VAO_H
#define VAO_H
#include "buffer.h"
#include "VBO.h"
class VAO: public Buffer {
    public:
    VAO();
    void VAttribPointer(VBO & VBO,  GLuint layout, GLuint elementsPerVertex, GLuint elementType, GLsizeiptr stride, void* offset);
    void Draw();
    void Bind() override;
    void Unbind() override;
    void Delete() override;
};
#endif

