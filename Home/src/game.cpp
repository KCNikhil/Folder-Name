#include <graphics.h>
#include <conio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <Windows.h>
#include <algorithm> // Include the algorithm header for std::min

#define MAX_ASTEROIDS 8
#define PLAYER_SPEED 10
#define BULLET_SPEED 49 // Increased bullet speed
#define ASTEROID_SPEED 5
#define MAX_BULLETS 10
#define BULLET_DELAY 10 // Delay in milliseconds between each bullet

struct Asteroid {
    int x, y;
    bool active;
};

int screenWidth, screenHeight; // Global variables to store screen dimensions

void drawPlayer(int x, int y, int playerSize);
void drawBullet(int x, int y, int bulletSize);
void drawAsteroid(int x, int y, int asteroidSize);
void gameOverAnimation();
void drawScore(int score);
void winnerAnimation();

bool isLeftKeyPressed();
bool isRightKeyPressed();
bool isSpaceKeyPressed();

int main() {
    initwindow(getmaxwidth(), getmaxheight()); // Initialize graphics window to full screen

    screenWidth = getmaxx();
    screenHeight = getmaxy();

    srand(time(NULL));

    int playerSize = screenWidth / 20; // Player size based on screen width
    int playerX = screenWidth / 2 - playerSize / 2, playerY = screenHeight - playerSize - 20;

    int bullets[MAX_BULLETS][3]; // Array to store bullets [x, y, active]
    for (int i = 0; i < MAX_BULLETS; ++i) {
        bullets[i][2] = 0; // Mark all bullets as inactive
    }

    int score = 0;

    Asteroid asteroids[MAX_ASTEROIDS];
    for (int i = 0; i < MAX_ASTEROIDS; ++i) {
        int asteroidSize = screenWidth / 20; // Asteroid size based on screen width
        asteroids[i].x = rand() % (screenWidth - asteroidSize);
        asteroids[i].y = rand() % (screenHeight / 2) - screenHeight; // Starting above the screen
        asteroids[i].active = true;
    }

    while (1) {
        // Clear the screen
        cleardevice();

        // Draw the player
        drawPlayer(playerX, playerY, playerSize);

        // Draw the score
        drawScore(score);

        // Move the player
        if (isLeftKeyPressed()) {
            if (playerX > 0) {
                playerX -= PLAYER_SPEED;
            }
        }
        if (isRightKeyPressed()) {
            if (playerX < screenWidth - playerSize) {
                playerX += PLAYER_SPEED;
            }
        }
        if (isSpaceKeyPressed()) {
            // Fire bullets
            for (int i = 0; i < MAX_BULLETS; ++i) {
                if (bullets[i][2] == 0) { // Find inactive bullet slot
                    bullets[i][0] = playerX + playerSize / 2; // X position
                    bullets[i][1] = playerY - playerSize / 2; // Y position
                    bullets[i][2] = 1; // Mark bullet as active
                    break; // Fire only one bullet per key press
                }
            }
        }

        // Move and draw bullets
        for (int i = 0; i < MAX_BULLETS; ++i) {
            if (bullets[i][2] == 1) { // Active bullet
                drawBullet(bullets[i][0], bullets[i][1], 5); // Draw bullet
                bullets[i][1] -= BULLET_SPEED; // Move bullet up

                // Check if bullet goes out of bounds
                if (bullets[i][1] < 0) {
                    bullets[i][2] = 0; // Mark bullet as inactive
                }

                // Check collision with asteroids
                for (int j = 0; j < MAX_ASTEROIDS; ++j) {
                    if (asteroids[j].active && bullets[i][0] >= asteroids[j].x && bullets[i][0] <= asteroids[j].x + screenWidth / 20 &&
                        bullets[i][1] >= asteroids[j].y && bullets[i][1] <= asteroids[j].y + screenWidth / 20) {
                        asteroids[j].active = false;
                        bullets[i][2] = 0; // Mark bullet as inactive
                        score += 10; // Increase score
                    }
                }
            }
        }

        // Draw and move asteroids
        int activeAsteroids = 0; // Counter for active asteroids
        for (int i = 0; i < MAX_ASTEROIDS; ++i) {
            if (asteroids[i].active) {
                int asteroidSize = screenWidth / 20; // Asteroid size based on screen width
                drawAsteroid(asteroids[i].x, asteroids[i].y, asteroidSize);
                asteroids[i].y += ASTEROID_SPEED;
                ++activeAsteroids; // Increment active asteroid count

                // Check collision with player
                if (asteroids[i].y + asteroidSize >= playerY && asteroids[i].x <= playerX + playerSize && asteroids[i].x + asteroidSize >= playerX) {
                    gameOverAnimation();
                    closegraph();
                    return 0;
                }

                // Respawn asteroids when they reach the bottom
                if (asteroids[i].y > screenHeight) {
                    asteroids[i].x = rand() % (screenWidth - asteroidSize);
                    asteroids[i].y = rand() % (screenHeight / 2) - screenHeight; // Starting above the screen
                    asteroids[i].active = true;
                }
            }
        }

        // Check if all asteroids are destroyed
        if (activeAsteroids == 0) {
            winnerAnimation();
            closegraph();
            return 0;
        }

        delay(30); // Delay for smooth animation
    }

    closegraph();
    return 0;
}

void drawPlayer(int x, int y, int playerSize) {
    int poly[6] = {x, y, x + playerSize / 2, y - playerSize, x + playerSize, y};
    setcolor(WHITE);
    setfillstyle(SOLID_FILL, WHITE);
    fillpoly(3, poly);
}

void drawBullet(int x, int y, int bulletSize) {
    setcolor(YELLOW);
    setfillstyle(SOLID_FILL, YELLOW);
    rectangle(x, y, x + bulletSize, y + bulletSize * 2);
    floodfill(x + 1, y + 1, YELLOW);
}

void drawAsteroid(int x, int y, int asteroidSize) {
    setcolor(WHITE);
    setfillstyle(SOLID_FILL, WHITE);

    int numPoints = 12; // Number of points to approximate the asteroid shape
    int radius = asteroidSize / 2;
    int poly[24]; // Array to hold x, y pairs for points

    // Angle of rotation (in degrees) based on current time
    static int rotationAngle = 0;
    rotationAngle = (rotationAngle + 1) % 360; // Increment rotation angle (slow rotation speed)

    for (int i = 0; i < numPoints; ++i) {
        int angle = i * (360 / numPoints) + rotationAngle; // Add rotation angle to each point
        int variation = rand() % (radius / 2); // Random variation to create irregularity
        poly[2 * i] = x + radius + cos(angle * 3.14 / 180) * (radius + variation);
        poly[2 * i + 1] = y + radius + sin(angle * 3.14 / 180) * (radius + variation);
    }

    fillpoly(numPoints, poly);
}

void drawScore(int score) {
    setcolor(WHITE);
    int fontSize = std::min(screenWidth, screenHeight) / 30; // Determine font size based on screen dimensions
    settextstyle(DEFAULT_FONT, HORIZ_DIR, fontSize);
    char buffer[50];
    sprintf(buffer, "Score: %d", score);
    outtextxy(10, 10, buffer);
}

void gameOverAnimation() {
    cleardevice();
    setcolor(RED);
    settextstyle(3, 0, 6);
    outtextxy(screenWidth / 2 - 200, screenHeight / 2 - 100, "Game Over!");
    delay(2000); // Delay for 2 seconds
}

void winnerAnimation() {
    cleardevice();
    setcolor(GREEN);
    settextstyle(3, 0, 6);
    outtextxy(screenWidth / 2 - 200, screenHeight / 2 - 100, "Winner!");
    delay(2000); // Delay for 2 seconds
}

// Helper functions to check keyboard state asynchronously
bool isLeftKeyPressed() {
    return GetAsyncKeyState(VK_LEFT) & 0x8000;
}

bool isRightKeyPressed() {
    return GetAsyncKeyState(VK_RIGHT) & 0x8000;
}

bool isSpaceKeyPressed() {
    return GetAsyncKeyState(VK_SPACE) & 0x8000;
}
