#ifndef CUBE_H
#define CUBE_H
#include <glm/glm.hpp>
#include "atom.h"
#include <vector>
#define CUBE_FRONT_FACE 0
#define CUBE_BACK_FACE 1
#define CUBE_RIGHT_FACE 2
#define CUBE_LEFT_FACE 3
#define CUBE_TOP_FACE 4
#define CUBE_BOTTOM_FACE 5
#define CUBE_Y_DIR 1
#define CUBE_X_DIR 2
#define CUBE_XY_DIR 3

class Cube : public Atom{
    public:
    static Cube * GetInstance();
    void SetRepeatUVCoord(GLfloat pUVMaxCoords, GLuint face, GLuint direction);
    void SetRepeatUVCoord(glm::vec3 pDims);
    void SetLineDrawingMode();
    void SetTriangleDrawingMode();
    void SetPointDrawingMode();
    void SetInvertDrawingMode();
    void ResetUVCoords();
    void DrawFace(GLuint face);
    ~Cube();
    Cube();

    protected:
    EBO aEBOLines;
    EBO aEBOPoints;
    EBO aEBOInvert;

};
#endif