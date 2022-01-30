#include "WidgetAction.h"
#include "WidgetActionLabel.h"
#include "WidgetActionCollapsedWidget.h"
#include "DataHierarchyItem.h"
#include "Application.h"

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
    _settingsPrefix()
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

QString WidgetAction::getSettingsPrefix() const
{
    return _settingsPrefix;
}

void WidgetAction::setSettingsPrefix(const QString& settingsPrefix)
{
    Q_ASSERT(!settingsPrefix.isEmpty());

    // No need in update the same prefix
    if (settingsPrefix == _settingsPrefix)
        return;

    // Assign settings prefix
    _settingsPrefix = settingsPrefix;

    // Load settings from registry
    loadDefault();
}

void WidgetAction::setSettingsPrefix(const QString& settingsPrefix, const plugin::Plugin* plugin)
{
    Q_ASSERT(!settingsPrefix.isEmpty());
    Q_ASSERT(plugin != nullptr);

    // Settings prefix consists of the plugin kind + settings prefix
    _settingsPrefix = QString("%1/%2").arg(plugin->getKind(), settingsPrefix);

    loadDefault();
}

bool WidgetAction::hasSavedDefault() const
{
    return Application::current()->getSetting(getSettingsPrefix() + "/Default").isValid();
}

void WidgetAction::loadDefault()
{
    setValue(Application::current()->getSetting(getSettingsPrefix() + "/Default"));
}

void WidgetAction::saveDefault()
{
    Application::current()->setSetting(getSettingsPrefix() + "/Default", valueToVariant());
}

QWidget* WidgetAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags)
{
    return new QWidget();
}

}
}
