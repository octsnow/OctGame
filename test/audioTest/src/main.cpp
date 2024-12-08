#include "OctGame.hpp"

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

using namespace std;

namespace {
    const clock_t FrameTime = 1000 / FPS;
    struct {
        clock_t countTime = 0;
        clock_t lastTime;
    } g_sys_info;

    OctGame g_oct_game;

    void Update() {
        g_oct_game.DrawText(0, 0, 0x00FFFFFF, "play: p");
        g_oct_game.DrawText(0, 30, 0x00FFFFFF, "stop: s");
        g_oct_game.DrawText(0, 60, 0x00FFFFFF, "pause: d");

        if(g_oct_game.IsDown('p')) {
            g_oct_game.audio.Play("BGM");
        }
        if(g_oct_game.IsDown('s')) {
            g_oct_game.audio.Stop("BGM");
        }
        if(g_oct_game.IsDown('d')) {
            g_oct_game.audio.Pause("BGM");
        }
        g_oct_game.Update();
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

    glClearColor(0.0, 0.0, 1.0, 0.0);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    g_oct_game.audio.Load("anoband.wav", "BGM");
    g_oct_game.audio.SetVolume("BGM", 0.01);
    g_oct_game.audio.SetIsLooping("BGM", true);

    g_oct_game.audio.Play("BGM");
    glutMainLoop();

    g_oct_game.Destroy();
}
