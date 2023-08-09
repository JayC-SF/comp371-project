class scoreboard
{
public:
    const int MAXSCORE = 3;
    int player1Score = 0, player2Score = 0;
    glm::vec3 player1Color = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 player2Color = glm::vec3(1.0f, 0.0f, 1.0f);
    glm::vec3 scoreboardColor = glm::vec3(0.0f, 0.0f, 0.0f);

    // Draw the scoreboard at a certain position and angle
    void drawScoreboard(int cubeVAO, int shaderProgram, glm::mat4 modelMatrix)
    {
        glm::mat4 transform = glm::mat4(1.0f);
    }
};