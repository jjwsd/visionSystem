#ifndef CVISIONMODULE_H
#define CVISIONMODULE_H

#include <S_Vision/CImageProcess.h>

#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>

namespace VISION{

enum VisionType{
    PATTERN = 0,
    CIRCLE = 1,
    RECTANGLE = 2,
};

}
class CVisionAgentResult
{
public:
    CVisionAgentResult():centerPt(0,0),iOccurrence(0),bOk(false),m_lStatus(0),dTaktTime(0.0){}
    enum status{
        CENTER_PT =0,
        ANGLE     =1,
        EXISTENCE =2,
    };

    cv::Point2f centerPt;
    double    dAngle;
    bool      bExistence;
    int       iOccurrence;
    bool      bOk;
    double    dTaktTime;

private:
    unsigned long m_lStatus;

};

class CVisionModule
{
public:
    CVisionModule():m_bDebugMode(false),m_eVisionType(VISION::PATTERN) {}
    virtual CVisionAgentResult RunVision(cv::Mat srcImg, cv::Mat& dispImg) =0;
    virtual std::string GetName() =0;

protected:
    CImageProcess   m_baseVision;

public:
    bool            m_bDebugMode;
    VISION::VisionType      m_eVisionType;
};

#endif // CVISIONMODULE_H
