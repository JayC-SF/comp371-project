#include "Plane.h"
#include <iostream>

Plane::Plane(GLfloat pWidth, GLfloat pHeight, vec3 pNormal, vec3 pUpTiltVector, vec3 pCenterPosition, const char * pPlaneName) {
    aWidth = pWidth;
    aHeight = pHeight;

    aCenterPosition  = pCenterPosition;
    aUpTiltVector = pUpTiltVector;
    aVelocity = vec3(0.f);
    // compute three orthogonal vectors
    aNormal = normalize(pNormal);
    aRight = normalize(cross(aNormal, aUpTiltVector));
    // no need to normalize aUp since aNormal and aRight are unit vectors
    aUp = cross(aRight, aNormal);
    aPlaneName = pPlaneName;
}

vec3 Plane::GetNormal() {
    return aNormal;
}
vec3 Plane::GetUpVector() {
    return aUp;
}
vec3 Plane::GetRightVector() {
    return aRight;
}
vec3 Plane::GetUpTiltVector() {
    return aUpTiltVector;
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

vec3 Plane::GetVelocity() {
    return aVelocity;
}

void Plane::SetPosition(vec3 pPosition) {
    aCenterPosition = pPosition;
}
void Plane::SetNormal(vec3 pNormal) {
    aNormal = pNormal;    
}

void Plane::SetUpTiltVector(vec3 pUpTiltVector) {
    aUpTiltVector = pUpTiltVector;
}

void Plane::UpdatePhysics(vec3 pPosition, vec3 pNormal, vec3 pUpTiltVector, GLfloat dt) {
    aVelocity = (pPosition - aCenterPosition)/dt;
    aCenterPosition = pPosition;
    aUpTiltVector = pUpTiltVector;
   
    // compute TNB frame
    aNormal = normalize(pNormal);
    aRight = normalize(cross(aNormal, aUpTiltVector));
    // no need to normalize aUp since aNormal and aRight are unit vectors
    aUp = cross(aRight, aNormal);
}

const char * Plane::GetPlaneName() {
    return aPlaneName;
}

void Plane::PlayCollisionSound() {
    for (int i = 0; i < aSoundSources.size(); i++) {
        getSoundEngine()->play2D(aSoundSources[i], false);
    }
}

void Plane::AddSound(ISoundSource* soundSource) {
    aSoundSources.push_back(soundSource);
}