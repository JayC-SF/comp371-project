#include "tennisball.h"
#include "atoms/sphere.h"
#include "../texture.h"
#include "glm/common.hpp"
#include "glm/geometric.hpp"
#include <cstdio>
#include <iostream>

TennisBall::TennisBall() :
aModelMatrix(mat4(1.f)), 
aScale(mat4(1.f)), 
aRotation(mat4(1.f)), 
aRadius(1.f), 
aIsUpdated(false),
aMaxSpeed(1.f)
{}

TennisBall::TennisBall(GLfloat pRadius):
aModelMatrix(mat4(1.f)), 
aScale(scale(mat4(1.f), vec3(pRadius))), 
aRotation(mat4(1.f)), 
aRadius(pRadius), 
aIsUpdated(false), 
aMaxSpeed(1.f)
{}

TennisBall::TennisBall(GLfloat pRadius, vec3 initPosition, vec3 initVelocity, vec3 initAcceleration, GLfloat pMaxSpeed):
aModelMatrix(mat4(1.f)), 
aScale(scale(mat4(1.f), vec3(pRadius))), 
aRotation(mat4(1.f)), 
aRadius(pRadius), 
aIsUpdated(false){
    SetAcceleration(initAcceleration);
    SetPosition(initPosition);
    SetVelocity(initVelocity);
    aMaxSpeed = pMaxSpeed;
}

void TennisBall::Draw(GLuint pShaderProgramId, GLuint pModelMatrixLocation) {
    Update();
    
    // Update Sphere object after update()
    glUseProgram(pShaderProgramId);
    Sphere & sphere = Sphere::GetInstance();    
    sphere.BindAttributes();
    glUniformMatrix4fv(pModelMatrixLocation, 1, GL_FALSE, &aModelMatrix[0][0]);

    // Set Texture 
    Texture * tennisTexture = Texture::GetTennisBallTexture();
    tennisTexture->UseTexture();

    // Draw Sphere
    sphere.Draw();
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

    vec3 currentHorizontalVelocity = vec3(aCurrentVelocity.x, 0.0f, aCurrentVelocity.z);
    GLfloat currentHorizontalSpeed = length(currentHorizontalVelocity);

    // cap currentHorizontalSpeed to max velocity allowed
    if (aMaxSpeed < currentHorizontalSpeed) {
        
       // transforms velocity into unit vector and assign it the speed of max velocity
       currentHorizontalVelocity = (currentHorizontalVelocity/currentHorizontalSpeed)*aMaxSpeed;
       aCurrentVelocity.x = currentHorizontalVelocity.x;
       aCurrentVelocity.z = currentHorizontalVelocity.z;
    }

    // calculate new velocity and collisions
    CheckCollisions();
    aIsUpdated = false;
}

void TennisBall::AddCollidingPlane(Plane * pPlane) {
    aCollidingPlanes.push_back(pPlane);
    aCollidingStates.push_back(false);
}
void printVector(vec3 v) {
    cout << "(" << v.x << ", " << v.y << ", " << v.z << ")" << endl;

}

void TennisBall::CheckCollisions() {
    for (int i = 0; i<aCollidingPlanes.size(); i++) {
        // check if we have a collision
        vec3 planePoint = aCollidingPlanes[i]->GetCenterPosition();
        vec3 planeNormal = aCollidingPlanes[i]->GetNormal();
        vec3 planeToSphereCenter = aCurrentPosition - planePoint;
        // since normal is already a unit vector, no need to divide by magnitude
        GLfloat projMag = dot(planeToSphereCenter, planeNormal);
        
        bool currentCollidingState = abs(projMag) < aRadius;
        // check if projection magnitude is less than the radius length
        // if so check if point is inside the plane.
        if (currentCollidingState) {
            vec3 pointOfIncidence = -projMag * planeNormal + aCurrentPosition;
            vec3 incidenceVector = pointOfIncidence - planePoint;

            GLfloat absHorizontalProjMagnitude = abs((dot(incidenceVector, aCollidingPlanes[i]->GetRightVector())));
            GLfloat absVerticalProjMagnitude = abs(dot(incidenceVector, aCollidingPlanes[i]->GetUpVector()));
            // check if horizontal projection is smaller than half of the width of the plane
            // and check if vertical projection is smaller than half of height of the plane
            currentCollidingState = 
                (absHorizontalProjMagnitude <= aCollidingPlanes[i]->GetWidth()/2.0f + aRadius) &&
                (absVerticalProjMagnitude <= aCollidingPlanes[i]->GetHeight()/2.0f + aRadius);
            // check if the sphere has not collided already
            // and check if the point of collision is within the plane
            if (!aCollidingStates[i] && currentCollidingState) {
                vec3 planeVelocity = aCollidingPlanes[i]->GetVelocity();
                
                // cout << "ball position:" << endl;
                // printVector(aCurrentPosition);
                // cout << aCollidingPlanes[i]->GetPlaneName() << ": collision " << endl;
                // printVector(pointOfIncidence);
                // cout << aCollidingPlanes[i]->GetPlaneName() << ": pointCenter " << endl;
                // printVector(planePoint);
                // cout << aCollidingPlanes[i]->GetPlaneName() << ": normal " << endl;
                // printVector(planeNormal);
                // cout << aCollidingPlanes[i]->GetPlaneName() << ": right " << endl;
                // printVector(aCollidingPlanes[i]->GetRightVector());
                // cout << aCollidingPlanes[i]->GetPlaneName() << ": up " << endl;
                // printVector(aCollidingPlanes[i]->GetUpVector());
                // cout << aCollidingPlanes[i]->GetPlaneName() << ": uptilt " << endl;
                // printVector(aCollidingPlanes[i]->GetUpTiltVector());
                
                // cout << "currentVelocity before" << aCurrentVelocity.x << " " << aCurrentVelocity.y << " " << aCurrentVelocity.z << endl;
                aCurrentVelocity = reflect(aCurrentVelocity, planeNormal) + planeVelocity;
                // cout << "currentVelocity after " << aCurrentVelocity.x << " " << aCurrentVelocity.y << " " << aCurrentVelocity.z << endl;

            }
        }
        aCollidingStates[i] = currentCollidingState;
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