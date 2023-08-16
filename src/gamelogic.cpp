#include "gamelogic.h"
#include "Plane.h"
#include <iostream>

GameLogic::GameLogic(Plane * pPlayer1Plane, Plane * pPlayer2Plane, Plane * pRacket1Plane, Plane * pRacket2Plane, Scoreboard * pScoreBoard, TennisBall * pTennisBall) {
    aPlayer1Plane = pPlayer1Plane; 
    aPlayer2Plane = pPlayer2Plane;
    aPlayer1Score = 0;
    aPlayer2Score = 0;
    aScoreBoard = pScoreBoard;
    aTennisBall = pTennisBall;
    aRacket1Plane = pRacket1Plane;
    aRacket2Plane = pRacket2Plane;
}

void GameLogic::Update(Subject * pSubject) {
    Plane * planeSubject = (Plane *) pSubject;
    // check if player 2 scored on player 1's plane
    if (planeSubject->GetCenterPosition() == aPlayer1Plane->GetCenterPosition()) {
        aScoreBoard->incrementPlayer2Score();
        SetBallToServingPosition(planeSubject, aRacket1Plane);
    }
    // check if player 1 scored on player 2's plane
    else if (planeSubject->GetCenterPosition() == aPlayer2Plane->GetCenterPosition()) {
        aScoreBoard->incrementPlayer1Score();
        SetBallToServingPosition(planeSubject, aRacket2Plane);
    }
    else if (isBallToBeServed == true) {
        aTennisBall->SetAcceleration(vec3(0.f, -25.f, 0.f));
    }
}

void GameLogic::SetBallToServingPosition (Plane * pPlane, Plane * pRacketPlane) {
    vec3 ballPosition;
    
    // set the offset so the ball does not collide.
    if (aRacket1Plane == pRacketPlane)
        ballPosition = vec3(-24.0f, 6, 10.5f);
    else 
        ballPosition = vec3(24.0f, 6, -10.5f);
        

    aTennisBall->SetPosition(ballPosition);
    aTennisBall->SetVelocity(vec3(0.f));
    aTennisBall->SetAcceleration(vec3(0.f));
    isBallToBeServed = true;
}
