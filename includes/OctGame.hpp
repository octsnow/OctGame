#pragma once

#include "octgamedef.hpp"

#include <GL/freeglut.h>
#include <string>
#include "ImageList.hpp"
#include "Audio.hpp"

#define KEY_ESC 27

class OCT_EXPORTS OctGame {
public:
    void Init(int* argc, char** argv, int width, int height);
    void Destroy();

    void Update();

    void DisplayFunc(void (*func)(void));
    void ReshapeFunc(void (*func)(int, int));
    void IdleFunc(void (*func)(void));
    void MouseFunc(void (*func)(int, int, int, int));
    void MouseDragFunc(void (*func)(int, int));
    void MouseMoveFunc(void (*func)(int, int));

    HWND GetWindowHandle();

    // Imgae
    int LoadImageFile(std::string filepath, bool isBmp=false);
    int LoadImageFile(std::string filepath, float sx, float sy, bool isBmp=false);
    int LoadRegionImageFile(std::string filepath, int width, int height, int n, bool isBmp=false);
    int LoadRegionImageFile(std::string filepath, float sx, float sy, int width, int height, int n, bool isBmp=false);
    void DrawBox(int x1, int y1, int x2, int y2, int color, bool fillFlag=false, float alpha=1.0F);
    void DrawImage(GHandle handle, int x, int y, bool transpose=false, bool isReverseX=false, bool isReverseY=false);
    void DrawResizedImage(GHandle handle, int x1, int y1, int x2, int y2, bool transpose=false);
    void DrawText(int x, int y, COLORREF color, const char* format, ...);
    void DrawText(int x, int y, COLORREF color, int size, const char* format, ...);
    void ClearScreen();
    void ScreenSwap();

    bool IsPressed(char key);
    bool IsUp(char key);
    bool IsDown(char key);

    Audio audio;
private:
    uint8_t* mScreen;
    int mWidth;
    int mHeight;
    HDC mHDC;
    HFONT mHFont;
    HBITMAP mHBitmap;
    HGDIOBJ mH01dHandle;
    ImageList mImgList;
    COLORREF mTextColor;
};
