#include "tennisball.h"
#include "atoms/sphere.h"
#include "../texture.h"
#include <cstdio>
#include <iostream>

TennisBall::TennisBall() :
aModelMatrix(mat4(1.f)), 
aScale(mat4(1.f)), 
aRotation(mat4(1.f)), 
aRadius(1.f), 
aIsUpdated(false)
{}

TennisBall::TennisBall(GLfloat pRadius):
aModelMatrix(mat4(1.f)), 
aScale(scale(mat4(1.f), vec3(pRadius))), 
aRotation(mat4(1.f)), 
aRadius(pRadius), 
aIsUpdated(false)
{}

TennisBall::TennisBall(GLfloat pRadius, vec3 initPosition, vec3 initVelocity, vec3 initAcceleration):
aModelMatrix(mat4(1.f)), 
aScale(scale(mat4(1.f), vec3(pRadius))), 
aRotation(mat4(1.f)), 
aRadius(pRadius), 
aIsUpdated(false){
    SetAcceleration(initAcceleration);
    SetPosition(initPosition);
    SetVelocity(initVelocity);
}

void TennisBall::Draw(GLuint pShaderProgramId, GLuint pModelMatrixLocation) {
    Update();
    Sphere & sphere = Sphere::GetInstance();
    Texture * tennisTexture = Texture::GetTennisBallTexture();
    // bool isTextured = shader->GetIsTextured();
    // shader->SetIsTextured(true);
    glUseProgram(pShaderProgramId);
    sphere.BindAttributes();
    tennisTexture->UseTexture();
    
    // shader->SetModelMatrix(aModelMatrix);
    glUniformMatrix4fv(pModelMatrixLocation, 1, GL_FALSE, &aModelMatrix[0][0]);

    sphere.Draw();
    // shader->SetIsTextured(isTextured);
    sphere.UnbindAttributes();
}

void TennisBall::SetRadius(GLfloat pRadius) {
    aRadius = pRadius;
    aScale = scale(mat4(1.f), vec3(aRadius));
}

void TennisBall::Translate(vec3 pTranslate) {
    aCurrentPosition += pTranslate;
    aIsUpdated = false;
}
void TennisBall::aRotate(GLfloat aRads, vec3 pAxis) {
    aRotation = rotate(mat4(1.f), aRads, pAxis) * aRotation;
    aIsUpdated = false;
}

void TennisBall::Update() {
    if(aIsUpdated)
        return;
    aModelMatrix = translate(mat4(1.f), aCurrentPosition) * aRotation * aScale;
    aIsUpdated = true;
}

void TennisBall::UpdatePhysics(GLfloat dt) {
    // calculate position
    aCurrentPosition += aCurrentVelocity*dt + aCurrentAcceleration*dt*dt/2.f ;
    aCurrentVelocity += aCurrentAcceleration*dt;
    // calculate velocity
    CheckCollisions();
    aIsUpdated = false;
}

void TennisBall::AddCollidingPlane(Plane * pPlane) {
    aCollidingPlanes.push_back(pPlane);
}

void TennisBall::CheckCollisions() {
    for (int i = 0; i<aCollidingPlanes.size(); i++) {
        // check if we have a collision
        vec3 planePoint = aCollidingPlanes[i]->GetCenterPosition();
        vec3 planeNormal = aCollidingPlanes[i]->GetNormal();
        vec3 planeToSphereCenter = aCurrentPosition - planePoint;
        // since normal is already a unit vector, no need to divide by magnitude
        GLfloat projMag = dot(planeToSphereCenter, planeNormal);
        
        if (projMag <= aRadius) {
            aCurrentVelocity = reflect(aCurrentVelocity, planeNormal);
        }
    }
}

void TennisBall::SetVelocity(vec3 pVelocity) {
    aCurrentVelocity = pVelocity;
}
void TennisBall::SetPosition(vec3 pPosition) {
    aCurrentPosition = pPosition;
}
void TennisBall::SetAcceleration(vec3 pAcceleration) {
    aCurrentAcceleration = pAcceleration;
}
