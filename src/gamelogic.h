#ifndef GAMELOGIC_H
#define GAMELOGIC_H
#include <GL/glew.h>
#include "ObserverPattern.h"
#include "Plane.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include "objects/include.h"
#include "objects/tennisball.h"
#include "scoreboard.hpp"

using namespace glm;

class GameLogic : public Observer{
    public:
    GameLogic(Plane * pPlayer1Plane, Plane * pPlayer2Plane, Plane * pRacket1Plane, Plane * pRacket2Plane, Plane * pTennisNet, Scoreboard * pScoreBoard, TennisBall * pTennisBall);
    void Update(Subject * pSubject) override;
    void SetBallToServingPosition(Plane * pPlane, Plane * pRacketPlane);
    void ServeBall();
    void UpdateBallPositionIfServing();
    protected:

    GLuint aPlayer1Score, aPlayer2Score;
    Plane * aPlayer1Plane, * aPlayer2Plane, * aRacket1Plane, * aRacket2Plane, * aNetPlane;
    Scoreboard * aScoreBoard;
    TennisBall * aTennisBall;
    // variable checking if the ball is being held in place until the player hits it off
    bool isBallToBeServed;
    bool aIsLastHitPlayer1;
};
#endif