#ifndef CVISIONMODULEMGR_H
#define CVISIONMODULEMGR_H

#include <map>
#include <VisionModule/CVisionModule.h>

class CVisionModuleMgr
{
public:
    CVisionModuleMgr();
    ~CVisionModuleMgr();
    std::map<int, CVisionModule *> m_VisionModuleMap;
};

#endif // CVISIONMODULEMGR_H
