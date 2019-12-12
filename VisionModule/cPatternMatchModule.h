#ifndef CPATTERNMATCHMODULE_H
#define CPATTERNMATCHMODULE_H

#include <VisionModule/CVisionModule.h>
#include <patternMatch.h>

class CPatternMatchParams
{
public:
    CPatternMatchParams():m_TplFolder(""),m_TplName(""),m_iResizeRatio(4){};
    cv::String m_TplFolder;
    cv::String m_TplName;
    int        m_iResizeRatio;
};

class CPatternMatchModule : public CVisionModule
{
public:
    CPatternMatchModule();
    virtual CVisionAgentResult RunVision(cv::Mat srcImg, cv::Mat& dispImg);
    virtual std::string GetName();

    void InitPath(cv::String tplFolder, cv::String templatePath);
    void SetResizeRatio(const int resizeRatio);
    void MakeNewTemplate(const int resizeRatio);
    std::string GetContourName() const;

public:
    PatternMatch m_Pattern;
    int         m_iResizeRatio;
    CPatternMatchParams m_Params;
};

#endif // CPATTERNMATCHMODULE_H
