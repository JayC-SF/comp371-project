#include "VAO.h"
#include "buffer.h"
#include <GL/glew.h>
VAO::VAO(){
      glGenVertexArrays(1, &aID);
}

void VAO::Bind(){
    glBindVertexArray(aID);
}

void VAO::VAttribPointer(VBO & VBO,  GLuint layout, GLuint elementsPerVertex, GLuint elementType, GLsizeiptr stride, void* offset) {
    VBO.Bind();
    // insert all the information to be added in the vertex attribute pointer.
    glVertexAttribPointer(layout, elementsPerVertex, elementType, GL_FALSE, stride, offset);
    glEnableVertexAttribArray(layout);
    VBO.Unbind();
}

void VAO::Unbind() {
    glBindVertexArray(0);
}

// delete the vertex array if not used anymore.
void VAO::Delete() {
    glDeleteVertexArrays(1, &aID);
    Buffer::Delete();
}

