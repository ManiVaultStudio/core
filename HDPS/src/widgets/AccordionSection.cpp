#include "AccordionSection.h"
#include "DockableWidget.h"
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
    _dockableWidget(nullptr)
{
    _mainLayout.setMargin(0);
    _mainLayout.setContentsMargins(0, 0, 0, 0);
    _mainLayout.setSpacing(0);
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
    Q_ASSERT(_dockableWidget != nullptr);

    return _dockableWidget->objectName();
}

hdps::gui::DockableWidget* AccordionSection::getDockableWidget()
{
    return _dockableWidget;
}

void AccordionSection::setDockableWidget(DockableWidget* dockableWidget)
{
    Q_ASSERT(dockableWidget != nullptr);

    _dockableWidget = dockableWidget;

    _contentLayout.addWidget(dockableWidget);

    updateTitleLabel();
    updateRightIcon();
    updateToggleButtonTooltip();

    QObject::connect(dockableWidget, &QWidget::objectNameChanged, [this](const QString& objectName) {
        updateTitleLabel();
    });

    QObject::connect(dockableWidget, &QWidget::windowIconChanged, [this](const QIcon& icon) {
        updateRightIcon();
    });

    dockableWidget->installEventFilter(this);
}

bool AccordionSection::eventFilter(QObject* object, QEvent* event)
{
    auto widget = dynamic_cast<QWidget*>(object);

    if (widget == nullptr)
        return QObject::eventFilter(object, event);

    if(widget == _dockableWidget && event->type() == QEvent::DynamicPropertyChange) {
        
        const auto propertyEvent = static_cast<QDynamicPropertyChangeEvent*>(event);

        if (propertyEvent) {
            const auto propertyName = QString::fromLatin1(propertyEvent->propertyName().data());

            if (propertyName == DockableWidget::TITLE_PROPERTY_NAME)
                updateTitleLabel();

            if (propertyName == DockableWidget::SUBTITLE_PROPERTY_NAME)
                updateSubtitleLabel();

            if (propertyName == DockableWidget::ICON_PROPERTY_NAME)
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

    _dockableWidget->setVisible(expanded);

    updateLeftIcon();
    updateToggleButtonTooltip();

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
    _titleLabel.setText(_dockableWidget->getTitle());
}

void AccordionSection::updateSubtitleLabel()
{
    _subtitleLabel.setText(_dockableWidget->getSubtitle());
}

void AccordionSection::updateRightIcon()
{
    _rightIconLabel.setPixmap(_dockableWidget->getIcon().pixmap(ICON_SIZE));
}

void AccordionSection::updateToggleButtonTooltip()
{
    Q_ASSERT(_dockableWidget != nullptr);

    const auto expandCollapse   = _toggleButton.isChecked() ? "collapse" : "expand";
    const auto title            = _dockableWidget->property(qPrintable(DockableWidget::TITLE_PROPERTY_NAME));

    _toggleButton.setToolTip(QString("Click to %1 %2").arg(expandCollapse, _dockableWidget->objectName()));
}

}
}