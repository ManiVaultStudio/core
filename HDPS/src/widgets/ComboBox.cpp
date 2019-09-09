#include "ComboBox.h"

namespace hdps
{
    namespace gui
    {
        ComboBox::ComboBox(QWidget* parent) :
            QComboBox(parent)
        {
            setMinimumHeight(30);
            
            setStyleSheet("QComboBox { background-color: white; color: #696969; border: none; border-radius: 2px; font-family: \"Open Sans\",Helvetica,Arial,sans-serif; font-size: 12px; }");
        }
    }
}
