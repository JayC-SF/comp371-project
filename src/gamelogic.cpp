#include "gamelogic.h"
#include "Plane.h"
#include <iostream>

GameLogic::GameLogic(Plane * pPlayer1Plane, Plane * pPlayer2Plane, Scoreboard * pScoreBoard, TennisBall * pTennisBall) {
    aPlayer1Plane = pPlayer1Plane; 
    aPlayer2Plane = pPlayer2Plane;
    aPlayer1Score = 0;
    aPlayer2Score = 0;
    aScoreBoard = pScoreBoard;
    aTennisBall = pTennisBall;
}

void GameLogic::Update(Subject * pSubject) {
    Plane * planeSubject = (Plane *) pSubject;
    // check if player 2 scored on player 1's plane
    if (planeSubject->GetCenterPosition() == aPlayer1Plane->GetCenterPosition()) {
        aScoreBoard->incrementPlayer2Score();
        isBallOnHold = true;
        SetBallToServingPosition(planeSubject);
    }
    // check if player 1 scored on player 2's plane
    else if (planeSubject->GetCenterPosition() == aPlayer2Plane->GetCenterPosition()) {
        aScoreBoard->incrementPlayer1Score();
        isBallOnHold = true;
        SetBallToServingPosition(planeSubject);

    }
}

void GameLogic::SetBallToServingPosition (Plane * pPlane) {
    vec3 ballPosition = pPlane->GetCenterPosition();
    // add 0.1 for the bias so that the tennis ball does not collide with the plane
    GLfloat offset = aTennisBall->GetRadius()/2.f + 0.1;
    
    // set the offset so the ball does not collide.
    if (ballPosition.x > 0)
        offset = -offset;
    
    // add the offset on the x axis.
    ballPosition.x += offset;
    ballPosition.y = 8.f;

    aTennisBall->SetPosition(ballPosition);
    aTennisBall->SetVelocity(vec3(0.f));
    aTennisBall->SetAcceleration(vec3(0.f));
}
