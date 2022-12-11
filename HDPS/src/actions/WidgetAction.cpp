#include "WidgetAction.h"
#include "WidgetActionLabel.h"
#include "WidgetActionCollapsedWidget.h"
#include "WidgetActionContextMenu.h"
#include "Application.h"
#include "DataHierarchyItem.h"
#include "Plugin.h"
#include "util/Exception.h"
#include "AbstractActionsManager.h"

#include <QDebug>
#include <QMenu>
#include <QFileDialog>
#include <QJsonArray>
#include <QDialog>
#include <QDialogButtonBox>

#ifdef _DEBUG
    #define WIDGET_ACTION_VERBOSE
#endif

using namespace hdps::util;

namespace hdps {

namespace gui {

WidgetAction::WidgetAction(QObject* parent /*= nullptr*/) :
    QWidgetAction(parent),
    _defaultWidgetFlags(),
    _sortIndex(-1),
    _connectionPermissions(static_cast<std::int32_t>(ConnectionPermissionFlag::Default)),
    _isPublic(false),
    _publicAction(nullptr),
    _connectedActions(),
    _settingsPrefix(),
    _highlighted(false),
    _popupSizeHint(QSize(0, 0)),
    _configuration(static_cast<std::int32_t>(ConfigurationFlag::Default))
{
}

WidgetAction::~WidgetAction()
{
    Application::core()->getActionsManager().addAction(this);
}

QString WidgetAction::getTypeString() const
{
    return "";
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

bool WidgetAction::isPublic() const
{
    return _isPublic;
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
    try
    {
        if (name.isEmpty()) {
            auto& fontAwesome = Application::getIconFont("FontAwesome");

            QDialog publishDialog;

            publishDialog.setWindowIcon(fontAwesome.getIcon("cloud-upload-alt"));
            publishDialog.setWindowTitle("Publish " + text() + " parameter");

            auto mainLayout         = new QVBoxLayout();
            auto parameterLayout    = new QHBoxLayout();
            auto label              = new QLabel("Name:");
            auto lineEdit           = new QLineEdit(text());

            parameterLayout->addWidget(label);
            parameterLayout->addWidget(lineEdit);

            mainLayout->addLayout(parameterLayout);

            auto dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

            dialogButtonBox->button(QDialogButtonBox::Ok)->setText("Publish");
            dialogButtonBox->button(QDialogButtonBox::Ok)->setToolTip("Publish the parameter");
            dialogButtonBox->button(QDialogButtonBox::Cancel)->setToolTip("Cancel publishing");

            connect(dialogButtonBox->button(QDialogButtonBox::Ok), &QPushButton::clicked, &publishDialog, &QDialog::accept);
            connect(dialogButtonBox->button(QDialogButtonBox::Cancel), &QPushButton::clicked, &publishDialog, &QDialog::reject);

            mainLayout->addWidget(dialogButtonBox);

            publishDialog.setLayout(mainLayout);
            publishDialog.setFixedWidth(300);

            const auto updateOkButtonReadOnly = [dialogButtonBox, lineEdit]() -> void {
                dialogButtonBox->button(QDialogButtonBox::Ok)->setEnabled(!lineEdit->text().isEmpty());
            };

            connect(lineEdit, &QLineEdit::textChanged, this, updateOkButtonReadOnly);

            updateOkButtonReadOnly();

            if (publishDialog.exec() == QDialog::Accepted)
                publish(lineEdit->text());
        }
        else {
            if (isPublished())
                throw std::runtime_error("Action is already published");

            auto publicCopy = getPublicCopy();

            publicCopy->_isPublic = true;

            if (publicCopy == nullptr)
                throw std::runtime_error("Public copy not created");

            publicCopy->setText(name);

            connectToPublicAction(publicCopy);

            Application::core()->getActionsManager().addAction(publicCopy);

            emit isPublishedChanged(isPublished());
            emit isConnectedChanged(isConnected());
        }
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to publish " + text(), e);
    }
    catch (...)
    {
        exceptionMessageBox("Unable to publish " + text());
    }
}

void WidgetAction::connectToPublicAction(WidgetAction* publicAction)
{
    Q_ASSERT(publicAction != nullptr);

    _publicAction = publicAction;

    _publicAction->connectPrivateAction(this);

    emit isConnectedChanged(isConnected());
}

void WidgetAction::disconnectFromPublicAction()
{
    Q_ASSERT(_publicAction != nullptr);

    _publicAction->disconnectPrivateAction(this);

    emit isConnectedChanged(isConnected());
}

void WidgetAction::connectPrivateAction(WidgetAction* privateAction)
{
    Q_ASSERT(privateAction != nullptr);

    _connectedActions << privateAction;

    emit actionConnected(privateAction);

    connect(privateAction, &WidgetAction::destroyed, this, [this, privateAction]() -> void {
        disconnectPrivateAction(privateAction);
    });
}

void WidgetAction::disconnectPrivateAction(WidgetAction* privateAction)
{
    Q_ASSERT(privateAction != nullptr);

    _connectedActions.removeOne(privateAction);

    emit actionDisconnected(privateAction);
}

WidgetAction* WidgetAction::getPublicAction()
{
    return _publicAction;
}

const QVector<WidgetAction*> WidgetAction::getConnectedActions() const
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

    fromVariantMap(Application::current()->getSetting(_settingsPrefix).toMap());
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

QString WidgetAction::getSettingsPath() const
{
    QStringList actionPath;

    auto currentParent = dynamic_cast<WidgetAction*>(parent());

    const auto getPathName = [](const WidgetAction* widgetAction) -> QString {
        if (!widgetAction->objectName().isEmpty())
            return widgetAction->objectName();

        return widgetAction->text();
    };

    actionPath << getPathName(this);

    while (currentParent) {
        actionPath.insert(actionPath.begin(), getPathName(currentParent));

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

}
}
