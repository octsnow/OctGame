#pragma once

#include "octgamedef.hpp"

#include <opencv2/opencv.hpp>
#include <vector>
#include <string>

#define ILFAILED -1

typedef int GHandle;

class OCT_EXPORTS ImageList {
public:
    GHandle LoadImageFile(std::string filepath, bool isBmp=false);
    GHandle LoadImageFile(std::string filepath, float sx, float sy, bool isBmp=false);
    GHandle LoadRegionImageFile(std::string filepath, int x, int y, int n, bool isBmp=false);
    GHandle LoadRegionImageFile(std::string filepath, float sx, float sy, int x, int y, int n, bool isBmp=false);
    cv::Mat* GetImage(GHandle handle);
private:
    std::vector<cv::Mat> mImageList;
};
