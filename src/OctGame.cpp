#include "OctGame.hpp"
#include <stdarg.h>

#define KEYARRAY_SIZE 256

using namespace std;

namespace {
    bool pressedKeys[KEYARRAY_SIZE];
    bool upKeys[KEYARRAY_SIZE];
    bool downKeys[KEYARRAY_SIZE];

    void Key(unsigned char key, int x, int y) {
        if(!pressedKeys[(int)key]) {
            downKeys[(int)key] = true;
        }
        pressedKeys[(int)key] = true;
    }

    void KeyUp(unsigned char key, int x, int y) {
        upKeys[(int)key] = true;
        pressedKeys[(int)key] = false;
    }

    void ResetKeys() {
        for(int i = 0; i < KEYARRAY_SIZE; i++) {
            upKeys[i] = 0;
            downKeys[i] = 0;
        }
    }

    int Clamp(int n, int min, int max) {
        if(n < min) return min;
        if(n > max) return max;
        return n;
    }

    inline void DrawPixel(uint8_t* pDst, uint8_t src, float alpha) {
        *pDst = src * alpha + *pDst * (1.0 - alpha);
    }
}

OCT_EXPORTS void OctGame::Init(int* argc, char** argv, int width, int height) {
    this->mWidth = width;
    this->mHeight = height;

    glutInitWindowPosition(0, 0);
    glutInitWindowSize(width, height);
    glutInit(argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
    glutCreateWindow(argv[0]);
     
    glutKeyboardFunc(Key);
    glutKeyboardUpFunc(KeyUp);
 
    BITMAPINFO bmi;
    ZeroMemory(&bmi, sizeof(bmi));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 24;
    bmi.bmiHeader.biCompression = BI_RGB;

    HDC tmpDC = GetDC(GetDesktopWindow());
    this->mHBitmap = CreateDIBSection(
          tmpDC, &bmi, DIB_RGB_COLORS, (void**)&this->mScreen, 0, 0);

    if(this->mHBitmap == NULL) {
        cout << "hBitmap is NULL" << endl;
        return;
    }

    this->mHDC = CreateCompatibleDC(tmpDC);
    this->mH01dHandle = SelectObject(this->mHDC, this->mHBitmap);
    SetBkMode(this->mHDC, TRANSPARENT);

    ReleaseDC(GetDesktopWindow(), tmpDC);

    this->mHFont = CreateFont(
        30, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        VARIABLE_PITCH | FF_ROMAN, NULL
    );
    this->mTextColor = RGB(0xFF, 0xFF, 0xFF);
    SetTextColor(this->mHDC, this->mTextColor);
    SelectObject(this->mHDC, this->mHFont);
}

OCT_EXPORTS void OctGame::Update() {
    ResetKeys();
}

OCT_EXPORTS void OctGame::DisplayFunc(void (*func)(void)) {
    glutDisplayFunc(func);
}

OCT_EXPORTS void OctGame::ReshapeFunc(void (*func)(int, int)) {
    glutReshapeFunc(func);
}

OCT_EXPORTS void OctGame::IdleFunc(void (*func)(void)) {
    glutIdleFunc(func);
}

OCT_EXPORTS void OctGame::MouseFunc(void (*func)(int, int, int, int)) {
    glutMouseFunc(func);
}

OCT_EXPORTS void OctGame::MouseDragFunc(void (*func)(int, int)) {
    glutMotionFunc(func);
}

OCT_EXPORTS void OctGame::MouseMoveFunc(void (*func)(int, int)) {
    glutPassiveMotionFunc(func);
}

OCT_EXPORTS HWND OctGame::GetWindowHandle() {
    return WindowFromDC(this->mHDC);
}

OCT_EXPORTS void OctGame::Destroy() {
    SelectObject(this->mHDC, GetStockObject(SYSTEM_FONT));
    DeleteObject(this->mHFont);

    SelectObject(this->mHDC, this->mH01dHandle);
    DeleteDC(this->mHDC);
    DeleteObject(this->mHBitmap);
}

OCT_EXPORTS int OctGame::LoadImageFile(string filepath, bool isBmp) {
    return this->mImgList.LoadImageFile(filepath, isBmp);
}

OCT_EXPORTS int OctGame::LoadImageFile(string filepath, float sx, float sy, bool isBmp) {
    return this->mImgList.LoadImageFile(filepath, sx, sy, isBmp);
}

OCT_EXPORTS int OctGame::LoadRegionImageFile(string filepath, int width, int height, int n, bool isBmp) {
    return this->mImgList.LoadRegionImageFile(filepath, width, height, n, isBmp);
}

OCT_EXPORTS int OctGame::LoadRegionImageFile(string filepath, float sx, float sy, int width, int height, int n, bool isBmp) {
    return this->mImgList.LoadRegionImageFile(filepath, sx, sy, width, height, n, isBmp);
}

OCT_EXPORTS void OctGame::DrawBox(int x1, int y1, int x2, int y2, int color, bool fillFlag, float alpha) {
    int minX = x1 < x2 ? x1 : x2;
    int minY = this->mHeight - (y1 > y2 ? y1 : y2);
    int maxX = x1 > x2 ? x1 : x2;
    int maxY = this->mHeight - (y1 < y2 ? y1 : y2) + 1;

    minX = Clamp(minX, 0, this->mWidth);
    maxX = Clamp(maxX, 0, this->mWidth);
    minY = Clamp(minY, 0, this->mHeight);
    maxY = Clamp(maxY, 0, this->mHeight);
    if(fillFlag) {
        for(int y = minY; y < maxY; y++) {
            for(int x = minX; x < maxX; x++) {
                for(int c = 0; c < 3; c++) {
                    uint8_t src = (uint8_t)((color >> ((2 - c) * 8)) & 0xFF);
                    DrawPixel(&this->mScreen[(y * this->mWidth + x) * 3 + c], src, alpha);
                    //this->mScreen[index] = src * alpha + dst * (1 - alpha);
                }
            }
        }
    } else {
        for(int x = minX; x < maxX; x++) {
            for(int c = 0; c < 3; c++) {
                uint8_t src = (uint8_t)((color >> ((2 - c) * 8)) & 0xFF);
                DrawPixel(&this->mScreen[(minY * this->mWidth + x) * 3 + c], src, alpha);
                DrawPixel(&this->mScreen[((maxY - 1) * this->mWidth + x) * 3 + c], src, alpha);
            }
        }

        for(int y = minY; y < maxY; y++) {
            for(int c = 0; c < 3; c++) {
                uint8_t src = (uint8_t)((color >> ((2 - c) * 8)) & 0xFF);
                DrawPixel(&this->mScreen[(y * this->mWidth + minX) * 3 + c], src, alpha);
                DrawPixel(&this->mScreen[(y * this->mWidth + (maxX - 1)) * 3 + c], src, alpha);
            }
        }
    }
}

OCT_EXPORTS void OctGame::DrawImage(GHandle handle, int dx, int dy, bool transpose, bool isReverse){
    cv::Mat* img = this->mImgList.GetImage(handle);
    uchar* data = img->data;

    if(img == nullptr) {
        return;
    }

    int imgC = img->channels();
    int imgW = img->cols;
    int imgH = img->rows;
    float tp = 1.0f;

    if(imgC <= 3) transpose = false;

    for(int y = 0; y < imgH && y + dy < this->mHeight; y++){
        for(int x = 0; x < imgW && x + dx < this->mWidth; x++) {
            if(x + dx < 0 || y + dy < 0) continue;

            int ix = x;
            int iy = y;

            if(isReverse) {
                ix = imgW - x - 1;
            }

            if(transpose) {
                tp = img->data[(iy * imgW + ix) * imgC + 3] / 255.0f;
                if(tp == 0) continue;
            }

            for(int c = 0; c < 3; c++){
                int dtIdx = ((this->mHeight - (y + dy) - 1) * this->mWidth + (x + dx)) * 3 + c;
                int scIdx = (iy * imgW + ix) * imgC + c;
                this->mScreen[dtIdx] = this->mScreen[dtIdx] * (1 - tp) + img->data[scIdx] * tp;
            }
        }
    }
}

OCT_EXPORTS void OctGame::DrawText(int x, int y, COLORREF color, const char* format, ...) {
    char buf[256];
    va_list ap;
    va_start(ap, format);
    vsprintf_s(buf, 256, format, ap);
    va_end(ap);

    if(this->mTextColor != color) {
        this->mTextColor = color;
        SetTextColor(this->mHDC, this->mTextColor);
    }
    TextOut(this->mHDC, x, y, buf, lstrlen(buf));
}

OCT_EXPORTS void OctGame::DrawText(int x, int y, COLORREF color, int size, const char* format, ...) {
    this->mHFont = CreateFont(
        size, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        VARIABLE_PITCH | FF_ROMAN, NULL
    );

    HFONT font = static_cast<HFONT>(SelectObject(this->mHDC, this->mHFont));

    char buf[256];
    va_list ap;
    va_start(ap, format);
    vsprintf_s(buf, 256, format, ap);
    va_end(ap);

    if(this->mTextColor != color) {
        this->mTextColor = color;
        SetTextColor(this->mHDC, this->mTextColor);
    }
    TextOut(this->mHDC, x, y, buf, lstrlen(buf));

    SelectObject(this->mHDC, font);
}

OCT_EXPORTS void OctGame::ClearScreen(){
    memset(this->mScreen, 0, this->mHeight * this->mWidth * 3);
}

OCT_EXPORTS void OctGame::ScreenSwap() {
    glDrawPixels(this->mWidth, this->mHeight, GL_RGB, GL_UNSIGNED_BYTE, this->mScreen );
    glutSwapBuffers();
}

OCT_EXPORTS bool OctGame::IsPressed(char key) {
    return pressedKeys[(int)key];
}

OCT_EXPORTS bool OctGame::IsUp(char key) {
    return upKeys[(int)key];
}

OCT_EXPORTS bool OctGame::IsDown(char key) {
    return downKeys[(int)key];
}
