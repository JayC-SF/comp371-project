class Scoreboard
{
public:
    const int MAXSCORE = 3;
    int player1Score = 0, player2Score = 0;
    glm::vec3 player1Color = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 player2Color = glm::vec3(1.0f, 0.0f, 1.0f);
    glm::vec3 scoreboardColor = glm::vec3(0.0f, 0.0f, 0.0f);

    // Draw the scoreboard at a certain position and angle
    void drawScoreboard(int cubeVAO, int shaderProgram)
    {
        // Make it a square
        // Player 1 score on the right and player 2 score on the left

        // Set uniforms for the shader
        glUniform1i(glGetUniformLocation(shaderProgram, "useTexture"), 0); // no texture being used
        glUniform3fv(glGetUniformLocation(shaderProgram, "myColor"), 1, &scoreboardColor[0]);

        // part matrix for the scoreboard
        glm::mat4 partMatrix = glm::mat4(1.0f);
        partMatrix = glm::translate(partMatrix, glm::vec3(0.0f, 2.5f, 0.0f));
        partMatrix = glm::scale(partMatrix, glm::vec3(0.5f, 5.0f, 10.0f));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "modelMatrix"), 1, GL_FALSE, &partMatrix[0][0]);

        // Draw scoreboard
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Turn textures back on after scoreboard is rendered
        glUniform1i(glGetUniformLocation(shaderProgram, "useTexture"), 1);
    }
};