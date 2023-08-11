#ifndef SPHERE_H
#define SPHERE_H
#include <glm/glm.hpp>
#include "atom.h"
#include <vector>
class Sphere : public Atom{
    public:
    static Sphere& GetInstance();
    ~Sphere();
    Sphere();
    protected:
    void computeVertices(GLuint pSubDivisions, std::vector<glm::vec3> & pVertices);
    void computeIndices(GLuint pSubDivisions, GLuint pVerticesSize, std::vector<GLuint> & pIndices);
    void computeUV(GLuint pVerticesSize, std::vector<glm::vec2> & UV);
};
#endif
