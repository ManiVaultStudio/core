#include "WidgetAction.h"
#include "WidgetActionLabel.h"
#include "WidgetActionCollapsedWidget.h"

#include <DataHierarchyItem.h>
#include <Application.h>
#include <Plugin.h>

#include <QDebug>

namespace hdps {

namespace gui {

WidgetAction::WidgetAction(QObject* parent) :
    QWidgetAction(parent),
    _dataHierarchyItemContext(nullptr),
    _defaultWidgetFlags(),
    _resettable(false),
    _mayReset(false),
    _sortIndex(-1),
    _serializable(false)
{
}

QWidget* WidgetAction::createWidget(QWidget* parent)
{
    if (parent != nullptr && dynamic_cast<WidgetActionCollapsedWidget::ToolButton*>(parent->parent()))
        return getWidget(parent, _defaultWidgetFlags | WidgetActionWidget::PopupLayout);

    return getWidget(parent, _defaultWidgetFlags);
}

QWidget* WidgetAction::createWidget(QWidget* parent, const std::int32_t& widgetFlags)
{
    return getWidget(parent, widgetFlags);
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

QWidget* WidgetAction::createLabelWidget(QWidget* parent)
{
    return new WidgetActionLabel(this, parent);
}

bool WidgetAction::isResettable() const
{
    if (!hasSavedDefault())
        return false;

    const auto anyChildResettable = [](const WidgetAction* widgetAction) -> bool {
        for (auto child : widgetAction->children()) {
            auto childWidgetAction = dynamic_cast<WidgetAction*>(child);

            if (!childWidgetAction)
                continue;

            if (childWidgetAction->isResettable())
                return true;
        }

        return false;
    };

    if (anyChildResettable(this))
        return true;

    return valueToVariant() != savedDefaultValueToVariant();
}

bool WidgetAction::isFactoryResettable() const
{
    return valueToVariant() != defaultValueToVariant();
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
    // Get default value as variant
    const auto defaultValue = defaultValueToVariant();

    // And assign it to the value if valid
    if (defaultValue.isValid())
        setValue(defaultValue);
}

std::int32_t WidgetAction::getDefaultWidgetFlags() const
{
    return _defaultWidgetFlags;
}

void WidgetAction::setDefaultWidgetFlags(const std::int32_t& widgetFlags)
{
    _defaultWidgetFlags = widgetFlags;
}

bool WidgetAction::isSerializable() const
{
    return _serializable;
}

void WidgetAction::setSerializable(const bool& serializable)
{
    // No need in update the same prefix
    if (serializable == _serializable)
        return;

    // Load settings from registry
    loadDefault();
}

bool WidgetAction::hasSavedDefault() const
{
    return Application::current()->getSetting(getSettingsPath() + "/Default").isValid();
}

void WidgetAction::loadDefault()
{
    setValue(Application::current()->getSetting(getSettingsPath() + "/Default"));
}

void WidgetAction::saveDefault()
{
    Application::current()->setSetting(getSettingsPath() + "/Default", valueToVariant());
}

QString WidgetAction::getSettingsPath() const
{
    QStringList actionPath;

    // Get the first action parent
    auto currentParent = dynamic_cast<WidgetAction*>(parent());

    // Add our own title
    actionPath << text();

    // Walk up the action tree
    while (currentParent)
    {
        // Insert the action text at the beginning
        actionPath.insert(actionPath.begin(), currentParent->text());

        // Get the next parent action
        currentParent = dynamic_cast<WidgetAction*>(currentParent->parent());
    }

    /*
    if (!currentParent) {
        auto parentPlugin = dynamic_cast<hdps::plugin::Plugin*>(currentParent);

        if (parentPlugin)
            actionPath.insert(actionPath.begin(), parentPlugin->getKind());
    }
    */  

    return actionPath.join("/");
}

QWidget* WidgetAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags)
{
    return new QWidget();
}

}
}
