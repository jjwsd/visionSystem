#ifndef CAMERADATA_H
#define CAMERADATA_H

#include "OneBody.h"

class OneBody;

class CameraData
{
public:
    CameraData();
    int m_iFrameCnt;
    bool m_bSaveNextImage;
    cv::Mat m_Frame;
    OneBody * m_pMainWindow;
};

#endif // CAMERADATA_H
