#include "AccordionSection.h"
#include "../Application.h"

#include <QDebug>
#include <QPropertyAnimation>

namespace hdps
{

namespace gui
{

AccordionSection::AccordionSection(const QString& title, QWidget* parent /*= nullptr*/) :
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

    _contentLayout.setMargin(0);
    _contentLayout.setContentsMargins(0, 0, 0, 0);

    _toggleButton.setCheckable(true);
    _toggleButton.setChecked(true);
    
    _toggleButton.setLayout(&_frameLayout);

    QObject::connect(&_toggleButton, &QPushButton::toggled, [this](bool checked) {
        setExpanded(checked);
    });

    updateLeftLabel();
}

void AccordionSection::setWidget(QWidget* widget)
{
    Q_ASSERT(widget != nullptr);

    _widget = widget;

    _contentLayout.addWidget(widget);

    updateTitleLabel();
    updateRightLabel();

    QObject::connect(widget, &QWidget::windowTitleChanged, [this](const QString& title) {
        updateTitleLabel();
    });

    QObject::connect(widget, &QWidget::windowIconChanged, [this](const QIcon& icon) {
        updateRightLabel();
    });
}

void AccordionSection::expand()
{
    setExpanded(true);
}

void AccordionSection::collapse()
{
    setExpanded(false);
}

void AccordionSection::setExpanded(const bool& expanded)
{
    QSignalBlocker toggleButtonBlocker(&_toggleButton);

    _toggleButton.setChecked(expanded);

    _widget->setVisible(expanded);

    updateLeftLabel();

    emit expandedChanged(isExpanded());
}

bool AccordionSection::isExpanded() const
{
    return _toggleButton.isChecked();
}

QString AccordionSection::getTitle() const
{
    Q_ASSERT(_widget != nullptr);

    return _widget->windowTitle();
}

void AccordionSection::updateLeftLabel()
{
    const auto iconName = _toggleButton.isChecked() ? "angle-right" : "angle-down";
    const auto icon     = Application::getIconFont("FontAwesome").getIcon(iconName);

    _leftIconLabel.setPixmap(icon.pixmap(ICON_SIZE));
}

void AccordionSection::updateTitleLabel()
{
    _titleLabel.setText(_widget->windowTitle());
}

void AccordionSection::updateRightLabel()
{
    _rightIconLabel.setPixmap(_widget->windowIcon().pixmap(ICON_SIZE));
}

}

}