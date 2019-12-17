#ifndef TABUI_H
#define TABUI_H

namespace Ui { class OneBody; }

class OneBody;

class TabUI
{
public:
    TabUI();
    void SetMainUi(Ui::OneBody * tmpUi);

protected:
    Ui::OneBody * ui;
    OneBody * m_MainWindow;
};

#endif // TABUI_H
