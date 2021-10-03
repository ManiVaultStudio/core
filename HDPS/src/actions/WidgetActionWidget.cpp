#include "WidgetActionWidget.h"
#include "WidgetAction.h"

#include <QDebug>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QDrag>
#include <QMimeData>
#include <QMouseEvent>

namespace hdps {

namespace gui {

WidgetActionWidget::WidgetActionWidget(QWidget* parent, WidgetAction* widgetAction, const State& state) :
    QWidget(parent),
    _widgetAction(widgetAction),
    _state(state)
{
    // Update basic widget settings when the action changes
    const auto update = [this, widgetAction]() -> void {
        setEnabled(widgetAction->isEnabled());
        setVisible(widgetAction->isVisible());
        setToolTip(widgetAction->toolTip());
    };

    // When the action changes, update basic widget settings 
    connect(widgetAction, &QAction::changed, this, [this, update]() {
        update();
    });

    // Do an initial update
    update();
}

void WidgetActionWidget::setPopupLayout(QLayout* popupLayout)
{
    auto mainLayout = new QVBoxLayout();

    mainLayout->setMargin(4);

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
