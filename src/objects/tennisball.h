#ifndef TENNIS_BALL_H
#define TENNIS_BALL_H
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
// #include "../shaders/shader.h"
#include "../Plane.h"

using namespace glm;
class TennisBall {
    public:
    TennisBall();
    TennisBall(GLfloat pRadius);
    TennisBall(GLfloat pRadius, vec3 initPosition, vec3 initVelocity, vec3 initAcceleration);

    void Draw(GLuint pShaderProgramId, GLuint pModelMatrixLocation);
    void Translate(vec3 pTranslate);
    void aRotate(GLfloat aRads, vec3 pAxis);
    void Update();
    void SetRadius(GLfloat pRadius);

    // physics
    void UpdatePhysics(GLfloat dt);
    void AddCollidingPlane(Plane * pPlane);
    void CheckCollisions();
    void SetVelocity(vec3 pVelocity);
    void SetPosition(vec3 pPosition);
    void SetAcceleration(vec3 pAcceleration);


    protected:
    GLboolean aIsUpdated;
    GLfloat aRadius;

    mat4 aModelMatrix;
    mat4 aScale;
    mat4 aRotation;

    vec3 aCurrentAcceleration;
    vec3 aCurrentVelocity;
    vec3 aCurrentPosition;
    vector<Plane *> aCollidingPlanes;

};
#endif