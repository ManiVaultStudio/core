#include "HorizontalDivider.h"

namespace hdps
{
    namespace gui
    {
        HorizontalDivider::HorizontalDivider(QWidget* parent = 0) :
            QHBoxLayout(parent)
        {
            setMargin(0);
            _titleLabel = new QLabel();
            _titleLabel->setMargin(0);
            _titleLabel->setStyleSheet("QLabel { color: white; font-family: \"Open Sans\",Helvetica,Arial,sans-serif; font-size: 11px; }");

            _divider = new QFrame();
            _divider->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            _divider->setFixedHeight(1);
            _divider->setStyleSheet("QFrame { background-color: #888b93; }");

            addWidget(_titleLabel);
            addWidget(_divider);
        }

        void HorizontalDivider::setTitle(QString title)
        {
            _title = title;
            _titleLabel->setText(title);

            //removeWidget(_divider);
            //addWidget(_titleLabel);
            //addWidget(_divider);
        }
    }
}
