#include "WidgetAction.h"
#include "WidgetActionCollapsedWidget.h"
#include "DataHierarchyItem.h"
#include "Application.h"

#include <QDebug>

namespace hdps {

namespace gui {

WidgetAction::WidgetAction(QObject* parent) :
    QWidgetAction(parent),
    _createdBy(),
    _context(),
    _dataHierarchyItemContext(nullptr),
    _isDropTarget(false),
    _widgetFlags(),
    _resettable(false)
{
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

WidgetActionResetButton* WidgetAction::createResetButton(QWidget* parent)
{
    return new WidgetActionResetButton(this, parent);
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

bool WidgetAction::isResettable() const
{
    return _resettable;
}

void WidgetAction::setResettable(const bool& resettable)
{
    if (resettable == _resettable)
        return;

    _resettable = resettable;

    emit resettableChanged(_resettable);
}

void WidgetAction::reset()
{
    qDebug() << text() << "Does not implement a reset function";
}

bool WidgetAction::hasWidgetFlag(const std::int32_t& widgetFlag) const
{
    return _widgetFlags & widgetFlag;
}

void WidgetAction::setWidgetFlags(const std::int32_t& widgetFlags)
{
    _widgetFlags = widgetFlags;
}

void WidgetAction::setContext(const QString& context)
{
    _context = context;
}

void WidgetAction::setContext(const DataHierarchyItem* dataHierarchyItem)
{
    if (dataHierarchyItem == _dataHierarchyItemContext)
        return;

    _dataHierarchyItemContext = dataHierarchyItem;
}

QString WidgetAction::getContext() const
{
    if (_dataHierarchyItemContext != nullptr)
        return _dataHierarchyItemContext->getDatasetName();

    return _context;
}

QWidget* WidgetAction::getWidget(QWidget* parent, const WidgetActionWidget::State& state /*= WidgetActionWidget::State::Standard*/)
{
    return new QWidget();
}

}
}
