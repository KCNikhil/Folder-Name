#include <graphics.h>
#include <conio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define MAX_ASTEROIDS 5
#define PLAYER_SPEED 10
#define BULLET_SPEED 10
#define ASTEROID_SPEED 5

struct Asteroid {
    int x, y;
    int size;
    bool active;
};

int screenWidth, screenHeight;

void drawPlayer(int x, int y, int playerSize);
void drawBullet(int x, int y, int bulletSize);
void drawAsteroid(int x, int y, int asteroidSize);
void drawScore(int score);

bool isLeftKeyPressed();
bool isRightKeyPressed();
bool isSpaceKeyPressed();

int main() {
    int gd = DETECT, gm;
    initgraph(&gd, &gm, "");

    screenWidth = getmaxx();
    screenHeight = getmaxy();

    srand(time(NULL));

    int playerSize = screenWidth / 20;
    int playerX = screenWidth / 2 - playerSize / 2, playerY = screenHeight - playerSize - 20;

    int bullets[MAX_ASTEROIDS][3];
    for (int i = 0; i < MAX_ASTEROIDS; ++i) {
        bullets[i][2] = 0;
    }

    int score = 0;

    Asteroid asteroids[MAX_ASTEROIDS];
    for (int i = 0; i < MAX_ASTEROIDS; ++i) {
        asteroids[i].size = screenWidth / 20 + rand() % (screenWidth / 20);
        asteroids[i].x = rand() % (screenWidth - asteroids[i].size);
        asteroids[i].y = rand() % (screenHeight / 2) - screenHeight;
        asteroids[i].active = true;
    }

    while (!kbhit()) {
        cleardevice();

        drawPlayer(playerX, playerY, playerSize);
        drawScore(score);

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
            for (int i = 0; i < MAX_ASTEROIDS; ++i) {
                if (bullets[i][2] == 0) {
                    bullets[i][0] = playerX + playerSize / 2;
                    bullets[i][1] = playerY - playerSize / 2;
                    bullets[i][2] = 1;
                    break;
                }
            }
        }

        for (int i = 0; i < MAX_ASTEROIDS; ++i) {
            if (bullets[i][2] == 1) {
                drawBullet(bullets[i][0], bullets[i][1], 5);
                bullets[i][1] -= BULLET_SPEED;

                if (bullets[i][1] < 0) {
                    bullets[i][2] = 0;
                }

                for (int j = 0; j < MAX_ASTEROIDS; ++j) {
                    if (asteroids[j].active && bullets[i][0] >= asteroids[j].x && bullets[i][0] <= asteroids[j].x + asteroids[j].size &&
                        bullets[i][1] >= asteroids[j].y && bullets[i][1] <= asteroids[j].y + asteroids[j].size) {
                        asteroids[j].active = false;
                        bullets[i][2] = 0;
                        score += 10;
                    }
                }
            }
        }

        for (int i = 0; i < MAX_ASTEROIDS; ++i) {
            if (asteroids[i].active) {
                drawAsteroid(asteroids[i].x, asteroids[i].y, asteroids[i].size);
                asteroids[i].y += ASTEROID_SPEED;

                if (asteroids[i].y > screenHeight) {
                    asteroids[i].size = screenWidth / 20 + rand() % (screenWidth / 20);
                    asteroids[i].x = rand() % (screenWidth - asteroids[i].size);
                    asteroids[i].y = rand() % (screenHeight / 2) - screenHeight;
                    asteroids[i].active = true;
                }
            }
        }

        delay(20); // Adjust delay for smoother gameplay
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

    int numPoints = 10 + rand() % 5; // Random number of points for variety
    int radius = asteroidSize / 2;
    int poly[20]; // Array to hold x, y pairs for points

    for (int i = 0; i < numPoints; ++i) {
        float angle = 2 * 3.14159 * i / numPoints;
        int variation = rand() % (radius / 2); // Random variation to create irregularity
        poly[2 * i] = x + radius + cos(angle) * (radius + variation);
        poly[2 * i + 1] = y + radius + sin(angle) * (radius + variation);
    }

    fillpoly(numPoints, poly);
}

void drawScore(int score) {
    setcolor(WHITE);
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 1);
    char buffer[50];
    sprintf(buffer, "Score: %d", score);
    outtextxy(10, 10, buffer);
}

bool isLeftKeyPressed() {
    return GetAsyncKeyState(VK_LEFT) & 0x8000;
}

bool isRightKeyPressed() {
    return GetAsyncKeyState(VK_RIGHT) & 0x8000;
}

bool isSpaceKeyPressed() {
    return GetAsyncKeyState(VK_SPACE) & 0x8000;
}
