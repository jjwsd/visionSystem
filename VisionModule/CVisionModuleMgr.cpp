#include "CVisionModuleMgr.h"

CVisionModuleMgr::CVisionModuleMgr()
{

}

CVisionModuleMgr::~CVisionModuleMgr()
{
    for (auto var = m_VisionModuleMap.begin(); var != m_VisionModuleMap.end(); ++var) {
        delete var->second;
    }
}

