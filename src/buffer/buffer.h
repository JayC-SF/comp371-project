#ifndef BUFFER_H
#define BUFFER_H
#include <GL/glew.h>    // Include GLEW - OpenGL Extension Wrangler
class Buffer {
    public:
    Buffer() {};
    virtual void Bind() {};
    virtual void Unbind() {};
    virtual void Delete() {aID = 0;};
    GLuint aID;
    ~Buffer() {Delete();}
};

#endif