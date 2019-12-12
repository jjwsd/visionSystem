#include "cPatternMatchModule.h"

CPatternMatchModule::CPatternMatchModule():m_iResizeRatio(4)
{
    this->m_eVisionType = VISION::PATTERN;
}

CVisionAgentResult CPatternMatchModule::RunVision(Mat srcImg, Mat &dispImg)
{
    CVisionAgentResult tempResult;
    if(m_Pattern.doCore(srcImg, false, m_iResizeRatio) == SUCCESS)
    {
        tempResult.bOk = true;
        m_Pattern.getResult(dispImg, tempResult.centerPt, tempResult.dAngle);        
    }
    return tempResult;
}

string CPatternMatchModule::GetName()
{
    return "PatternMatch";
}

void CPatternMatchModule::InitPath(cv::String tplFolder, cv::String templatePath)
{
    m_Pattern.initialize(tplFolder, templatePath);
}

void CPatternMatchModule::SetResizeRatio(const int resizeRatio)
{
    m_iResizeRatio = resizeRatio;
}

void CPatternMatchModule::MakeNewTemplate(const int resizeRatio)
{
    m_iResizeRatio = resizeRatio;
    m_Pattern.MakeTemplateImg(resizeRatio);
}

string CPatternMatchModule::GetContourName() const
{
    return m_Pattern.GetConTourImgName();
}

