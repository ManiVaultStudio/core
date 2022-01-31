#include "WidgetActionWidget.h"
#include "WidgetAction.h"

#include <QDebug>
#include <QVBoxLayout>
#include <QGroupBox>

namespace hdps {

namespace gui {

WidgetActionWidget::WidgetActionWidget(QWidget* parent, WidgetAction* widgetAction, const std::int32_t& widgetFlags /*= 0*/) :
    QWidget(parent),
    _widgetAction(widgetAction),
    _widgetFlags(widgetFlags)
{
    // Update basic widget settings when the action changes
    const auto update = [this]() -> void {
        setEnabled(_widgetAction->isEnabled());
        setToolTip(_widgetAction->toolTip());
        setVisible(_widgetAction->isVisible());
    };

    // When the action changes, update basic widget settings 
    connect(widgetAction, &QAction::changed, this, update);

    // Do an initial update
    update();
}

void WidgetActionWidget::setPopupLayout(QLayout* popupLayout)
{
    auto mainLayout = new QVBoxLayout();

    mainLayout->setContentsMargins(4,4,4,4);

    setLayout(mainLayout);

    auto groupBox = new QGroupBox(_widgetAction->text());

    groupBox->setLayout(popupLayout);

    mainLayout->addWidget(groupBox);

    const auto update = [this, groupBox]() -> void {
        groupBox->setTitle(_widgetAction->text());
        groupBox->setToolTip(_widgetAction->text());
    };

    connect(_widgetAction, &WidgetAction::changed, this, [update]() {
        update();
    });

    update();
}

}
}
