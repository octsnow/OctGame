#include "ImageList.hpp"

using namespace std;

namespace {
    cv::Mat ReadImage(string filepath, bool isBmp) {
#ifdef OCT_DEBUG
        cout << "start reading " << filepath << endl;
#endif
        cv::Mat img;

        img = cv::imread(filepath, cv::IMREAD_UNCHANGED);

        if(img.data == NULL) {
            return img;
        }

        if(isBmp) {
            cv::cvtColor(img, img, cv::COLOR_BGRA2RGBA);
        }

        if(img.channels() < 4) {
            const int dims[] = {img.cols, img.rows, 4};
            cv::Mat newImg = cv::Mat::ones(3, dims, img.type());

            for(int y = 0; y < img.rows; y++) {
                for(int x = 0; x < img.cols; x++) {
                    for(int c = 0; c < 3; c++) {
                        newImg.data[(y * img.cols + x)  * 4 + c] = img.data[(y * img.cols + x) * 3 + c];
                    }
                    newImg.data[(y * img.cols + x) * 4 + 3] = 1;
                }
            }
            img = newImg;
        }
#ifdef OCT_DEBUG
        cout << "end reading " << filepath << endl;
#endif

        return img;
    }

    cv::Mat ReadImage(string filepath, float sx, float sy, bool isBmp) {
        cv::Mat img = ReadImage(filepath, isBmp);
        cv::resize(img, img, cv::Size(), sx, sy);

        return img;
    }
}

/// <summary>Load image and add mImageList.</summary>
OCT_EXPORTS GHandle ImageList::LoadImageFile(string filepath, bool isBmp) {
    cv::Mat img = ReadImage(filepath, isBmp);

    if(img.data == NULL) {
        return ILFAILED;
    }

    this->mImageList.push_back(img);
    return this->mImageList.size() - 1;
}

OCT_EXPORTS GHandle ImageList::LoadImageFile(string filepath, float sx, float sy, bool isBmp) {
    this->mImageList.push_back(ReadImage(filepath, sx, sy, isBmp));
    return this->mImageList.size() - 1;
}

OCT_EXPORTS GHandle ImageList::LoadRegionImageFile(string filepath, int x, int y, int n, bool isBmp) {
    return this->LoadRegionImageFile(filepath, 1, 1, x, y, n, isBmp);
}

OCT_EXPORTS GHandle ImageList::LoadRegionImageFile(string filepath, float sx, float sy, int x, int y, int n, bool isBmp) {
    cv::Mat img = ReadImage(filepath, sx, sy, isBmp);
    int handle = this->mImageList.size();
    int nx = x * sx;
    int ny = y * sy;

    int i = 0;
    for(int r = 0; r * ny < img.rows; r++) {
        for(int c = 0; c * nx < img.cols; c++) {
            this->mImageList.push_back(
                    cv::Mat(
                        img,
                        cv::Rect(
                            c * nx,
                            r * ny,
                            c * nx + nx <= img.cols ? nx : img.cols - c * nx,
                            r * ny + ny <= img.rows ? ny : img.rows - r * ny)));
            if(++i >= n) return handle;
        }
    }

    return handle;
}

OCT_EXPORTS cv::Mat* ImageList::GetImage(GHandle handle) {
    if(handle >= mImageList.size()) {
        return nullptr;
    }

    return &this->mImageList[handle];
}
