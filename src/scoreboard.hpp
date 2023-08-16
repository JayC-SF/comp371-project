#ifndef SCOREBOARD_HPP
#define SCOREBOARD_HPP
class Scoreboard
{
public:
    const int MAXSCORE = 40;
    int player1Score = 0, player2Score = 0;
    int minutes = 0, seconds = 0;
    glm::vec3 player1Color = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 player2Color = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 scoreboardColor = glm::vec3(30 / 255.0f, 86 / 255.0f, 90 / 255.0f);
    glm::vec3 timeColor = glm::vec3(235 / 255.0f, 192 / 255.0f, 52 / 255.0f);

    // Draw the scoreboard at a certain position and angle
    void drawScoreboard(int cubeVAO, int shaderProgram)
    {
        // Set uniforms for the shader
        glUniform3fv(glGetUniformLocation(shaderProgram, "myColor"), 1, &scoreboardColor[0]);

        // part matrix for the scoreboard
        glm::mat4 partMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.25f, 0.0f));
        partMatrix = glm::scale(partMatrix, glm::vec3(0.25f, 8.0f, 15.0f));

        // model matrix for scoreboard and numbers
        glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 3.0f, 21.0f));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(10.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
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

        // Draw player 2 score
        // Ones
        glUniform3fv(glGetUniformLocation(shaderProgram, "myColor"), 1, &player1Color[0]); // set color
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 2.5f));
        drawNumber(shaderProgram, player2Score  % 10, modelMatrix * transform);

        // Tens
        transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 6.0f));
        drawNumber(shaderProgram,  player2Score / 10, modelMatrix * transform);

        // Draw player 2 score
        // Ones
        glUniform3fv(glGetUniformLocation(shaderProgram, "myColor"), 1, &player2Color[0]); // set color
        transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -6.0f));
        drawNumber(shaderProgram, player1Score % 10, modelMatrix * transform);

        // Tens
        transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.5f));
        drawNumber(shaderProgram, player1Score / 10, modelMatrix * transform);

        // render time above scoreboard
        glUniform3fv(glGetUniformLocation(shaderProgram, "myColor"), 1, &timeColor[0]); // set color
        transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 4.0f, 0.0f));
        transform = glm::scale(transform, glm::vec3(1.0f, 0.4f, 0.4f));
        incrementTime(); // increment time before drawing it
        drawTime(shaderProgram, modelMatrix * transform);
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

    void drawTime(int shaderProgram, glm::mat4 modelMatrix)
    {
        // Minutes
        // Ones
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 2.5f));
        drawNumber(shaderProgram, minutes % 10, modelMatrix * transform);

        // Tens
        transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 6.0f));
        drawNumber(shaderProgram, minutes / 10, modelMatrix * transform);

        // Seconds
        // Ones
        transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -6.0f));
        drawNumber(shaderProgram, seconds % 10, modelMatrix * transform);

        // Tens
        transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.5f));
        drawNumber(shaderProgram, seconds / 10, modelMatrix * transform);

        // colon
        transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        transform = glm::scale(transform, glm::vec3(0.4f, 0.4f, 0.4f));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "modelMatrix"), 1, GL_FALSE, &(modelMatrix * transform)[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
        transform = glm::scale(transform, glm::vec3(0.4f, 0.4f, 0.4f));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "modelMatrix"), 1, GL_FALSE, &(modelMatrix * transform)[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36);
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

    void incrementTime()
    {
        int timeFromStart = (int)glfwGetTime();
        seconds = timeFromStart % 60;
        minutes = timeFromStart / 60;
    }

    // Resets all vars of the scoreboard
    // Time is set to 0 as well
    void reset()
    {
        player1Score = 0;
        player2Score = 0;
        seconds = 0;
        minutes = 0;
        glfwSetTime(0.0f);
    }
};
#endif