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

    void Update() {
        static int c = 0;
        float ratio = sin(2 * PI * (c / 360.0f));
        int centerX = SCREEN_W / 2;
        int centerY = SCREEN_H / 2;
        int imageHalfWidth = 300 * ratio / 2;
        int imageHalfHeight = 300 * ratio / 2;
        g_oct_game.DrawText(0, 0, 0xFFFFFF, "ratio=%f\n", ratio);
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
