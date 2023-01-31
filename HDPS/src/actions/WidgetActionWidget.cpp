#include "WidgetActionWidget.h"
#include "WidgetAction.h"
#include "WidgetActionCollapsedWidget.h"
#include "widgets/OverlayWidget.h"

#include <QDebug>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QStyleOption>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QResizeEvent>
#include <QLabel>

namespace hdps {

namespace gui {

WidgetActionWidget::WidgetActionWidget(QWidget* parent, WidgetAction* widgetAction, const std::int32_t& widgetFlags /*= 0*/) :
    QWidget(parent),
    _widgetAction(widgetAction),
    _widgetFlags(widgetFlags),
    _highlightWidget(nullptr)
{
    setWidgetAction(widgetAction);
}

WidgetAction* WidgetActionWidget::getWidgetAction()
{
    return _widgetAction;
}

void WidgetActionWidget::setWidgetAction(WidgetAction* widgetAction)
{
    if (_widgetAction != nullptr) {
        disconnect(_widgetAction, &WidgetAction::changed, this, nullptr);
        disconnect(_widgetAction, &WidgetAction::highlightedChanged, this, nullptr);
    }

    _widgetAction = widgetAction;

    if (_widgetAction == nullptr)
        return;

    const auto update = [this]() -> void {
        setEnabled(_widgetAction->isEnabled());
        setToolTip(_widgetAction->toolTip());
        setVisible(_widgetAction->isVisible());
    };

    connect(_widgetAction, &WidgetAction::changed, this, update);

    update();

    const auto updateHighlighted = [this]() -> void {
        if (_widgetAction->isHighlighted()) {
            if (_highlightWidget == nullptr)
                _highlightWidget = new OverlayWidget(this);

            _highlightWidget->show();
        }
        else {
            if (_highlightWidget)
                _highlightWidget->hide();
        }
    };

    connect(_widgetAction, &WidgetAction::highlightedChanged, this, updateHighlighted);

    updateHighlighted();
}

QSize WidgetActionWidget::sizeHint() const
{
    if (_widgetFlags & WidgetFlag::PopupLayout)
        return _widgetAction->getPopupSizeHint();

    return QWidget::sizeHint();
}

void WidgetActionWidget::setPopupLayout(QLayout* popupLayout)
{
    auto mainLayout = new QVBoxLayout();

    mainLayout->setContentsMargins(4, 4, 4, 4);

    setLayout(mainLayout);

    auto groupBox = new QGroupBox(_widgetAction->text());

    groupBox->setLayout(popupLayout);
    groupBox->setCheckable(_widgetAction->isCheckable());

    mainLayout->addWidget(groupBox);

    const auto update = [this, groupBox]() -> void {
        QSignalBlocker blocker(_widgetAction);

        groupBox->setTitle(_widgetAction->text());
        groupBox->setToolTip(_widgetAction->text());
        groupBox->setChecked(_widgetAction->isChecked());
    };

    connect(groupBox, &QGroupBox::toggled, this, [this](bool toggled) {
        _widgetAction->setChecked(toggled);
    });

    connect(_widgetAction, &WidgetAction::changed, this, [update]() {
        update();
    });

    update();
}

}
}
