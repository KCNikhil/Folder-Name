#include <graphics.h>
#include <conio.h>

int main() {
    // Initialize the graphics mode and graphics driver
    int gd = DETECT, gm;
    initgraph(&gd, &gm, "");

    // Set the color of the line
    setcolor(WHITE);

    // Draw a line from (100, 100) to (200, 200)
    line(100, 100, 200, 200);

    // Wait for a key press
    getch();

    // Close the graphics mode and deallocate all resources
    closegraph();

    return 0;
}
