#include "WidgetAction.h"
#include "WidgetActionLabel.h"
#include "WidgetActionCollapsedWidget.h"
#include "Application.h"
#include "DataHierarchyItem.h"
#include "Plugin.h"
#include "util/Exception.h"

#include <QDebug>
#include <QMenu>
#include <QFileDialog>
#include <QJsonArray>

//#define WIDGET_ACTION_VERBOSE

namespace hdps {

namespace gui {

WidgetAction::WidgetAction(QObject* parent /*= nullptr*/) :
    QWidgetAction(parent),
    _defaultWidgetFlags(),
    _sortIndex(-1),
    _isSerializing(),
    _publicAction(nullptr),
    _connectedActions()
{
}

WidgetAction::~WidgetAction()
{
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
    return nullptr;
}

std::int32_t WidgetAction::getDefaultWidgetFlags() const
{
    return _defaultWidgetFlags;
}

void WidgetAction::setDefaultWidgetFlags(const std::int32_t& widgetFlags)
{
    _defaultWidgetFlags = widgetFlags;
}

bool WidgetAction::mayPublish() const
{
    return false;
}

bool WidgetAction::isPublic() const
{
    return Application::getActionsManager().isActionPublic(this);
}

bool WidgetAction::isPublished() const
{
    return Application::getActionsManager().isActionPublished(this);
}

bool WidgetAction::isConnected() const
{
    return Application::getActionsManager().isActionConnected(this);
}

void WidgetAction::publish(const QString& name)
{
    try
    {
        if (name.isEmpty())
            throw std::runtime_error("Parameter name may not be empty");

        if (Application::getActionsManager().isActionPublished(this))
            throw std::runtime_error("Action is already published");

        auto publicCopy = getPublicCopy();

        if (publicCopy == nullptr)
            throw std::runtime_error("Public copy not created");

        publicCopy->setText(name);

        connectToPublicAction(publicCopy);

        Application::getActionsManager().addAction(publicCopy);

        emit isPublishedChanged(Application::getActionsManager().isActionPublished(this));
        emit isConnectedChanged(Application::getActionsManager().isActionConnected(this));
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

    emit isConnectedChanged(Application::getActionsManager().isActionConnected(this));
}

void WidgetAction::disconnectFromPublicAction()
{
    Q_ASSERT(_publicAction != nullptr);

    _publicAction->disconnectPrivateAction(this);

    emit isConnectedChanged(Application::getActionsManager().isActionConnected(this));
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

WidgetAction* WidgetAction::getPublicCopy() const
{
    return nullptr;
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

QString WidgetAction::getSerializationName() const
{
    return !objectName().isEmpty() ? objectName() : text();
}

bool WidgetAction::isSerializing() const
{
    return _isSerializing;
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
        if (searchString.isEmpty())
            foundChildren << childWidgetAction;
        else
            if (childWidgetAction->text().contains(searchString, Qt::CaseInsensitive))
                foundChildren << childWidgetAction;

        // Find recursively
        if (recursive)
            foundChildren << childWidgetAction->findChildren(searchString, recursive);
    }

    return foundChildren;
}

void WidgetAction::fromVariantMap(const QVariantMap& variantMap)
{
}

QVariantMap WidgetAction::toVariantMap() const
{
    return QVariantMap();
}

void WidgetAction::fromVariantMap(WidgetAction* widgetAction, const QVariantMap& variantMap)
{
#ifdef WIDGET_ACTION_VERBOSE
    qDebug().noquote() << QString("From variant map: %1").arg(widgetAction->text());
#endif

    widgetAction->fromVariantMap(variantMap);

    // Loop over all child objects and serialize each
    for (auto child : widgetAction->children()) {

        // Cast to widget action
        auto childWidgetAction = dynamic_cast<WidgetAction*>(child);

        if (!childWidgetAction)
            continue;

        childWidgetAction->fromVariantMap(variantMap[childWidgetAction->getSerializationName()].toMap());
    }
}

QVariantMap WidgetAction::toVariantMap(const WidgetAction* widgetAction)
{
#ifdef WIDGET_ACTION_VERBOSE
    qDebug().noquote() << QString("To variant map: %1").arg(widgetAction->text());
#endif

    return widgetAction->toVariantMap();
}

void WidgetAction::fromJsonDocument(const QJsonDocument& jsonDocument) const
{
    const auto variantMap = jsonDocument.toVariant().toMap();

    fromVariantMap(const_cast<WidgetAction*>(this), variantMap[getSerializationName()].toMap());
}

QJsonDocument WidgetAction::toJsonDocument() const
{
    QVariantMap variantMap;

    variantMap[getSerializationName()] = toVariantMap(this);

    return QJsonDocument::fromVariant(variantMap);
}

void WidgetAction::fromJsonFile(const QString& filePath /*= ""*/)
{
    // Exit loading prematurely if the serialization process was aborted
    if (Application::isSerializationAborted())
        return;

    try
    {
        // Except if the supplied file path is not found
        if (!QFileInfo(filePath).exists())
            throw std::runtime_error("File does not exist");

        // Create the preset file
        QFile jsonPresetFile(filePath);

        // And load the file
        if (!jsonPresetFile.open(QIODevice::ReadOnly))
            throw std::runtime_error("Unable to open file for reading");

        // Get the cluster data
        QByteArray presetData = jsonPresetFile.readAll();

        // Create JSON document
        QJsonDocument jsonDocument;

        // Convert preset raw data to JSON document
        jsonDocument = QJsonDocument::fromJson(presetData);

        // Shallow sanity check
        if (jsonDocument.isNull() || jsonDocument.isEmpty())
            throw std::runtime_error("JSON document is invalid");

        // Load the preset
        fromJsonDocument(jsonDocument);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to load data from JSON file", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to load data from JSON file");
    }
}

void WidgetAction::toJsonFile(const QString& filePath /*= ""*/)
{
    // Exit saving prematurely if the serialization process was aborted
    if (Application::isSerializationAborted())
        return;

    try
    {
        // Create the JSON file
        QFile jsonFile(filePath);

        // And open the file for writing
        if (!jsonFile.open(QFile::WriteOnly))
            throw std::runtime_error("Unable to open file for writing");

        // Create JSON document
        auto jsonDocument = toJsonDocument();

        // Shallow sanity check
        if (jsonDocument.isNull() || jsonDocument.isEmpty())
            throw std::runtime_error("JSON document is invalid");

#ifdef WIDGET_ACTION_VERBOSE
        qDebug().noquote() << jsonDocument.toJson(QJsonDocument::Indented);
#endif

        // Write the JSON document to disk
        jsonFile.write(jsonDocument.toJson());
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to save data to JSON file", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to save data to JSON file");
    }
}

void WidgetAction::setIsSerializing(bool isSerializing)
{
    _isSerializing = isSerializing;

    // Notify others that the serialization started/ended
    emit isSerializingChanged(_isSerializing);
}

QWidget* WidgetAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags)
{
    return new QWidget();
}

}
}
