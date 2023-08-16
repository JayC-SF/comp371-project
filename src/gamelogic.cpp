#include "gamelogic.h"
#include "Plane.h"
#include <iostream>
#include <string.h>

GameLogic::GameLogic(Plane * pPlayer1Plane, Plane * pPlayer2Plane, Plane * pRacket1Plane, Plane * pRacket2Plane, Plane * pNetPlane, Scoreboard * pScoreBoard, TennisBall * pTennisBall) {
    aPlayer1Plane = pPlayer1Plane; 
    aPlayer2Plane = pPlayer2Plane;
    aPlayer1Score = 0;
    aPlayer2Score = 0;
    aScoreBoard = pScoreBoard;
    aTennisBall = pTennisBall;
    aRacket1Plane = pRacket1Plane;
    aRacket2Plane = pRacket2Plane;
    aNetPlane = pNetPlane;
}

void GameLogic::Update(Subject * pSubject) {
    Plane * planeSubject = (Plane *) pSubject;
    if (isBallToBeServed == true) {
        aTennisBall->SetAcceleration(vec3(0.f, -25.f, 0.f));
        isBallToBeServed = false;
    }
    // check if player 2 scored on player 1's plane
    if (strcmp(planeSubject->GetPlaneName(), aPlayer1Plane->GetPlaneName()) == 0) {
        aScoreBoard->incrementPlayer2Score();
        SetBallToServingPosition(planeSubject, aRacket1Plane);
    }
    // check if player 1 scored on player 2's plane
    else if (strcmp(planeSubject->GetPlaneName(), aPlayer2Plane->GetPlaneName()) == 0) {
        aScoreBoard->incrementPlayer1Score();
        SetBallToServingPosition(planeSubject, aRacket2Plane);
    }
    else if (strcmp(planeSubject->GetPlaneName(), aRacket1Plane->GetPlaneName()) == 0) {
        aIsLastHitPlayer1 = true;
    }
    else if (strcmp(planeSubject->GetPlaneName(), aRacket2Plane->GetPlaneName()) == 0) {
        aIsLastHitPlayer1 = false;
    }
    else if (strcmp(planeSubject->GetPlaneName(), aNetPlane->GetPlaneName()) == 0) {
        // perform the score
        if (aIsLastHitPlayer1) {
            aScoreBoard->incrementPlayer2Score();
            SetBallToServingPosition(planeSubject, aRacket1Plane);
        }
        else {
            aScoreBoard->incrementPlayer1Score();
            SetBallToServingPosition(planeSubject, aRacket2Plane);
        }
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
