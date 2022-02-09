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

namespace hdps {

namespace gui {

WidgetAction::WidgetAction(QObject* parent) :
    QWidgetAction(parent),
    _defaultWidgetFlags(),
    _sortIndex(-1),
    _serializable(true),
    _isSerializing()
{
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

QWidget* WidgetAction::createLabelWidget(QWidget* parent)
{
    return new WidgetActionLabel(this, parent);
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
    if (isSerializable() && Application::current()->getSetting(getSettingsPath() + "/Default").isValid())
        return true;

    // Check whether any of the children has a saved default
    for (auto child : children()) {
        auto childWidgetAction = dynamic_cast<WidgetAction*>(child);

        if (!childWidgetAction)
            continue;

        // Load child default
        if (childWidgetAction->hasSavedDefault())
            return true;
    }

    return false;
}

bool WidgetAction::canSaveDefault(bool recursive /*= true*/) const
{
    if (isSerializable()) {
        const auto valueVariant             = valueToVariant();
        const auto defaultValueVariant      = defaultValueToVariant();
        const auto savedDefaultValueVariant = savedDefaultValueToVariant();

        if (valueVariant.isValid()) {
            if (savedDefaultValueVariant.isValid() && (valueVariant != savedDefaultValueVariant))
                return true;

            if (defaultValueVariant.isValid() && (valueVariant != savedDefaultValueVariant && valueVariant != defaultValueVariant))
                return true;
        }
    }

    if (!recursive)
        return false;

    // Check whether any of the children can save to default
    for (auto child : children()) {
        auto childWidgetAction = dynamic_cast<WidgetAction*>(child);

        if (!childWidgetAction)
            continue;

        // Check whether child can save to default
        if (childWidgetAction->canSaveDefault())
            return true;
    }

    return false;
}

void WidgetAction::loadDefault(bool recursive /*= true*/)
{
#ifdef _DEBUG
    qDebug().noquote() << QString("Load default for: %1").arg(getSettingsPath());
#endif

    setIsSerializing(true);
    {
        if (isSerializable())
            setValueFromVariant(Application::current()->getSetting(getSettingsPath() + "/Default"));

        if (!recursive)
            return;

        // Load default for all children
        for (auto child : children()) {
            auto childWidgetAction = dynamic_cast<WidgetAction*>(child);

            if (!childWidgetAction)
                continue;

            // Load child default
            childWidgetAction->loadDefault();
        }
    }
    setIsSerializing(false);
}

void WidgetAction::saveDefault(bool recursive /*= true*/)
{
#ifdef _DEBUG
    qDebug().noquote() << QString("Save default for: %1").arg(getSettingsPath());
#endif

    setIsSerializing(true);
    {
        if (isSerializable())
            Application::current()->setSetting(getSettingsPath() + "/Default", valueToVariant());

        if (!recursive)
            return;

        // Save default for all children
        for (auto child : children()) {
            auto childWidgetAction = dynamic_cast<WidgetAction*>(child);

            if (!childWidgetAction)
                continue;

            // Save child default
            childWidgetAction->saveDefault();
        }
    }
    setIsSerializing(false);
}

bool WidgetAction::isResettable(bool recursive /*= true*/) const
{
    if (isSerializable() && hasSavedDefault() && (valueToVariant() != savedDefaultValueToVariant()))
        return true;

    if (!recursive)
        return false;

    // Determine whether any of the children is resettable
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

    return false;
}

bool WidgetAction::isFactoryResettable(bool recursive /*= true*/) const
{
    if (isSerializable() && (valueToVariant() != defaultValueToVariant()))
        return true;

    if (!recursive)
        return false;

    // Determine whether any of the children is factory resettable
    const auto anyChildFactoryResettable = [](const WidgetAction* widgetAction) -> bool {
        for (auto child : widgetAction->children()) {
            auto childWidgetAction = dynamic_cast<WidgetAction*>(child);

            if (!childWidgetAction)
                continue;

            if (childWidgetAction->isFactoryResettable())
                return true;
        }

        return false;
    };

    if (anyChildFactoryResettable(this))
        return true;

    return false;
}

void WidgetAction::notifyResettable()
{
#ifdef _DEBUG
    qDebug().noquote() << QString("Notify resettable: %1").arg(getSettingsPath());
#endif

    return;

    if (isSerializing())
        return;

    emit resettableChanged(isResettable());
    emit factoryResettableChanged(isFactoryResettable());

    const auto widgetActionParent = dynamic_cast<WidgetAction*>(parent());

    if (widgetActionParent)
        widgetActionParent->notifyResettable();
}

void WidgetAction::reset(bool recursive /*= true*/)
{
    // Get default value as variant
    const auto defaultValue = defaultValueToVariant();

    // And assign it to the value if valid
    if (isSerializable() && defaultValue.isValid())
        setValueFromVariant(defaultValue);

    if (!recursive)
        return;

    // Reset all children
    for (auto child : children()) {
        auto childWidgetAction = dynamic_cast<WidgetAction*>(child);

        if (!childWidgetAction)
            continue;

        // Reset the child
        childWidgetAction->reset();
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

void WidgetAction::fromVariant(const QVariant& value) const
{

}

QVariant WidgetAction::toVariant() const
{
    return QVariant();
}

void WidgetAction::fromVariantMap(const QVariantMap& value) const
{

}

QVariantMap WidgetAction::toVariantMap(const WidgetAction* widgetAction)
{
#ifdef _DEBUG
    qDebug().noquote() << QString("Convert %1 to variant map").arg(widgetAction->text());
#endif

    QVariantMap resultVariantMap, widgetActionVariantMap;

    

    // Loop over all child objects
    for (auto child : widgetAction->children()) {

        // Cast to widget action
        auto childWidgetAction = dynamic_cast<WidgetAction*>(child);

        if (!childWidgetAction)
            continue;

        const auto childVariantMap = toVariantMap(childWidgetAction);
        widgetActionVariantMap[widgetAction->text()] = childVariantMap;

        //if (widgetAction->isSerializable())
            
    }

    resultVariantMap[widgetAction->text()] = widgetActionVariantMap;

    return resultVariantMap;
}

void WidgetAction::fromJsonDocument(const QJsonDocument& jsonDocument) const
{

}

QJsonDocument WidgetAction::toJsonDocument() const
{
    return QJsonDocument::fromVariant(toVariantMap(this));
}

void WidgetAction::fromJsonFile(const QString& filePath /*= ""*/)
{
    try
    {
        // Create file info to check if the supplied file path actually exists
        QFileInfo jsonFileInfo(filePath);

        // JSON file path for loading
        QString jsonFilePath = filePath;

        // Request user to select a JSON file if the supplied file path is not valid
        if (!jsonFileInfo.exists()) {

            // Supplied file path is not valid so create a file dialog for opening a JSON file
            QFileDialog fileDialog;

            // Configure file dialog
            fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
            fileDialog.setFileMode(QFileDialog::ExistingFile);
            fileDialog.setNameFilters({ "HDPS preset files (*json)" });
            fileDialog.setDefaultSuffix(".json");

            // Loading failed when the file dialog is canceled
            if (fileDialog.exec() == 0)
                throw std::runtime_error("File selection was canceled");

            // Only load if we have one file
            if (fileDialog.selectedFiles().count() != 1)
                throw std::runtime_error("Only one file may be selected");

            // Establish the JSON file path that will be loaded
            jsonFilePath = fileDialog.selectedFiles().first();
        }

        // Create the preset file
        QFile jsonPresetFile(jsonFilePath);

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
        exceptionMessageBox("Unable to load widget action from JSON file", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to load widget action from JSON file");
    }
}

void WidgetAction::toJsonFile(const QString& filePath /*= ""*/)
{
    try
    {
        // Create file info to check if the supplied file path actually exists
        QFileInfo jsonFileInfo(filePath);

        // JSON file path for saving
        QString jsonFilePath = filePath;

        // Request user to select a JSON file if the supplied file path is not valid
        if (!jsonFileInfo.exists()) {

            // Supplied file path is not valid so create a file dialog for saving a JSON file
            QFileDialog fileDialog;

            // Configure file dialog
            fileDialog.setAcceptMode(QFileDialog::AcceptSave);
            fileDialog.setNameFilters({ "HDPS preset files (*json)" });
            fileDialog.setDefaultSuffix(".json");

            // Saving failed when the file dialog is canceled
            if (fileDialog.exec() == 0)
                throw std::runtime_error("File selection was canceled");

            // Only save if we have one file
            if (fileDialog.selectedFiles().count() != 1)
                throw std::runtime_error("Only one file may be selected");

            // Establish the JSON file path that will be saved
            jsonFilePath = fileDialog.selectedFiles().first();
        }

        // Create the preset file
        QFile jsonFile(jsonFilePath);

        // And save the file
        if (!jsonFile.open(QFile::WriteOnly))
            throw std::runtime_error("Unable to open file for writing");

        // Create JSON document
        auto jsonDocument = toJsonDocument();

        // Shallow sanity check
        if (jsonDocument.isNull() || jsonDocument.isEmpty())
            throw std::runtime_error("JSON document is invalid");

        // Write the JSON document to disk
        jsonFile.write(QJsonDocument::fromVariant(toVariantMap(this)).toJson());
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to save widget action to JSON file", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to save widget action to JSON file");
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

WidgetAction* WidgetAction::getSerializationProxyParent()
{
    return _serializationProxyParent;
}

bool WidgetAction::hasSerializationProxyParent() const
{
    return _serializationProxyParent != nullptr;
}

void WidgetAction::setSerializationProxyParent(WidgetAction* serializationProxyParent)
{
    _serializationProxyParent = serializationProxyParent;
}

void WidgetAction::setValueFromVariant(const QVariant& value)
{
#ifdef _DEBUG
    qDebug().noquote() << "Set value not implemented in " << text();
#endif
}

QVariant WidgetAction::valueToVariant() const
{
#ifdef _DEBUG
    //qDebug().noquote() << "Value to variant not implemented in " << text();
#endif

    return QVariant();
}

QVariant WidgetAction::savedDefaultValueToVariant() const
{
    return Application::current()->getSetting(getSettingsPath() + "/Default");
}

QVariant WidgetAction::defaultValueToVariant() const
{
#ifdef _DEBUG
    //qDebug().noquote() << "Default value to variant not implemented in derived widget action class";
#endif

    return QVariant();
}

}
}
