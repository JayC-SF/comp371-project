#ifndef PLANE_H
#define PLANE_H
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

using namespace glm;
using  namespace std;

class Plane{
    public:
    Plane(GLfloat pWidth, GLfloat pHeight, vec3 pNormal, vec3 pPosition);

    vec3 GetNormal();
    GLfloat GetWidth();
    GLfloat GetHeight();
    vec3 GetCenterPosition();
    vec3 GetEdgeDirection();
    vec3 GetVelocity();
    
    void SetPosition(vec3 pPosition);
    void SetNormal(vec3 pNormal);
    void UpdatePhysics(vec3 pPosition, vec3 pNormal, GLfloat dt);

    // void GetCornerPoints(vector<vec3> & points);
    // void Rotate(GLfloat pRads, vec3 pDirection);

    protected:
    GLfloat aWidth, aHeight;
    vec3 aNormal;
    
    // vector pointing to the top edge of the plane.
    // this vector will be used to always point towards the same edge
    vec3 aEdgeDirection;
    vec3 aCenterPosition;
    vec3 aVelocity;
    
};
#endif