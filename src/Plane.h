#ifndef PLANE_H
#define PLANE_H
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include "SoundEngine.h"
#include "ObserverPattern.h"

using namespace glm;
using  namespace std;

class Plane : public Subject{
    public:
    Plane(GLfloat pWidth, GLfloat pHeight, vec3 pNormal, vec3 pUpTiltVector, vec3 pPosition, const char * pPlaneName);

    vec3 GetNormal();
    vec3 GetUpVector();
    vec3 GetRightVector();
    vec3 GetUpTiltVector();
    GLfloat GetWidth();
    GLfloat GetHeight();
    vec3 GetCenterPosition();
    vec3 GetVelocity();
    void SetPosition(vec3 pPosition);
    void SetNormal(vec3 pNormal);
    void SetUpTiltVector(vec3 pUpTiltVector);
    const char * GetPlaneName();
    void PlayCollisionSound();
    void AddSound(ISoundSource* soundSource);

    void UpdatePhysics(vec3 pPosition, vec3 pNormal, vec3 pUpTiltVector, GLfloat dt);

    // void GetCornerPoints(vector<vec3> & points);
    // void Rotate(GLfloat pRads, vec3 pDirection);

    protected:
    GLfloat aWidth, aHeight;
    vec3 aNormal;
    vec3 aRight;
    vec3 aUp;

    // tilt vector used to determine the tilt of the plane and is used to compute the TNB frame
    vec3 aUpTiltVector;
    
    // vector pointing to the top edge of the plane.
    // this vector will be used to always point towards the same edge
    vec3 aCenterPosition;
    vec3 aVelocity;
    const char * aPlaneName;
    vector<ISoundSource*> aSoundSources;
};
#endif