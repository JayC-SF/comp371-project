#ifndef GAMELOGIC_H
#define GAMELOGIC_H
#include <GL/glew.h>
#include "ObserverPattern.h"
#include "Plane.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include "scoreboard.hpp"

using namespace glm;

class GameLogic : public Observer{
    public:
    GameLogic(Plane * pPlayer1Plane, Plane * pPlayer2Plane, Scoreboard * pScoreBoard);
    void Update(Subject * pSubject) override;
    protected:

    GLuint aPlayer1Score, aPlayer2Score;
    Plane * aPlayer1Plane, * aPlayer2Plane;
    Scoreboard * aScoreBoard;
};
#endif