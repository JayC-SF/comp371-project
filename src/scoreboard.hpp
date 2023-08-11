class Scoreboard
{
public:
    const int MAXSCORE = 9;
    int player1Score = 0, player2Score = 0;
    glm::vec3 player1Color = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 player2Color = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 scoreboardColor = glm::vec3(0.0f, 0.0f, 0.0f);

    // Draw the scoreboard at a certain position and angle
    void drawScoreboard(int cubeVAO, int shaderProgram)
    {
        // Set uniforms for the shader
        glUniform3fv(glGetUniformLocation(shaderProgram, "myColor"), 1, &scoreboardColor[0]);

        // part matrix for the scoreboard
        glm::mat4 partMatrix = glm::mat4(1.0f);
        partMatrix = glm::scale(partMatrix, glm::vec3(0.25f, 5.0f, 10.0f));

        // model matrix for scoreboard and numbers
        glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.5f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "modelMatrix"), 1, GL_FALSE, &(modelMatrix * partMatrix)[0][0]);

        // Draw scoreboard
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // check if score is above the max allowed score
        if (player1Score > MAXSCORE || player2Score > MAXSCORE)
        {
            // TODO make this do something
            // for now it will reset the scores to 0
            player1Score = 0;
            player2Score = 0;
        }

        // Draw player 1 score
        glUniform3fv(glGetUniformLocation(shaderProgram, "myColor"), 1, &player1Color[0]); // set color
        glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 2.5f));
        drawNumber(shaderProgram, player1Score, modelMatrix * translate);

        // Draw player 2 score
        glUniform3fv(glGetUniformLocation(shaderProgram, "myColor"), 1, &player2Color[0]); // set color
        translate = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.5f));
        drawNumber(shaderProgram, player2Score, modelMatrix * translate);
    }

    // numbers drawn using 7 segments like on a digital clock
    void drawNumber(int shaderProgram, int number, glm::mat4 modelMatrix)
    {
        // Part Matrix for segments
        glm::mat4 partMatrix;

        // Based on https://en.wikipedia.org/wiki/File:7_Segment_Display_with_Labeled_Segments.svg
        // Segment A
        if (number == 0 || number == 2 || number == 3 || number == 5 || number == 6 || number == 7 || number == 8 || number == 9)
        {
            partMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.0f, 0.0f));
            partMatrix = glm::scale(partMatrix, glm::vec3(0.5f, 0.5f, 2.0f));
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "modelMatrix"), 1, GL_FALSE, &(modelMatrix * partMatrix)[0][0]);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // Segment B
        if (number == 0 || number == 1 || number == 2 || number == 3 || number == 4 || number == 7 || number == 8 || number == 9)
        {
            partMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, -1.0f));
            partMatrix = glm::scale(partMatrix, glm::vec3(0.5f, 2.0f, 0.5f));
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "modelMatrix"), 1, GL_FALSE, &(modelMatrix * partMatrix)[0][0]);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // Segment C
        if (number == 0 || number == 1 || number == 3 || number == 4 || number == 5 || number == 6 || number == 7 || number == 8 || number == 9)
        {
            partMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, -1.0f));
            partMatrix = glm::scale(partMatrix, glm::vec3(0.5f, 2.0f, 0.5f));
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "modelMatrix"), 1, GL_FALSE, &(modelMatrix * partMatrix)[0][0]);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // Segment D
        if (number == 0 || number == 2 || number == 3 || number == 5 || number == 6 || number == 8 || number == 9)
        {
            partMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -2.0f, 0.0f));
            partMatrix = glm::scale(partMatrix, glm::vec3(0.5f, 0.5f, 2.0f));
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "modelMatrix"), 1, GL_FALSE, &(modelMatrix * partMatrix)[0][0]);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // Segment E
        if (number == 0 || number == 2 || number == 6 || number == 8)
        {
            partMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 1.0f));
            partMatrix = glm::scale(partMatrix, glm::vec3(0.5f, 2.0f, 0.5f));
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "modelMatrix"), 1, GL_FALSE, &(modelMatrix * partMatrix)[0][0]);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // Segment F
        if (number == 0 || number == 4 || number == 5 || number == 6 || number == 8 || number == 9)
        {
            partMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 1.0f));
            partMatrix = glm::scale(partMatrix, glm::vec3(0.5f, 2.0f, 0.5f));
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "modelMatrix"), 1, GL_FALSE, &(modelMatrix * partMatrix)[0][0]);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // Segment G
        if (number == 2 || number == 3 || number == 4 || number == 5 || number == 6 || number == 8 || number == 9)
        {
            partMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
            partMatrix = glm::scale(partMatrix, glm::vec3(0.5f, 0.5f, 2.0f));
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "modelMatrix"), 1, GL_FALSE, &(modelMatrix * partMatrix)[0][0]);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }

    int getPlayer1Score()
    {
        return player1Score;
    }

    int getPlayer2Score()
    {
        return player2Score;
    }

    void incrementPlayer1Score()
    {
        player1Score += 1;
    }

    void incrementPlayer2Score()
    {
        player2Score += 1;
    }
};