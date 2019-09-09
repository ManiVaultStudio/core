#include "PushButton.h"

namespace hdps
{
    namespace gui
    {
        PushButton::PushButton(QWidget* parent) :
            QPushButton(parent)
        {
            setMinimumHeight(30);

            setStyleSheet("QPushButton { background-color: #6e7279; color: white; border: none; }");
        }
    }
}
