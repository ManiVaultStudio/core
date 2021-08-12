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
    const auto updateAction = [this, widgetAction]() -> void {
        setEnabled(widgetAction->isEnabled());
        setVisible(widgetAction->isVisible());
    };

    connect(widgetAction, &QAction::changed, this, [this, updateAction]() {
        updateAction();
    });

    updateAction();
}

void WidgetActionWidget::dragEnterEvent(QDragEnterEvent* dragEnterEvent)
{
    auto mimeData = dragEnterEvent->mimeData();

    if (!mimeData->hasFormat("text/plain"))
        return;

    qDebug() << mimeData->text() << typeid(*_widgetAction).name();

    if (mimeData->text() == typeid(*_widgetAction).name())
        dragEnterEvent->acceptProposedAction();
}

void WidgetActionWidget::dropEvent(QDropEvent* dropEvent)
{
    qDebug() << dropEvent->mimeData()->text();

    dropEvent->acceptProposedAction();
}

void WidgetActionWidget::setPopupLayout(QLayout* popupLayout)
{
    auto mainLayout = new QVBoxLayout();

    mainLayout->setMargin(4);

    setLayout(mainLayout);

    auto groupBox = new QGroupBox(_widgetAction->text());

    groupBox->setLayout(popupLayout);

    mainLayout->addWidget(groupBox);
}

}
}
