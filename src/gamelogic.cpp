#include "gamelogic.h"
#include "Plane.h"
#include <iostream>

GameLogic::GameLogic(Plane * pPlayer1Plane, Plane * pPlayer2Plane, Scoreboard * pScoreBoard) {
    aPlayer1Plane = pPlayer1Plane; 
    aPlayer2Plane = pPlayer2Plane;
    aPlayer1Score = 0;
    aPlayer2Score = 0;
    aScoreBoard = pScoreBoard;
}

void GameLogic::Update(Subject * pSubject) {
    Plane * planeSubject = (Plane *) pSubject;
    // check if player 2 scored on player 1's plane
    if (planeSubject->GetCenterPosition() == aPlayer1Plane->GetCenterPosition()) {
        aScoreBoard->incrementPlayer2Score();
        // cout << "player 2 score "<< aScoreBoard->getPlayer2Score() << endl;
    }
    // check if player 1 scored on player 2's plane
    else if (planeSubject->GetCenterPosition() == aPlayer2Plane->GetCenterPosition()) {
        aScoreBoard->incrementPlayer1Score();
        // cout << "player 1 score "<< aScoreBoard->getPlayer1Score() << endl;
    }
}
