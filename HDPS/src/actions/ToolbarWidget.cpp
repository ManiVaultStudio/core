#include "ToolbarWidget.h"
#include "ToolbarAction.h"
#include "ToolbarActionWidget.h"
#include "ToolbarSpacerWidget.h"

#include <QResizeEvent>

namespace hdps {

namespace gui {

ToolbarWidget::ToolbarWidget(QWidget* parent, ToolbarAction& toolbarAction, const Qt::Orientation& orientation) :
    QWidget(parent),
    _orientation(orientation),
    _toolbarAction(toolbarAction),
    _toolbarItemWidgets(),
    _resizeTimer(),
    _toolbarHiddenItemsAction(toolbarAction)
{
    // Set resize timer interval and only timeout once
    _resizeTimer.setInterval(RESIZE_TIMER_INTERVAL);
    _resizeTimer.setSingleShot(true);

    // Handle resize timer timeout
    connect(&_resizeTimer, &QTimer::timeout, this, [this]() {
        computeLayout();
    });

    // Listen to resize events from parent
    parent->installEventFilter(this);
}

bool ToolbarWidget::eventFilter(QObject* target, QEvent* event)
{
    switch (event->type())
    {
        case QEvent::Resize:
        {
            const auto resizeEvent = static_cast<QResizeEvent*>(event);
            
            auto layoutDirty = false;

            switch (_orientation)
            {
                case Qt::Horizontal:
                {
                    if (resizeEvent->size().width() != resizeEvent->oldSize().width())
                        layoutDirty = true;

                    break;
                }

                case Qt::Vertical:
                {
                    if (resizeEvent->size().height() != resizeEvent->oldSize().height())
                        layoutDirty = true;

                    break;
                }

                default:
                    break;
            }

            // Do nothing when the layout is not dirty
            if (!layoutDirty)
                break;

            // Prevent resize timer timeout
            if (_resizeTimer.isActive())
                _resizeTimer.stop();

            // Start resize timer
            _resizeTimer.start();

            break;
        }
    }

    return QWidget::eventFilter(target, event);
}

Qt::Orientation ToolbarWidget::getOrientation() const
{
    return _orientation;
}

bool ToolbarWidget::getEnableAnimation() const
{
    return _toolbarAction.getEnableAnimation();
}

QVector<SharedToolbarItemWidget> ToolbarWidget::getToolbarItemWidgets()
{
    return _toolbarItemWidgets;
}

QVector<ToolbarActionWidget*> ToolbarWidget::getToolbarActionWidgets()
{
    QVector<ToolbarActionWidget*> toolbarActionWidgets;

    for (const auto& toolbarItemWidget : _toolbarItemWidgets) {
        auto toolbarActionWidget = dynamic_cast<ToolbarActionWidget*>(toolbarItemWidget.get());

        if (toolbarActionWidget)
            toolbarActionWidgets.append(toolbarActionWidget);
    }

    return toolbarActionWidgets;
}

QVector<ToolbarSpacerWidget*> ToolbarWidget::getToolbarSpacerWidgets()
{
    QVector<ToolbarSpacerWidget*> toolbarSpacerWidgets;

    for (const auto& toolbarItemWidget : _toolbarItemWidgets) {
        auto toolbarActionWidget = dynamic_cast<ToolbarSpacerWidget*>(toolbarItemWidget.get());

        if (toolbarActionWidget)
            toolbarSpacerWidgets.append(toolbarActionWidget);
    }

    return toolbarSpacerWidgets;
}

}
}
