#include "WidgetAction.h"
#include "WidgetActionLabel.h"
#include "WidgetActionCollapsedWidget.h"
#include "WidgetActionContextMenu.h"
#include "Application.h"
#include "CoreInterface.h"
#include "DataHierarchyItem.h"
#include "Plugin.h"
#include "util/Exception.h"
#include "AbstractActionsManager.h"

#include <QDebug>
#include <QMenu>
#include <QFileDialog>
#include <QJsonArray>
#include <QDialog>

#ifdef _DEBUG
    #define WIDGET_ACTION_VERBOSE
#endif

using namespace hdps::util;

namespace hdps::gui {

QMap<WidgetAction::Scope, QString> WidgetAction::scopeNames {
    { WidgetAction::Scope::Private, "Private" },
    { WidgetAction::Scope::Public, "Public" }
};

WidgetAction::WidgetAction(QObject* parent, const QString& title) :
    QWidgetAction(parent),
    util::Serializable(),
    _defaultWidgetFlags(),
    _sortIndex(-1),
    _stretch(-1),
    _connectionPermissions(static_cast<std::int32_t>(ConnectionPermissionFlag::None)),
    _scope(Scope::Private),
    _publicAction(nullptr),
    _connectedActions(),
    _settingsPrefix(),
    _highlighted(false),
    _popupSizeHint(QSize(0, 0)),
    _configuration(static_cast<std::int32_t>(ConfigurationFlag::Default))
{
    setText(title);
    
    auto serializationName = title;

    serializationName.replace(" ", "");

    setSerializationName(serializationName);

    if (core()->isInitialized())
        actions().addActionToModel(this);
}

WidgetAction::~WidgetAction()
{
    if (!core()->isInitialized())
        return;

    actions().removeActionFromModel(this);
}

WidgetAction* WidgetAction::getParentWidgetAction()
{
    return dynamic_cast<WidgetAction*>(this->parent());
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

QWidget* WidgetAction::createLabelWidget(QWidget* parent, const std::int32_t& widgetFlags /*= 0x00001*/)
{
    return new WidgetActionLabel(this, parent, widgetFlags);
}

QMenu* WidgetAction::getContextMenu(QWidget* parent /*= nullptr*/)
{
    return new WidgetActionContextMenu(parent, this);
}

std::int32_t WidgetAction::getDefaultWidgetFlags() const
{
    return _defaultWidgetFlags;
}

void WidgetAction::setDefaultWidgetFlags(const std::int32_t& widgetFlags)
{
    _defaultWidgetFlags = widgetFlags;
}

void WidgetAction::setHighlighted(bool highlighted)
{
    if (highlighted == _highlighted)
        return;

    _highlighted = highlighted;

    emit highlightedChanged(_highlighted);
}

bool WidgetAction::isHighlighted() const
{
    return _highlighted;
}

WidgetAction::Scope WidgetAction::getScope() const
{
    return _scope;
}

bool WidgetAction::isPrivate() const
{
    return _scope == Scope::Private;
}

bool WidgetAction::isPublic() const
{
    return _scope == Scope::Public;
}

void WidgetAction::makePublic(bool recursive /*= true*/)
{
    _scope = Scope::Public;

    emit scopeChanged(_scope);

    for (auto child : children()) {
        auto widgetAction = dynamic_cast<WidgetAction*>(child);

        if (widgetAction != nullptr)
            widgetAction->makePublic(recursive);
    }
}

bool WidgetAction::isPublished() const
{
    if (_publicAction == nullptr)
        return false;

    for (const auto connectedAction : _publicAction->getConnectedActions())
        if (connectedAction == this)
            return true;

    return false;
}

bool WidgetAction::isConnected() const
{
    return _publicAction != nullptr;
}

void WidgetAction::publish(const QString& name /*= ""*/)
{
    hdps::actions().publishPrivateAction(this, name);
}

void WidgetAction::connectToPublicAction(WidgetAction* publicAction)
{
    Q_ASSERT(publicAction != nullptr);

    if (_publicAction == nullptr)
        return;

#ifdef WIDGET_ACTION_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    _publicAction = publicAction;

    actions().addPrivateActionToPublicAction(this, publicAction);

    const auto updateReadOnly = [this]() -> void {
        setEnabled(_publicAction->isEnabled());
    };

    updateReadOnly();

    connect(_publicAction, &QAction::changed, this, updateReadOnly);

    emit isConnectedChanged(isConnected());
}

void WidgetAction::disconnectFromPublicAction()
{
    Q_ASSERT(_publicAction != nullptr);

    if (_publicAction == nullptr)
        return;

#ifdef WIDGET_ACTION_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    actions().removePrivateActionFromPublicAction(this, _publicAction);

    disconnect(_publicAction, &QAction::changed, this, nullptr);

    setEnabled(true);

    emit isConnectedChanged(isConnected());
}

WidgetAction* WidgetAction::getPublicAction()
{
    return _publicAction;
}

const QVector<WidgetAction*> WidgetAction::getConnectedActions() const
{
    return _connectedActions;
}

QVector<WidgetAction*>& WidgetAction::getConnectedActions()
{
    return _connectedActions;
}

bool WidgetAction::mayPublish(ConnectionContextFlag connectionContextFlags) const
{
    switch (connectionContextFlags)
    {
        case WidgetAction::Api:
            return _connectionPermissions & static_cast<std::int32_t>(ConnectionPermissionFlag::PublishViaApi);

        case WidgetAction::Gui:
            return _connectionPermissions & static_cast<std::int32_t>(ConnectionPermissionFlag::PublishViaGui);

        case WidgetAction::ApiAndGui:
            break;

        default:
            break;
    }

    return false;
}

WidgetAction* WidgetAction::getPublicCopy() const
{
    return nullptr;
}

void WidgetAction::setSettingsPrefix(const QString& settingsPrefix, const bool& load /*= true*/)
{
    _settingsPrefix = settingsPrefix;

    if (load)
        loadFromSettings();
}

void WidgetAction::setSettingsPrefix(plugin::Plugin* plugin, const QString& settingsPrefix, const bool& load /*= true*/)
{
    setSettingsPrefix(QString("Plugins/%1/%2").arg(plugin->getKind(), settingsPrefix), load);
}

QString WidgetAction::getSettingsPrefix() const
{
    return _settingsPrefix;
}

void WidgetAction::loadFromSettings()
{
    if (getSettingsPrefix().isEmpty())
        return;

#ifdef WIDGET_ACTION_VERBOSE
    qDebug() << QString("Load from settings: %1").arg(getSettingsPrefix());
#endif

    const auto settingsVariant = Application::current()->getSetting(_settingsPrefix);

    if (settingsVariant.isValid())
        fromVariantMap(settingsVariant.toMap());
}

void WidgetAction::saveToSettings()
{
    if (getSettingsPrefix().isEmpty())
        return;

#ifdef WIDGET_ACTION_VERBOSE
    qDebug() << QString("Save to settings: %1").arg(getSettingsPrefix());
#endif

    Application::current()->setSetting(_settingsPrefix, toVariantMap());
}

QString WidgetAction::getPath() const
{
    QStringList actionPath;

    auto currentParent = dynamic_cast<WidgetAction*>(parent());

    actionPath << text();

    while (currentParent) {
        actionPath.insert(actionPath.begin(), currentParent->text());

        currentParent = dynamic_cast<WidgetAction*>(currentParent->parent());
    }

    return actionPath.join("/");
}

QVector<WidgetAction*> WidgetAction::findChildren(const QString& searchString, bool recursive /*= true*/) const
{
    QVector<WidgetAction*> foundChildren;

    for (auto child : children()) {
        auto childWidgetAction = dynamic_cast<WidgetAction*>(child);

        if (!childWidgetAction)
            continue;

        if (searchString.isEmpty())
            foundChildren << childWidgetAction;
        else
            if (childWidgetAction->text().contains(searchString, Qt::CaseInsensitive))
                foundChildren << childWidgetAction;

        if (recursive)
            foundChildren << childWidgetAction->findChildren(searchString, recursive);
    }

    return foundChildren;
}

QSize WidgetAction::getPopupSizeHint() const
{
    return _popupSizeHint;
}

void WidgetAction::setPopupSizeHint(const QSize& popupSizeHint)
{
    _popupSizeHint = popupSizeHint;
}

QWidget* WidgetAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags)
{
    return new QWidget();
}

void WidgetAction::fromVariantMap(const QVariantMap& variantMap)
{
    Serializable::fromVariantMap(variantMap);

    variantMapMustContain(variantMap, "Enabled");
    variantMapMustContain(variantMap, "Checked");
    variantMapMustContain(variantMap, "Visible");
    variantMapMustContain(variantMap, "SortIndex");
    variantMapMustContain(variantMap, "ConnectionPermissions");

    setEnabled(variantMap["Enabled"].toBool());
    setChecked(variantMap["Checked"].toBool());
    setVisible(variantMap["Visible"].toBool());
    setSortIndex(variantMap["SortIndex"].toInt());
    setConnectionPermissions(variantMap["ConnectionPermissions"].toInt());

    /*
    if (variantMap.contains("PublicActionID")) {
        const auto publicActionId   = variantMap["PublicActionID"].toString();
        const auto publicAction     = actions().getAction(publicActionId);

        if (publicAction)
            connectToPublicAction(publicAction);
    }
    */
}

QVariantMap WidgetAction::toVariantMap() const
{
    QVariantMap variantMap = Serializable::toVariantMap();

    variantMap.insert({
        { "Type", QVariant::fromValue(getTypeString()) },
        { "Enabled", QVariant::fromValue(isEnabled()) },
        { "Checked", QVariant::fromValue(isChecked()) },
        { "Visible", QVariant::fromValue(isVisible()) },
        { "SortIndex", QVariant::fromValue(_sortIndex) },
        { "ConnectionPermissions", QVariant::fromValue(_connectionPermissions) },
        { "PublicActionID", QVariant::fromValue(_publicAction == nullptr ? "" : _publicAction->getId()) }
    });

    

    return variantMap;
}

QVector<WidgetAction*> WidgetAction::getChildActions()
{
    QVector<WidgetAction*> childActions;

    for (auto child : children()) {
        auto childAction = dynamic_cast<WidgetAction*>(child);

        if (childAction)
            childActions << childAction;
    }

    return childActions;
}

bool WidgetAction::mayConnect(ConnectionContextFlag connectionContextFlags) const
{
    switch (connectionContextFlags)
    {
        case WidgetAction::Api:
            return _connectionPermissions & static_cast<std::int32_t>(ConnectionPermissionFlag::ConnectViaApi);

        case WidgetAction::Gui:
            return _connectionPermissions & static_cast<std::int32_t>(ConnectionPermissionFlag::ConnectViaGui);

        case WidgetAction::ApiAndGui:
            break;

        default:
            break;
    }

    return false;
}

bool WidgetAction::mayDisconnect(ConnectionContextFlag connectionContextFlags) const
{
    switch (connectionContextFlags)
    {
        case hdps::gui::WidgetAction::Api:
            return _connectionPermissions & static_cast<std::int32_t>(ConnectionPermissionFlag::DisconnectViaApi);

        case hdps::gui::WidgetAction::Gui:
            return _connectionPermissions & static_cast<std::int32_t>(ConnectionPermissionFlag::DisconnectViaGui);

        case hdps::gui::WidgetAction::ApiAndGui:
            break;

        default:
            break;
    }

    return false;
}

std::int32_t WidgetAction::getConnectionPermissions() const
{
    return _connectionPermissions;
}

bool WidgetAction::isConnectionPermissionFlagSet(ConnectionPermissionFlag connectionPermissionsFlag)
{
    return _connectionPermissions & static_cast<std::int32_t>(connectionPermissionsFlag);
}

void WidgetAction::setConnectionPermissionsFlag(ConnectionPermissionFlag connectionPermissionsFlag, bool unset /*= false*/, bool recursive /*= false*/)
{
    if (unset)
        _connectionPermissions = _connectionPermissions & ~static_cast<std::int32_t>(connectionPermissionsFlag);
    else
        _connectionPermissions |= static_cast<std::int32_t>(connectionPermissionsFlag);

    emit connectionPermissionsChanged(_connectionPermissions);

    if (recursive) {
        for (auto childAction : getChildActions())
            childAction->setConnectionPermissionsFlag(connectionPermissionsFlag, unset, recursive);
    }
}

void WidgetAction::setConnectionPermissions(std::int32_t connectionPermissions, bool recursive /*= false*/)
{
    _connectionPermissions = connectionPermissions;

    emit connectionPermissionsChanged(_connectionPermissions);

    if (recursive) {
        for (auto childAction : getChildActions())
            childAction->setConnectionPermissions(connectionPermissions, recursive);
    }
}

void WidgetAction::setConnectionPermissionsToNone(bool recursive /*= false*/)
{
    setConnectionPermissions(static_cast<std::int32_t>(ConnectionPermissionFlag::None), recursive);
}

void WidgetAction::setConnectionPermissionsToAll(bool recursive /*= false*/)
{
    setConnectionPermissions(static_cast<std::int32_t>(ConnectionPermissionFlag::All), recursive);
}

bool WidgetAction::isResettable()
{
    return false;
}

std::int32_t WidgetAction::getConfiguration() const
{
    return _configuration;
}

bool WidgetAction::isConfigurationFlagSet(ConfigurationFlag configurationFlag)
{
    return _configuration & static_cast<std::int32_t>(configurationFlag);
}

void WidgetAction::setConfigurationFlag(ConfigurationFlag configurationFlag, bool unset /*= false*/, bool recursive /*= false*/)
{
    if (unset)
        _configuration = _configuration & ~static_cast<std::int32_t>(configurationFlag);
    else
        _configuration |= static_cast<std::int32_t>(configurationFlag);

    emit configurationChanged(_configuration);

    if (recursive) {
        for (auto childAction : getChildActions())
            childAction->setConfigurationFlag(configurationFlag, unset, recursive);
    }
}

void WidgetAction::setConfiguration(std::int32_t configuration, bool recursive /*= false*/)
{
    _configuration = configuration;

    emit configurationChanged(_configuration);

    if (recursive) {
        for (auto childAction : getChildActions())
            childAction->setConfiguration(configuration, recursive);
    }
}

QString WidgetAction::getTypeString(bool humanFriendly /*= false*/) const
{
    const auto className = QString(metaObject()->className());

    if (humanFriendly)
        return className.split("::").last().replace("Action", "");
    
    return className;
}

std::int32_t WidgetAction::getStretch() const
{
    return _stretch;
}

void WidgetAction::setStretch(const std::int32_t& stretch)
{
    _stretch = stretch;
}

WidgetAction* WidgetAction::_getPublicCopy() const
{
    auto actionCopy = static_cast<WidgetAction*>(metaObject()->newInstance(Q_ARG(QObject*, parent()), Q_ARG(QString, text())));

    actionCopy->fromVariantMap(toVariantMap());
    actionCopy->makePublic();

    return actionCopy;

}

}
