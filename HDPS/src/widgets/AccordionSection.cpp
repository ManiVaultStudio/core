#include "AccordionSection.h"
#include "../Application.h"

#include <QDebug>
#include <QPropertyAnimation>

namespace hdps
{

namespace gui
{

AccordionSection::AccordionSection(const QString & title /*= ""*/, QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _mainLayout(),
    _frame(),
    _frameLayout(),
    _leftIconLabel(),
    _titleLabel(),
    _rightIconLabel(),
    _toggleButton(),
    _widget(nullptr)
{
    _mainLayout.setContentsMargins(0, 0, 0, 0);
    _mainLayout.addWidget(&_toggleButton);
    _mainLayout.addLayout(&_contentLayout);

    setLayout(&_mainLayout);

    _frame.setLayout(&_frameLayout);

    _frameLayout.addWidget(&_leftIconLabel);
    _frameLayout.addStretch(1);
    _frameLayout.addWidget(&_titleLabel);
    _frameLayout.addStretch(1);
    _frameLayout.addWidget(&_rightIconLabel);

    _frameLayout.setMargin(4);
    _frameLayout.setContentsMargins(5, 0, 5, 0);

    _titleLabel.setText(title);

    _contentLayout.setMargin(0);

    _toggleButton.setCheckable(true);
    _toggleButton.setChecked(true);
    
    _toggleButton.setLayout(&_frameLayout);

    const auto updateRightAlignedIcon = [this]() {
        const auto iconName = _toggleButton.isChecked() ? "angle-right" : "angle-down";
        const auto icon     = hdps::Application::getIconFont("FontAwesome").getIcon(iconName);

        _leftIconLabel.setPixmap(icon.pixmap(ICON_SIZE));
    };

    QObject::connect(&_toggleButton, &QPushButton::toggled, [this, updateRightAlignedIcon](bool checked) {
        _widget->setVisible(checked);
        updateRightAlignedIcon();
    });

    updateRightAlignedIcon();
}

void AccordionSection::setWidget(QWidget* widget)
{
    _widget = widget;

    _contentLayout.addWidget(widget);
}

void AccordionSection::setIcon(const QIcon& icon)
{
    _rightIconLabel.setPixmap(icon.pixmap(ICON_SIZE));
}

}

}