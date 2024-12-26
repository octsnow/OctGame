#include "OctGame.hpp"
#include <stdarg.h>

#define KEYARRAY_SIZE 256
#define DEFAULT_FONT_SIZE 30

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

    inline int Clamp(int n, int min, int max) {
        if(n < min) return min;
        if(n > max) return max;
        return n;
    }

    inline void DrawPixel(uint8_t* pDst, int width, int height, int x, int y, int color, float alpha) {
        for(int c = 0; c < 3; c++) {
            uint8_t src = (uint8_t)((color >> ((2 - c) * 8)) & 0xFF);
            uint8_t *pixel = &pDst[((height - y - 1) * width + x) * 3 + c];
            *pixel = src * alpha + *pixel * (1.0 - alpha);
        }
        //*pDst = src * alpha + *pDst * (1.0 - alpha);
    }

    inline void DrawPixelIfOnScreen(uint8_t* pDst, int width, int height, int x, int y, int color, float alpha) {
        if(x < 0 || x >= width || y < 0 || y >= height) {
            return;
        }
        DrawPixel(pDst, width, height, x, y, color, alpha);
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
        DEFAULT_FONT_SIZE, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
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

OCT_EXPORTS void OctGame::DrawLine(int x1, int y1, int x2, int y2, int color, float alpha) {
    float slope = 0;
    if(x1 != x2) {
        slope = (float)(y2 - y1) / (x2 - x1);
        if(x1 > x2) {
            int tmp = x1;
            x1 = x2;
            x2 = tmp;
            tmp = y1;
            y1 = y2;
            y2 = tmp;
        }
    } else if(y1 < 0 || y2 >= this->mHeight) {
        return;
    }

    if(x2 < 0 || x1 >= this->mWidth) {
        return;
    }
    if(x1 < 0) {
        y1 += slope * (-x1);
        x1 = 0;
    }
    if(x2 >= this->mWidth) {
        y2 -= slope * (x2 - this->mWidth - 1);
        x2 = this->mWidth - 1;
    }

    slope = 0;
    if(y1 != y2) {
        slope = (float)(x2 - x1) / (y2 - y1);
        if(y1 > y2) {
            int tmp = x1;
            x1 = x2;
            x2 = tmp;
            tmp = y1;
            y1 = y2;
            y2 = tmp;
        }
    } else if(x1 < 0 || x2 >= this->mWidth) {
        return;
    }

    if(y2 < 0 || y1 >= this->mHeight) {
        return;
    }
    if(y1 < 0) {
        x1 += slope * (-y1);
        y1 = 0;
    }
    if(y2 >= this->mHeight) {
        x2 -= slope * (y2 - this->mHeight - 1);
        y2 = this->mHeight - 1;
    }

    int dx = abs(x1 - x2);
    int dy = abs(y1 - y2);

    if(dx > dy) {
        if(x1 > x2) {
            int tmp = x1;
            x1 = x2;
            x2 = tmp;
            tmp = y1;
            y1 = y2;
            y2 = tmp;
        }
        dy = y2 - y1;

        int x = 0;
        int y = 0;
        int f = 0;
        int k = y1 < y2 ? 1 : -1;
        while(x <= dx) {
            int _x = x + x1;
            int _y = y * k + y1;
            DrawPixel(
                    this->mScreen,
                    this->mWidth,
                    this->mHeight,
                    _x, _y,
                    color,
                    alpha);

            f = f + k * (dy << 1) - dx;
            if(f < 0) {
                x++;
                f += dx;
            } else {
                x++;
                y++;
                f -= dx;
            }
        }
    } else if(dy > dx) {
        if(y1 > y2) {
            int tmp = x1;
            x1 = x2;
            x2 = tmp;
            tmp = y1;
            y1 = y2;
            y2 = tmp;
        }
        dx = x2 - x1;

        int x = 0;
        int y = 0;
        int f = 0;
        int k = x1 < x2 ? 1 : -1;
        while(y <= dy) {
            int _x = x * k + x1;
            int _y = y + y1;
            DrawPixel(
                    this->mScreen,
                    this->mWidth,
                    this->mHeight,
                    _x, _y,
                    color,
                    alpha);

            f = f + k * (dx << 1) - dy;
            if(f < 0) {
                y++;
                f += dy;
            } else {
                x++;
                y++;
                f -= dy;
            }
        }
    }
}

OCT_EXPORTS void OctGame::DrawBox(int x1, int y1, int x2, int y2, int color, bool fillFlag, float alpha) {
    int minX = x1 < x2 ? x1 : x2;
    int minY = y1 < y2 ? y1 : y2;
    int maxX = x1 > x2 ? x1 : x2;
    int maxY = y1 > y2 ? y1 : y2;

    if(maxX < 0 || maxY < 0 || minX >= this->mWidth || minY >= this->mHeight) {
        return;
    }

    if(fillFlag) {
        minX = Clamp(minX, 0, this->mWidth - 1);
        maxX = Clamp(maxX, 0, this->mWidth - 1);
        minY = Clamp(minY, 0, this->mHeight - 1);
        maxY = Clamp(maxY, 0, this->mHeight - 1);
        for(int y = minY; y < maxY; y++) {
            for(int x = minX; x < maxX; x++) {
                DrawPixel(this->mScreen, this->mWidth, this->mHeight, x, y, color, alpha);
            }
        }
    } else {
        int s = minX;
        int e = maxX;
        if(minX < 0) {
            s = 0;
        }
        if(maxX >= this->mWidth) {
            e = this->mWidth - 1;
        }
        if(minY < 0) {
            for(int x = s; x <= e; x++) {
                DrawPixel(this->mScreen, this->mWidth, this->mHeight, x, maxY, color, alpha);
            }
        } else if(maxY >= this->mHeight) {
            for(int x = s; x <= e; x++) {
                DrawPixel(this->mScreen, this->mWidth, this->mHeight, x, minY, color, alpha);
            }
        } else {
            for(int x = s; x <= e; x++) {
                DrawPixel(this->mScreen, this->mWidth, this->mHeight, x, minY, color, alpha);
                DrawPixel(this->mScreen, this->mWidth, this->mHeight, x, maxY, color, alpha);
            }
        }

        s = minY;
        e = maxY;
        if(minY < 0) {
            s = 0;
        }
        if(maxY >= this->mHeight) {
            e = this->mHeight - 1;
        }
        if(minX < 0) {
            for(int y = s; y <= e; y++) {
                DrawPixel(this->mScreen, this->mWidth, this->mHeight, maxX, y, color, alpha);
            }
        } else if(maxX >= this->mWidth) {
            for(int y = s; y <= e; y++) {
                DrawPixel(this->mScreen, this->mWidth, this->mHeight, minX, y, color, alpha);
            }
        } else {
            for(int y = s; y <= e; y++) {
                DrawPixel(this->mScreen, this->mWidth, this->mHeight, minX, y, color, alpha);
                DrawPixel(this->mScreen, this->mWidth, this->mHeight, maxX, y, color, alpha);
            }
        }
    }
}

OCT_EXPORTS void OctGame::DrawCircle(int x0, int y0, int r, int color, bool fillFlag, float alpha) {
    int x = r;
    int y = 0;
    int f = 0;

    if(fillFlag) {
        while(y <= x) {
            for(int i = 0; i <= x; i++) {
                DrawPixelIfOnScreen(this->mScreen, this->mWidth, this->mHeight, x0 + i, y0 + y, color, alpha);
                DrawPixelIfOnScreen(this->mScreen, this->mWidth, this->mHeight, x0 + i, y0 - y, color, alpha);
                DrawPixelIfOnScreen(this->mScreen, this->mWidth, this->mHeight, x0 - i, y0 + y, color, alpha);
                DrawPixelIfOnScreen(this->mScreen, this->mWidth, this->mHeight, x0 - i, y0 - y, color, alpha);
                DrawPixelIfOnScreen(this->mScreen, this->mWidth, this->mHeight, x0 + y, y0 + i, color, alpha);
                DrawPixelIfOnScreen(this->mScreen, this->mWidth, this->mHeight, x0 + y, y0 - i, color, alpha);
                DrawPixelIfOnScreen(this->mScreen, this->mWidth, this->mHeight, x0 - y, y0 + i, color, alpha);
                DrawPixelIfOnScreen(this->mScreen, this->mWidth, this->mHeight, x0 - y, y0 - i, color, alpha);
            }
            f = f + 5 - (x << 2) + (y << 3);
            if(f < 0) {
                y++;
                f = f - 1 + (x << 2);
            } else {
                x--;
                y++;
                f = f + 3 - (x << 2);
            }
        }
    } else {
        while(y <= x) {
            DrawPixelIfOnScreen(this->mScreen, this->mWidth, this->mHeight, x0 + x, y0 + y, color, alpha);
            DrawPixelIfOnScreen(this->mScreen, this->mWidth, this->mHeight, x0 + x, y0 - y, color, alpha);
            DrawPixelIfOnScreen(this->mScreen, this->mWidth, this->mHeight, x0 - x, y0 + y, color, alpha);
            DrawPixelIfOnScreen(this->mScreen, this->mWidth, this->mHeight, x0 - x, y0 - y, color, alpha);
            DrawPixelIfOnScreen(this->mScreen, this->mWidth, this->mHeight, x0 + y, y0 + x, color, alpha);
            DrawPixelIfOnScreen(this->mScreen, this->mWidth, this->mHeight, x0 + y, y0 - x, color, alpha);
            DrawPixelIfOnScreen(this->mScreen, this->mWidth, this->mHeight, x0 - y, y0 + x, color, alpha);
            DrawPixelIfOnScreen(this->mScreen, this->mWidth, this->mHeight, x0 - y, y0 - x, color, alpha);
            f = f + 5 - (x << 2) + (y << 3);
            if(f < 0) {
                y++;
                f = f - 1 + (x << 2);
            } else {
                x--;
                y++;
                f = f + 3 - (x << 2);
            }
        }
    }
}

OCT_EXPORTS void OctGame::DrawImage(GHandle handle, int x, int y, bool transpose, bool isReverseX, bool isReverseY){
    cv::Mat* pImage = this->mImgList.GetImage(handle);
    uchar* data = pImage->data;

    if(pImage == nullptr) {
        return;
    }

    int imageChannel = pImage->channels();
    int imageWidth = pImage->cols;
    int imageHeight = pImage->rows;
    float tp = 1.0f;

    if(imageChannel <= 3) {
        transpose = false;
    }

    for(int _y = 0; _y < imageHeight && _y + y < this->mHeight; _y++){
        for(int _x = 0; _x < imageWidth && _x + x < this->mWidth; _x++) {
            if(_x + x < 0 || _y + y < 0) continue;

            int xi = _x;
            int yi = _y;

            if(isReverseX) {
                xi = imageWidth - xi - 1;
            }

            if(isReverseY) {
                yi = imageHeight - yi - 1;
            }

            if(transpose) {
                tp = pImage->data[(yi * imageWidth + xi) * imageChannel + 3] / 255.0f;
                if(tp == 0) continue;
            }

            for(int c = 0; c < 3; c++){
                int dtIdx = ((this->mHeight - (_y + y) - 1) * this->mWidth + (_x + x)) * 3 + c;
                int scIdx = (yi * imageWidth + xi) * imageChannel + c;
                this->mScreen[dtIdx] = this->mScreen[dtIdx] * (1 - tp) + pImage->data[scIdx] * tp;
            }
        }
    }
}

OCT_EXPORTS void OctGame::DrawResizedImage(GHandle handle, int x1, int y1, int x2, int y2, bool transpose){
    cv::Mat* pImage = this->mImgList.GetImage(handle);

    if(pImage == nullptr) {
        return;
    }

    uchar* data = pImage->data;
    int imageChannel = pImage->channels();
    int imageWidth = pImage->cols;
    int imageHeight = pImage->rows;

    bool isReverseX = x1 > x2;
    bool isReverseY = y1 > y2;
    if(isReverseX) {
        int tmp = x1;
        x1 = x2;
        x2 = tmp;
    }
    if(isReverseY) {
        int tmp = y1;
        y1 = y2;
        y2 = tmp;
    }

    int dstAreaWidth = x2 - x1 + 1;
    int dstAreaHeight = y2 - y1 + 1;
    float ratioWidth = imageWidth / (float)dstAreaWidth;
    float ratioHeight = imageHeight / (float)dstAreaHeight;
    float tp = 1.0f;

    if(imageChannel <= 3) {
        transpose = false;
    }

    for(int _y = 0; _y < dstAreaHeight && _y + y1 < this->mHeight; _y++){
        for(int _x = 0; _x < dstAreaWidth && _x + x1 < this->mWidth; _x++) {
            if(_x + x1 < 0 || _y + 1 < 0) continue;

            int xi = ratioWidth * _x;
            int yi = ratioHeight * _y;

            if(isReverseX) {
                xi = imageWidth - xi - 1;
            }
            if(isReverseY) {
                yi = imageHeight - yi - 1;
            }

            if(transpose) {
                tp = data[(yi * imageWidth + xi) * imageChannel + 3] / 255.0f;
                if(tp == 0) continue;
            }

            for(int c = 0; c < 3; c++){
                int dstIdx = ((this->mHeight - (_y + y1) - 1) * this->mWidth + (_x + x1)) * 3 + c;
                int srcIdx = (yi * imageWidth + xi) * imageChannel + c;
                this->mScreen[dstIdx] = this->mScreen[dstIdx] * (1 - tp) + data[srcIdx] * tp;
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

    this->DrawText(x, y, color, DEFAULT_FONT_SIZE, buf);

    /*
    if(this->mTextColor != color) {
        this->mTextColor = color;
        SetTextColor(this->mHDC, this->mTextColor);
    }
    TextOut(this->mHDC, x, y, buf, lstrlen(buf));
    */
}

OCT_EXPORTS void OctGame::DrawText(int x, int y, COLORREF color, int size, const char* format, ...) {
    this->mHFont = CreateFont(
        size, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        VARIABLE_PITCH | FF_DONTCARE, TEXT("HGP行書体")
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
