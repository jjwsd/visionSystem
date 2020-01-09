#ifndef CVISIONMODULE_H
#define CVISIONMODULE_H

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
    CVisionAgentResult():iOccurrence(0),bOk(false),dTaktTime(0.0),m_bUseCenterPoint(0),
        m_CenterPt(0,0),m_bUseAngle(0),m_lStatus(0){}
    enum status{
        CENTER_PT =0,
        ANGLE     =1,
        EXISTENCE =2,
    };

    void SetCenterPoint(cv::Point2f centerPt);
    bool GetCenterPoint(cv::Point2f &centerPt);

    void SetAngle(double dAngle);
    bool GetAngle(double &dAngle);

    bool      bExistence;
    int       iOccurrence;
    bool      bOk;
    double    dTaktTime;

private:
    bool        m_bUseCenterPoint;
    cv::Point2f m_CenterPt;

    bool        m_bUseAngle;
    double    m_dAngle;

    unsigned long m_lStatus;

};

inline void CVisionAgentResult::SetCenterPoint(cv::Point2f centerPt)
{
    m_bUseCenterPoint = true;
    m_CenterPt = centerPt;
}

inline void CVisionAgentResult::SetAngle(double dAngle)
{
    m_bUseAngle = true;
    m_dAngle = dAngle;
}

inline bool CVisionAgentResult::GetCenterPoint(cv::Point2f &centerPt)
{
    centerPt = m_CenterPt;
    return m_bUseCenterPoint;
}

inline bool CVisionAgentResult::GetAngle(double &dAngle)
{
    dAngle = m_dAngle;
    return m_bUseAngle;
}

class CVisionModule
{
public:
    CVisionModule(){};
    virtual ~CVisionModule(){}
    virtual void TestName() = 0;
    virtual CVisionAgentResult RunVision(cv::Mat srcImg, cv::Mat& dispImg) =0;
    virtual std::string GetName() =0;

public:
    bool            m_bDebugMode;
    VISION::VisionType      m_eVisionType;
};

#endif // CVISIONMODULE_H
