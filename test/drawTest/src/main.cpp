#include "OctGame.hpp"

#include <cmath>
#include <time.h>
#include <windows.h>

#pragma comment(lib, "opencv_world455.lib")
#pragma comment(lib, "OpenAL32.lib")
#pragma comment(lib, "oct_game.lib")
#pragma comment(lib, "oct_wav.lib")
#pragma comment(lib, "oct_binary.lib")

#define SCREEN_H 600
#define SCREEN_W 800
#define STAGE_H 12
#define STAGE_W 100
#define BLOCK_SIZE 50
#define GRAVITY 1
#define FPS 60
#define OBJECT_COIN 0
#define PI 3.14159265358979

using namespace std;

namespace {
    const clock_t FrameTime = 1000 / FPS;
    struct {
        clock_t countTime = 0;
        clock_t lastTime;
    } g_sys_info;

    OctGame g_oct_game;
    GHandle g_image;

    void DrawDiameter(int cx, int cy, float rad) {
        int r = 50;
        int x1 = cos(rad) * r;
        int y1 = sin(rad) * r;
        int x2 = -x1;
        int y2 = -y1;
        x1 += cx;
        x2 += cx;
        y1 += cy;
        y2 += cy;
        g_oct_game.DrawLine(x1, y1, x2, y2, 0xFFFFFF);
    }

    void DrawBoxes(float ratio) {
        int size = 25 * ratio;
        g_oct_game.DrawBox(SCREEN_W / 4, -25, SCREEN_W / 4 + size, -25 + size, 0xFFFFFF, false);
        g_oct_game.DrawBox(SCREEN_W / 4, SCREEN_H - 25, SCREEN_W / 4 + size, SCREEN_H - 25 + size, 0xFFFFFF, true);
        g_oct_game.DrawBox(400, 10, 400 + size, 10 + size, 0xFFFFFF, false);
        g_oct_game.DrawBox(400, 70, 400 + size, 70 + size, 0xFFFFFF, true);
    }

    void DrawCircles(float ratio) {
        int size = 20 * ratio;
        g_oct_game.DrawCircle(0, SCREEN_H / 4, size, 0xFFFFFF, false);
        g_oct_game.DrawCircle(SCREEN_W, SCREEN_H / 4, size, 0xFFFFFF, true);
        g_oct_game.DrawCircle(100, SCREEN_H / 2, size, 0xFFFFFF, true);
        g_oct_game.DrawCircle(100, SCREEN_H / 2 + 110, size, 0xFFFFFF, false);
    }

    void Update() {
        static int c = 0;
        float radian = 2 * PI * (c / 360.0f);
        float ratio = sin(radian);
        int centerX = SCREEN_W / 2;
        int centerY = SCREEN_H / 2;
        int imageHalfWidth = 150 * ratio / 2;
        int imageHalfHeight = 150 * ratio / 2;
        g_oct_game.DrawText(0, 0, 0xFFFFFF, "ratio=%f\n", ratio);
        DrawBoxes(ratio + 1);
        DrawCircles(ratio + 1);
        DrawDiameter(0, SCREEN_H / 2, radian);
        DrawDiameter(SCREEN_W - 1, SCREEN_H / 2, radian);
        DrawDiameter(SCREEN_W / 2, 0, radian);
        DrawDiameter(SCREEN_W / 2, SCREEN_H - 1, radian);
        g_oct_game.DrawResizedImage(g_image, centerX - imageHalfWidth, centerY - imageHalfHeight, centerX + imageHalfWidth - 1, centerY + imageHalfHeight - 1, true);
        g_oct_game.Update();
        c = c >= 360 ? 0 : c + 1;
    }

    void idle() {
        glutPostRedisplay();
    }

    void display() {
        if(g_sys_info.countTime >= FrameTime) {
            glClear(GL_COLOR_BUFFER_BIT);
            g_oct_game.ClearScreen();
            Update();
            g_sys_info.countTime -= FrameTime;
            g_oct_game.ScreenSwap();
        }
        g_sys_info.countTime += clock() - g_sys_info.lastTime;
        g_sys_info.lastTime = clock();
    }

    void resize(int w, int h) {
        glViewport(0, 0, w, h);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(30.0, (double)w / (double)h, 1.0, 100.0);
        
        glMatrixMode(GL_MODELVIEW);
    }

}

int main(int argc, char** argv) {
    g_oct_game.Init(&argc, argv, SCREEN_W, SCREEN_H);
    g_oct_game.DisplayFunc(display);
    g_oct_game.ReshapeFunc(resize);
    g_oct_game.IdleFunc(idle);

    g_image = g_oct_game.LoadImageFile("1-yen.bmp", true);

    glClearColor(0.0, 0.0, 1.0, 0.0);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    glutMainLoop();

    g_oct_game.Destroy();
}
