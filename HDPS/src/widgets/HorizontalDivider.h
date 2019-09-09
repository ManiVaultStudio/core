#pragma once

#include <QHBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QString>

namespace hdps
{
    namespace gui
    {
        class HorizontalDivider : public QHBoxLayout
        {
        public:
            HorizontalDivider(QWidget* parent = 0);

            void setTitle(QString title);

        private:
            QString _title;

            QLabel* _titleLabel;
            QFrame* _divider;
        };
    }
}
