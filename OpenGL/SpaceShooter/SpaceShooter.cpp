#include <iostream>
#include <GL/glut.h>
#include <cmath>
#include <ctime>
#include <vector>
#include <string>

// Screen dimensions
int screenWidth = 800;  // Width
int screenHeight = 600; // Height

// Player position
float playerY = screenHeight / 2;

// Enemy position and speed
float enemyX = screenWidth - 50;
float enemyY = screenHeight / 2;
float enemySpeed = 3.0f;

// Structure and vector for stars
struct Star {
    float x;
    float y;
    float speed;
};

std::vector<Star> stars;

// Game states
bool gameOver = false;
bool welcomeScreen = true;
bool showGameOverText = false;
bool gameOverBlink = false;
bool blinkState = false;
bool showRestartMessage = false;

// Score
int score = 0;

// Positions of bullets fired by the player
struct Bullet {
    float x;
    float y;
    bool active;
    time_t lastFireTime; // Last firing time
};

Bullet bullets[100]; // Maximum 100 bullets
time_t lastFireTime = 0; // Last firing time

// Input processing function
void processInput(unsigned char key, int x, int y) {
    if (welcomeScreen) {
        if (key == ' ') {
            welcomeScreen = false; // Close the welcome screen
        }
    }
    else {
        if (!gameOver) {
            switch (key) {
            case 'w':
                if (playerY < screenHeight - 50) playerY += 10.0f; // Move up
                break;
            case 's':
                if (playerY > 0) playerY -= 10.0f; // Move down
                break;
            case ' ':
                // Control firing of bullets based on timer
                if (time(nullptr) - lastFireTime >= 1) {
                    for (int i = 0; i < 100; i++) {
                        if (!bullets[i].active) {
                            bullets[i].active = true;
                            bullets[i].x = 70;
                            bullets[i].y = playerY + 25;
                            bullets[i].lastFireTime = time(nullptr);
                            lastFireTime = time(nullptr);
                            break;
                        }
                    }
                }
                break;
            }
        }
        // Restart the game by pressing "y" when game over
        else if (key == 'y') {
            // Reset game state
            gameOver = false;
            showGameOverText = false;
            showRestartMessage = false;
            score = 0;
            enemyX = screenWidth - 50;
            enemyY = screenHeight / 2;
            // Reset bullets
            for (int i = 0; i < 100; ++i) {
                bullets[i].active = false;
            }
        }
        else {
            exit(0); // Exit with any key except "y"
        }
    }
}

// Function to add stars
void addStars(int numStars) {
    for (int i = 0; i < numStars; ++i) {
        Star star;
        star.x = rand() % screenWidth;
        star.y = rand() % screenHeight;
        star.speed = (rand() % 10) / 10.0f + 0.1f; // Random speed setting
        stars.push_back(star);
    }
}

// Function to update stars
void updateStars() {
    for (auto& star : stars) {
        star.y -= star.speed; // Move stars downwards
        if (star.y < 0) {
            // Reset position if out of screen
            star.x = rand() % screenWidth;
            star.y = screenHeight;
            star.speed = (rand() % 10) / 10.0f + 0.1f; // Random speed setting
        }
    }
}

// Function to update game state
void update(int value) {
    if (!gameOver && !welcomeScreen) {
        // Enemy movement
        enemyX -= enemySpeed;

        // Check if enemy has crossed the boundary
        if (enemyX < 0) {
            enemyX = screenWidth - 50; // Reset enemy position
            enemyY = rand() % (screenHeight - 50); // Set Y position randomly
        }

        // Bullet movement and hitting the enemy
        for (int i = 0; i < 100; i++) {
            if (bullets[i].active) {
                bullets[i].x += 5.0f;
                if (bullets[i].x > screenWidth) {
                    bullets[i].active = false; // Disable bullet when out of screen
                }
                else {
                    // Check for bullet-enemy collision
                    if (bullets[i].x > enemyX && bullets[i].x < enemyX + 50 &&
                        bullets[i].y > enemyY && bullets[i].y < enemyY + 50) {
                        enemyX = screenWidth - 50; // Reset enemy position
                        enemyY = rand() % (screenHeight - 50); // Set Y position randomly
                        bullets[i].active = false; // Disable bullet
                        score++; // Increment score
                    }
                }
            }
        }

        // Check for enemy-player collision
        if (enemyX < 52 && enemyX + 50 > 50 && enemyY < playerY + 50 && enemyY + 50 > playerY) {
            gameOver = true; // End the game
            showGameOverText = true; // Show Game Over text
            showRestartMessage = true; // Show message for playing again
        }

        // Check if enemy has reached the left edge
        if (enemyX <= 0) {
            gameOver = true; // End the game
            glClear(GL_COLOR_BUFFER_BIT);
            showGameOverText = true; // Show Game Over text
            showRestartMessage = true; // Show message for playing again
        }

        // Update stars
        updateStars();
    }

    if (gameOver) {
        // Control blinking of Game Over text
        if (gameOverBlink) {
            blinkState = !blinkState;
            if (blinkState) {
                showGameOverText = true;
            }
            else {
                showGameOverText = false;
            }
            glutPostRedisplay();
            glutTimerFunc(750, update, 0); // Call again after 750ms
            return;
        }
    }

    glutPostRedisplay(); // Redraw the screen
    glutTimerFunc(16, update, 0); // Call again after 16ms
}

// Function to render the screen
void render() {
    glClear(GL_COLOR_BUFFER_BIT);

    if (welcomeScreen) {
        glColor3f(1.0, 0.0, 0.0); // Red color
        glRasterPos2i(screenWidth / 2 - 200, screenHeight / 2 + 50);
        std::string welcomeText = "Welcome to Space Shooter!";
        for (int i = 0; i < welcomeText.size(); i++) {
            glutBitmapCharacter(GLUT_BITMAP_9_BY_15, welcomeText[i]);
        }
        glRasterPos2i(screenWidth / 2 - 200, screenHeight / 2);
        std::string instructionText = "Press W to move up, S to move down.";
        for (int i = 0; i < instructionText.size(); i++) {
            glutBitmapCharacter(GLUT_BITMAP_9_BY_15, instructionText[i]);
        }
        glRasterPos2i(screenWidth / 2 - 200, screenHeight / 2 - 50);
        std::string startText = "Press Space to start.";
        for (int i = 0; i < startText.size(); i++) {
            glutBitmapCharacter(GLUT_BITMAP_9_BY_15, startText[i]);
        }
    }
    else {
        // Draw the background
        glColor3f(0.0, 0.0, 0.0); // Black color
        glBegin(GL_QUADS);
        glVertex2f(0, 0);
        glVertex2f(screenWidth, 0);
        glVertex2f(screenWidth, screenHeight);
        glVertex2f(0, screenHeight);
        glEnd();

        // Draw stars
        glColor3f(1.0, 1.0, 0.0); // Yellow color
        glPointSize(2.0f); // Point size
        glBegin(GL_POINTS);
        for (const auto& star : stars) {
            glVertex2f(star.x, star.y);
        }
        glEnd();

        // Player
        glColor3f(1.0, 1.0, 1.0); // White color
        glBegin(GL_TRIANGLES);
        glVertex2f(20, playerY);
        glVertex2f(20, playerY + 50);
        glVertex2f(50, playerY + 25);
        glEnd();

        // Enemy
        glColor3f(0.0, 0.0, 1.0); // Blue color
        glBegin(GL_TRIANGLES);

        // Front part
        glVertex2f(enemyX + 25, enemyY + 50);
        glVertex2f(enemyX, enemyY + 25);
        glVertex2f(enemyX + 50, enemyY + 25);

        // Behind part
        glVertex2f(enemyX + 25, enemyY);
        glVertex2f(enemyX, enemyY + 25);
        glVertex2f(enemyX + 50, enemyY + 25);

        // Up part
        glVertex2f(enemyX + 25, enemyY + 50);
        glVertex2f(enemyX, enemyY + 25);
        glVertex2f(enemyX + 50, enemyY + 25);

        // Down part
        glVertex2f(enemyX + 25, enemyY);
        glVertex2f(enemyX, enemyY + 25);
        glVertex2f(enemyX + 50, enemyY + 25);
        glEnd();

        // Bullets
        glColor3f(1.0, 0.0, 0.0); // Red color
        glPointSize(5.0f); // Point size
        glBegin(GL_POINTS);
        for (int i = 0; i < 100; i++) {
            if (bullets[i].active) {
                glVertex2f(bullets[i].x, bullets[i].y);
            }
        }
        glEnd();

        // Scor
        glColor3f(0.0, 1.0, 1.0); // White color
        glRasterPos2i(10, 20);
        std::string scoreText = "Score: " + std::to_string(score);
        for (int i = 0; i < scoreText.size(); i++) {
            glutBitmapCharacter(GLUT_BITMAP_9_BY_15, scoreText[i]);
        }

        // Draw Game Over text on screen
        if (showGameOverText) {
            
            glClear(GL_COLOR_BUFFER_BIT);

            glColor3f(1.0, 0.0, 0.0); // Red color
            glRasterPos2i(screenWidth / 2 - 50, screenHeight / 2);
            std::string text = "Game Over!";
            for (int i = 0; i < text.size(); i++) {
                glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, text[i]);
            }
            // Settings for flashing Game Over text
            gameOverBlink = true;
            blinkState = true;

            if (showRestartMessage) {
                glRasterPos2i(screenWidth / 2 - 100, screenHeight / 2 - 50);
                std::string restartText = "Press 'y' to play again.";
                for (int i = 0; i < restartText.size(); i++) {
                    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, restartText[i]);
                }
            }
        }
    }

    glutSwapBuffers();
}

// Function called when window size changes
void reshape(int w, int h) {
    screenWidth = w;
    screenHeight = h;
    glViewport(0, 0, screenWidth, screenHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, screenWidth, 0.0, screenHeight, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// Main function
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(screenWidth, screenHeight);
    glutCreateWindow("Space Shooter");

    // Stars are added
    addStars(100);

    glutDisplayFunc(render);
    glutReshapeFunc(reshape);
    glutTimerFunc(0, update, 0);
    glutKeyboardFunc(processInput);

    glClearColor(0.0, 0.0, 0.0, 1.0); // Background color black
    glutMainLoop();
    return 0;
}
