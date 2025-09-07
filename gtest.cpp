#include<graphics.h>

const int WIDTH = 800;
const int HEIGHT = 700;
const char* TITLE = "TESTING WINDOW";

main()
{
    initwindow(WIDTH, HEIGHT, TITLE);

    setcolor(GREEN);
    arc(400, 350, 0, 120, 100);
    line(400, 350, 500, 350);
    setcolor(YELLOW);
    arc(400, 350, 120, 240, 100);
    line(400, 350, 350, 263.375);
    setcolor(RED);
    arc(400, 350, 240, 0, 100);
    line(400, 350, 350, 436.6025);
    getch();
}