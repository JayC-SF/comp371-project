#ifndef ATOM_H
#define ATOM_H
#include "../../buffer/include.h"
// this class serves as a basis for atom objects, like cube, sphere, etc.
// This objects can then be modified using matrices.
class Atom {
    public:
    Atom();
    ~Atom();
    virtual void Draw();
    virtual void BindAttributes();
    virtual void UnbindAttributes();
    protected:
    VAO aVAO;
    VBO aVBOVertex;
    VBO aVBONormals;
    VBO aVBOUV;
    EBO aEBO;
    EBO * aCurrentEBO;
};
#endif