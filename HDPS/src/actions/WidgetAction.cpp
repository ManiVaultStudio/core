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
    _defaultWidgetFlags(),
    _resettable(false),
    _mayReset(false),
    _sortIndex(-1),
    _serializable(true)
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

    // Determine whether any of the children is resettable
    const auto anyChildResettable = [](const WidgetAction* widgetAction) -> bool {
        for (auto child : widgetAction->children()) {
            auto childWidgetAction = dynamic_cast<WidgetAction*>(child);

            if (!childWidgetAction)
                continue;

            if (!childWidgetAction->isSerializable())
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
    // Determine whether any of the children is factory resettable
    const auto anyChildFactoryResettable = [](const WidgetAction* widgetAction) -> bool {
        for (auto child : widgetAction->children()) {
            auto childWidgetAction = dynamic_cast<WidgetAction*>(child);

            if (!childWidgetAction)
                continue;

            if (!childWidgetAction->isSerializable())
                continue;

            if (childWidgetAction->isFactoryResettable())
                return true;
        }

        return false;
    };

    if (anyChildFactoryResettable(this))
        return true;

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
        setValueFromVariant(defaultValue);

    // Reset all children
    for (auto child : children()) {
        auto childWidgetAction = dynamic_cast<WidgetAction*>(child);

        if (!childWidgetAction)
            continue;

        if (!childWidgetAction->isSerializable())
            continue;

        // Reset the child
        childWidgetAction->reset();
    }
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
    if (_serializable)
        return true;

    // Check whether any of the children is serializable
    for (auto child : children()) {
        auto childWidgetAction = dynamic_cast<WidgetAction*>(child);

        if (!childWidgetAction)
            continue;

        if (childWidgetAction->isSerializable())
            return true;
    }

    return false;
}

void WidgetAction::setSerializable(const bool& serializable, bool recursive /*= true*/)
{
    // No need in update the same prefix
    if (serializable == _serializable)
        return;

    _serializable = serializable;

    if (!recursive)
        return;

    // Set children serializable
    for (auto child : children()) {
        auto childWidgetAction = dynamic_cast<WidgetAction*>(child);

        if (!childWidgetAction)
            continue;

        childWidgetAction->setSerializable(serializable);
    }
}

bool WidgetAction::hasSavedDefault() const
{
    if (Application::current()->getSetting(getSettingsPath() + "/Default").isValid())
        return true;

    // Check whether any of the children has a saved default
    for (auto child : children()) {
        auto childWidgetAction = dynamic_cast<WidgetAction*>(child);

        if (!childWidgetAction)
            continue;

        if (!childWidgetAction->isSerializable())
            continue;

        // Load child default
        if (childWidgetAction->hasSavedDefault())
            return true;
    }

    return false;
}

bool WidgetAction::canSaveDefault() const
{
    const auto valueVariant             = valueToVariant();
    const auto defaultValueVariant      = defaultValueToVariant();
    const auto savedDefaultValueVariant = savedDefaultValueToVariant();

    if (valueVariant.isValid()) {
        if (savedDefaultValueVariant.isValid() && (valueVariant != savedDefaultValueVariant))
            return true;

        if (defaultValueVariant.isValid() && (valueVariant != savedDefaultValueVariant && valueVariant != defaultValueVariant))
            return true;
    }

    // Check whether any of the children can save to default
    for (auto child : children()) {
        auto childWidgetAction = dynamic_cast<WidgetAction*>(child);

        if (!childWidgetAction)
            continue;

        if (!childWidgetAction->isSerializable())
            continue;

        // Check whether child can save to default
        if (childWidgetAction->canSaveDefault())
            return true;
    }

    return false;
}

void WidgetAction::loadDefault(bool recursive /*= true*/)
{
    //qDebug() << "Load default for " << text();

    // Load own default
    setValueFromVariant(Application::current()->getSetting(getSettingsPath() + "/Default"));

    if (!recursive)
        return;

    // Load default for all children
    for (auto child : children()) {
        auto childWidgetAction = dynamic_cast<WidgetAction*>(child);

        if (!childWidgetAction)
            continue;

        if (!childWidgetAction->isSerializable())
            continue;

        // Load child default
        childWidgetAction->loadDefault();
    }
}

void WidgetAction::saveDefault(bool recursive /*= true*/)
{
    //qDebug() << "Save default for " << text();

    // Save own default
    Application::current()->setSetting(getSettingsPath() + "/Default", valueToVariant());

    if (!recursive)
        return;

    // Save default for all children
    for (auto child : children()) {
        auto childWidgetAction = dynamic_cast<WidgetAction*>(child);

        if (!childWidgetAction)
            continue;

        if (!childWidgetAction->isSerializable())
            continue;

        // Save child default
        childWidgetAction->saveDefault();
    }
}

QString WidgetAction::getSettingsPath() const
{
    QStringList actionPath;

    // Get the first action parent
    auto currentParent = dynamic_cast<WidgetAction*>(parent());

    // Get path name from widget action
    const auto getPathName = [](const WidgetAction* widgetAction) -> QString {
        if (!widgetAction->objectName().isEmpty())
            return widgetAction->objectName();

        return widgetAction->text();
    };

    // Add our own title
    actionPath << getPathName(this);

    // Walk up the action tree
    while (currentParent)
    {
        // Insert the action text at the beginning
        actionPath.insert(actionPath.begin(), getPathName(currentParent));

        // Get the next parent action
        currentParent = dynamic_cast<WidgetAction*>(currentParent->parent());
    }

    return actionPath.join("/");
}

QVector<WidgetAction*> WidgetAction::findChildren(const QString& searchString, bool recursive /*= true*/) const
{
    QVector<WidgetAction*> foundChildren;

    // Loop over all children
    for (auto child : children()) {

        // Cast to widget action
        auto childWidgetAction = dynamic_cast<WidgetAction*>(child);

        if (!childWidgetAction)
            continue;

        // Add child widget action if the name (text) contains the search string
        if (childWidgetAction->text().contains(searchString, Qt::CaseInsensitive))
            foundChildren << childWidgetAction;

        // Find recursively
        if (recursive)
            foundChildren << childWidgetAction->findChildren(searchString, recursive);
    }

    return foundChildren;
}

QWidget* WidgetAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags)
{
    return new QWidget();
}

}
}
