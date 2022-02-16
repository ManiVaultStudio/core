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

//#define _VERBOSE

namespace hdps {

namespace gui {

WidgetAction::WidgetAction(QObject* parent /*= nullptr*/) :
    QWidgetAction(parent),
    _defaultWidgetFlags(),
    _sortIndex(-1),
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

bool WidgetAction::hasSavedDefault() const
{
    /*
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
    */

    return false;
}

bool WidgetAction::canSaveDefault(bool recursive /*= true*/) const
{
    /*
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
    */

    return false;
}

void WidgetAction::loadDefault(bool recursive /*= true*/)
{
#ifdef _VERBOSE
    qDebug().noquote() << QString("Load default for: %1").arg(getSettingsPath());
#endif

    /*
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
    */
}

void WidgetAction::saveDefault(bool recursive /*= true*/)
{
#ifdef _VERBOSE
    qDebug().noquote() << QString("Save default for: %1").arg(getSettingsPath());
#endif

    /*
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
    */
}

bool WidgetAction::isResettable(bool recursive /*= true*/) const
{
    /*
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
    */

    return false;
}

bool WidgetAction::isFactoryResettable(bool recursive /*= true*/) const
{
    /*
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
    */

    return false;
}

void WidgetAction::notifyResettable()
{
#ifdef _VERBOSE
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
    /*
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
    */
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
#ifdef _VERBOSE
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
#ifdef _VERBOSE
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

#if _VERBOSE
        qDebug().noquote() << jsonDocument.toJson(QJsonDocument::Indented);
#endif

        // Write the JSON document to disk
        jsonFile.write(jsonDocument.toJson());
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

}
}
