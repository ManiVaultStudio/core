#pragma once

#include <QComboBox>

namespace hdps
{
    namespace gui
    {
        class ComboBox : public QComboBox
        {
        public:
            ComboBox(QWidget* parent = 0);
        };
    }
}
