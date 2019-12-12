#ifndef DEFINEDATA
#define DEFINEDATA

#include "opencv2/opencv.hpp"

#include <VisionModule/CVisionModule.h>

namespace CAM {
enum CamStreamMode{
    LIVE_STOP = 0,
    CAPTURE = 1,
    LIVE_STREAM = 2,
    AUTO_RUN_MODE = 3,
};

}

typedef cv::Mat _MatImg;
typedef CVisionAgentResult _VisionResult;

#endif // DEFINEDATA

