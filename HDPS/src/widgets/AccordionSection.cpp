#include "AccordionSection.h"
#include "../Application.h"

#include <QDebug>
#include <QPropertyAnimation>

namespace hdps
{

namespace gui
{

AccordionSection::AccordionSection(QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _mainLayout(),
    _frame(),
    _frameLayout(),
    _leftIconLabel(),
    _titleLabel(),
    _subtitleLabel(),
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
    _frameLayout.addWidget(&_titleLabel);
    _frameLayout.addStretch(1);
    _frameLayout.addWidget(&_subtitleLabel);
    _frameLayout.addWidget(&_rightIconLabel);

    _frameLayout.setMargin(4);
    _frameLayout.setContentsMargins(5, 0, 5, 0);

    //_subtitleLabel.setStyleSheet("QLabel { color: rgb(100, 100, 100); }");

    _contentLayout.setMargin(0);
    _contentLayout.setContentsMargins(0, 0, 0, 0);

    _toggleButton.setCheckable(true);
    _toggleButton.setChecked(true);
    
    _toggleButton.setLayout(&_frameLayout);

    QObject::connect(&_toggleButton, &QPushButton::toggled, [this](bool checked) {
        setExpanded(checked);
    });

    updateLeftIcon();
}

QString AccordionSection::getName() const
{
    Q_ASSERT(_widget != nullptr);

    return _widget->objectName();
}

QWidget* AccordionSection::getWidget()
{
    return _widget;
}

void AccordionSection::setWidget(QWidget* widget)
{
    Q_ASSERT(widget != nullptr);

    _widget = widget;

    _contentLayout.addWidget(widget);

    updateTitleLabel();
    updateRightIcon();

    QObject::connect(widget, &QWidget::objectNameChanged, [this](const QString& objectName) {
        updateTitleLabel();
    });

    QObject::connect(widget, &QWidget::windowIconChanged, [this](const QIcon& icon) {
        updateRightIcon();
    });

    widget->installEventFilter(this);
}

bool AccordionSection::eventFilter(QObject* object, QEvent* event)
{
    if(event->type() == QEvent::DynamicPropertyChange) {
        
        const auto propertyEvent = static_cast<QDynamicPropertyChangeEvent*>(event);

        if (propertyEvent) {
            const auto propertyName = QString::fromLatin1(propertyEvent->propertyName().data());

            if (propertyName == "SectionTitle")
                updateTitleLabel();

            if (propertyName == "SectionSubtitle")
                updateSubtitleLabel();

            if (propertyName == "SectionIcon")
                updateRightIcon();
        }
    }

    return QObject::eventFilter(object, event);
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

    updateLeftIcon();

    emit expandedChanged(isExpanded());
}

bool AccordionSection::isExpanded() const
{
    return _toggleButton.isChecked();
}

void AccordionSection::updateLeftIcon()
{
    const auto iconName = _toggleButton.isChecked() ? "angle-right" : "angle-down";
    const auto icon     = Application::getIconFont("FontAwesome").getIcon(iconName);

    _leftIconLabel.setPixmap(icon.pixmap(ICON_SIZE));
}

void AccordionSection::updateTitleLabel()
{
    const auto title = _widget->property("SectionTitle");

    if (!title.isValid())
        return;

    _titleLabel.setText(title.toString());
}

void AccordionSection::updateSubtitleLabel()
{
    const auto subtitle = _widget->property("SectionSubtitle");

    if (!subtitle.isValid())
        return;

    _subtitleLabel.setText(subtitle.toString());
}

void AccordionSection::updateRightIcon()
{
    const auto icon = _widget->property("SectionIcon");

    if (!icon.isValid())
        return;

    _rightIconLabel.setPixmap(icon.value<QIcon>().pixmap(ICON_SIZE));
}

}
}