#include "UI/TabUI.h"

#include <QApplication>

TabUI::TabUI()
{

}

void TabUI::SetMainUi(Ui::OneBody *tmpUi)
{
    ui = tmpUi;
    m_MainWindow = (OneBody *)(QApplication::activeWindow());
}
