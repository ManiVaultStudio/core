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
    _defaultWidgetFlags(),
    _resettable(false),
    _mayReset(false),
    _sortIndex(-1)
{
}

QWidget* WidgetAction::createWidget(QWidget* parent)
{
    if (parent != nullptr && dynamic_cast<WidgetActionCollapsedWidget::ToolButton*>(parent->parent()))
        return getWidget(parent, _defaultWidgetFlags, WidgetActionWidget::State::Popup);

    return getWidget(parent, _defaultWidgetFlags, WidgetActionWidget::State::Standard);
}

QWidget* WidgetAction::createWidget(QWidget* parent, const std::int32_t& widgetFlags)
{
    return getWidget(parent, widgetFlags, WidgetActionWidget::State::Standard);
}

bool WidgetAction::getMayReset() const
{
    return _mayReset;
}

void WidgetAction::setMayReset(const bool& mayReset)
{
    _mayReset = mayReset;
}

std::int32_t WidgetAction::getSortIndex() const
{
    return _sortIndex;
}

void WidgetAction::setSortIndex(const std::int32_t& sortIndex)
{
    _sortIndex = sortIndex;
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

void WidgetAction::setDefaultWidgetFlags(const std::int32_t& widgetFlags)
{
    _defaultWidgetFlags = widgetFlags;
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

QWidget* WidgetAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags, const WidgetActionWidget::State& state /*= WidgetActionWidget::State::Standard*/)
{
    return new QWidget();
}

}
}
