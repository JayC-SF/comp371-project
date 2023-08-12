#include "Plane.h"
#include <iostream>
Plane::Plane(GLfloat pWidth, GLfloat pHeight, vec3 pNormal, vec3 pCenterPosition) {
    aWidth = pWidth;
    aHeight = pHeight;
    aWidth = pWidth;
    aNormal = normalize(pNormal);
    aCenterPosition  = pCenterPosition;
    aVelocity = vec3(0.f);
}

vec3 Plane::GetNormal() {
    return aNormal;
}
GLfloat Plane::GetWidth() {
    return aWidth;
}
GLfloat Plane::GetHeight() {
    return aHeight;
}
vec3 Plane::GetCenterPosition() {
    return aCenterPosition;
}
vec3 Plane::GetEdgeDirection() {
    return aEdgeDirection;
}
vec3 Plane::GetVelocity() {
    return aVelocity;
}

void Plane::SetPosition(vec3 pPosition) {
    aCenterPosition = pPosition;
}
void Plane::SetNormal(vec3 pNormal) {
    aNormal = pNormal;    
}

void Plane::UpdatePhysics(vec3 pPosition, vec3 pNormal, GLfloat dt) {
    aVelocity = (pPosition - aCenterPosition)/dt;
    // cout << aVelocity.x << " " << aVelocity.y << " " << aVelocity.z << endl;
    aCenterPosition = pPosition;
    aNormal = pNormal;
}