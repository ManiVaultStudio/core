#include "WidgetAction.h"
#include "WidgetActionCollapsedWidget.h"
#include "Application.h"

#include <QDebug>
#include <QMenu>
#include <QEvent>
#include <QPainter>
#include <QLabel>
#include <QGroupBox>
#include <QDrag>
#include <QMimeData>
#include <QMouseEvent>

namespace hdps {

namespace gui {

WidgetAction::WidgetAction(QObject* parent) :
    QWidgetAction(parent),
    _createdBy(),
    _context(),
    _isDropTarget(false)
{
    Application::current()->getWidgetActionsManager().addAction(this);
}

QWidget* WidgetAction::createWidget(QWidget* parent)
{
    if (parent != nullptr && dynamic_cast<WidgetActionCollapsedWidget::ToolButton*>(parent->parent()))
        return getWidget(parent, WidgetActionWidget::State::Popup);

    return getWidget(parent, WidgetActionWidget::State::Standard);
}

QWidget* WidgetAction::createCollapsedWidget(QWidget* parent)
{
    return new WidgetActionCollapsedWidget(parent, this);
}

WidgetActionLabel* WidgetAction::createLabelWidget(QWidget* parent)
{
    return new WidgetActionLabel(this, parent);
}

void WidgetAction::setDropTarget(const bool& isDropTarget)
{
    if (isDropTarget == _isDropTarget)
        return;

    _isDropTarget = isDropTarget;

    isDropTargetChanged(_isDropTarget);
}

bool WidgetAction::isDropTarget() const
{
    return _isDropTarget;
}

void WidgetAction::setContext(const QString& context)
{
    _context = context;
}

QString WidgetAction::getContext() const
{
    return _context;
}

QWidget* WidgetAction::getWidget(QWidget* parent, const WidgetActionWidget::State& state /*= WidgetActionWidget::State::Standard*/)
{
    return new QWidget();
}

}
}
