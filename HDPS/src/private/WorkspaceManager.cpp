#include "WorkspaceManager.h"
#include "ViewPluginDockWidget.h"
#include "ViewMenu.h"
#include "LoadSystemViewMenu.h"
#include "DockComponentsFactory.h"
#include "Archiver.h"

#include <Application.h>

#include <util/Serialization.h>
#include <util/Icon.h>

#include <QMainWindow>
#include <QToolButton>
#include <QPainter>
#include <QFileDialog>
#include <QStandardPaths>
#include <QBuffer>
#include <QTemporaryDir>

#ifdef _DEBUG
    #define WORKSPACE_MANAGER_VERBOSE
#endif

using namespace ads;

using namespace hdps;
using namespace hdps::plugin;
using namespace hdps::util;
using namespace hdps::gui;

namespace hdps
{

WorkspaceManager::WorkspaceManager() :
    AbstractWorkspaceManager(),
    _mainDockManager(),
    _viewPluginsDockWidget(),
    _loadWorkspaceAction(this, "Load"),
    _importWorkspaceFromProjectAction(this, "Import from project"),
    _newWorkspaceAction(this, "New"),
    _saveWorkspaceAction(this, "Save"),
    _saveWorkspaceAsAction(this, "Save As..."),
    _recentWorkspacesAction(this),
    _icon()
{
    setObjectName("WorkspaceManager");

    ads::CDockComponentsFactory::setFactory(new DockComponentsFactory());

    _newWorkspaceAction.setShortcut(QKeySequence("Ctrl+Alt+N"));
    _newWorkspaceAction.setShortcutContext(Qt::ApplicationShortcut);
    _newWorkspaceAction.setIcon(Application::getIconFont("FontAwesome").getIcon("file"));
    _newWorkspaceAction.setToolTip("Create new workspace");

    _loadWorkspaceAction.setShortcut(QKeySequence("Ctrl+Alt+L"));
    _loadWorkspaceAction.setShortcutContext(Qt::ApplicationShortcut);
    _loadWorkspaceAction.setIcon(Application::getIconFont("FontAwesome").getIcon("folder-open"));
    _loadWorkspaceAction.setToolTip("Open workspace from disk");

    _saveWorkspaceAction.setShortcut(QKeySequence("Ctrl+Alt+S"));
    _saveWorkspaceAction.setShortcutContext(Qt::ApplicationShortcut);
    _saveWorkspaceAction.setIcon(Application::getIconFont("FontAwesome").getIcon("save"));
    _saveWorkspaceAction.setToolTip("Save workspace to disk");

    _saveWorkspaceAsAction.setShortcut(QKeySequence("Ctrl+Alt+Shift+S"));
    _saveWorkspaceAsAction.setShortcutContext(Qt::ApplicationShortcut);
    _saveWorkspaceAsAction.setIcon(Application::getIconFont("FontAwesome").getIcon("save"));
    _saveWorkspaceAsAction.setToolTip("Save workspace under a new file to disk");

    _importWorkspaceFromProjectAction.setShortcut(QKeySequence("Ctrl+Alt+I"));
    _importWorkspaceFromProjectAction.setShortcutContext(Qt::ApplicationShortcut);
    _importWorkspaceFromProjectAction.setIcon(Application::getIconFont("FontAwesome").getIcon("file-import"));
    _importWorkspaceFromProjectAction.setToolTip("Import workspace from project");
    //_importWorkspaceFromProjectAction.setEnabled(false);

    auto mainWindow = Application::topLevelWidgets().first();

    mainWindow->addAction(&_newWorkspaceAction);
    mainWindow->addAction(&_loadWorkspaceAction);
    mainWindow->addAction(&_importWorkspaceFromProjectAction);
    mainWindow->addAction(&_saveWorkspaceAction);
    mainWindow->addAction(&_saveWorkspaceAsAction);

    connect(&_newWorkspaceAction, &TriggerAction::triggered, [this](bool) {
        reset();
        setWorkspaceFilePath("");
    });

    connect(&_loadWorkspaceAction, &TriggerAction::triggered, [this](bool) {
        loadWorkspace();
    });

    connect(&_saveWorkspaceAction, &TriggerAction::triggered, [this](bool) {
        saveWorkspace(getWorkspaceFilePath());
    });

    connect(&_saveWorkspaceAsAction, &TriggerAction::triggered, [this](bool) {
        saveWorkspaceAs();
    });

    connect(&_importWorkspaceFromProjectAction, &TriggerAction::triggered, [this](bool) {
        importWorkspaceFromProjectFile("", false);
    });

    createIcon();

    _recentWorkspacesAction.initialize("Manager/Workspace/Recent", "Workspace", "Ctrl+Alt", _icon);

    connect(&_recentWorkspacesAction, &RecentFilesAction::triggered, this, [this](const QString& filePath) -> void {
        loadWorkspace(filePath);
    });

    const auto updateActionsReadOnly = [this]() -> void {
        _saveWorkspaceAction.setEnabled(true);
        _saveWorkspaceAsAction.setEnabled(true);
        _saveWorkspaceAsAction.setEnabled(!getWorkspaceFilePath().isEmpty());
    };

    connect(this, &WorkspaceManager::workspaceLoaded, this, updateActionsReadOnly);
    connect(this, &WorkspaceManager::workspaceSaved, this, updateActionsReadOnly);

    updateActionsReadOnly();
}

void WorkspaceManager::initalize()
{
#ifdef WORKSPACE_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    if (isInitialized())
        return;

    beginInitialization();
    {
        _mainDockManager        = new DockManager();
        _viewPluginsDockManager = new DockManager();
        _viewPluginsDockWidget  = new ViewPluginsDockWidget(_viewPluginsDockManager);

        _mainDockManager->setConfigFlag(CDockManager::FocusHighlighting, true);
        _mainDockManager->setObjectName("MainDockManager");

        _viewPluginsDockManager->setConfigFlag(CDockManager::FocusHighlighting, true);
        _viewPluginsDockManager->setObjectName("ViewPluginsDockManager");

        auto viewPluginsDockArea = _mainDockManager->setCentralWidget(_viewPluginsDockWidget.get());

        viewPluginsDockArea->setAllowedAreas(DockWidgetArea::NoDockWidgetArea);

        connect(&Application::core()->getPluginManager(), &AbstractPluginManager::pluginAboutToBeDestroyed, this, [this](plugin::Plugin* plugin) -> void {
            const auto viewPlugin = dynamic_cast<ViewPlugin*>(plugin);

            if (!viewPlugin)
                return;
            
            if (viewPlugin->isSystemViewPlugin())
                _mainDockManager->removeViewPluginDockWidget(viewPlugin);
            else
                _viewPluginsDockManager->removeViewPluginDockWidget(viewPlugin);
        });

        connect(&Application::core()->getProjectManager(), &AbstractProjectManager::projectCreated, this, [this]() -> void {
            setWorkspaceFilePath("");
        });
    }
    endInitialization();
}

void WorkspaceManager::reset()
{
#ifdef WORKSPACE_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    beginReset();
    {
        for (auto plugin : Application::core()->getPluginManager().getPluginsByType(plugin::Type::VIEW))
            plugin->destroy();
    }
    endReset();
}

void WorkspaceManager::loadWorkspace(QString filePath /*= ""*/, bool addToRecentWorkspaces /*= true*/)
{
    try
    {
#ifdef WORKSPACE_MANAGER_VERBOSE
        qDebug() << __FUNCTION__ << filePath;
#endif

        reset();

        setWorkspaceFilePath(filePath);

        beginLoadWorkspace();
        {
            if (filePath.isEmpty()) {
                QFileDialog fileDialog;

                fileDialog.setWindowIcon(Application::getIconFont("FontAwesome").getIcon("folder-open"));
                fileDialog.setWindowTitle("Load workspace");
                fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
                fileDialog.setFileMode(QFileDialog::ExistingFile);
                fileDialog.setNameFilters({ "HDPS workspace files (*.hws)" });
                fileDialog.setDefaultSuffix(".hws");
                fileDialog.setDirectory(Application::current()->getSetting("Workspaces/WorkingDirectory", QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)).toString());
                fileDialog.setOption(QFileDialog::DontUseNativeDialog, true);
                fileDialog.setMinimumHeight(700);

                auto fileDialogLayout = dynamic_cast<QGridLayout*>(fileDialog.layout());
                auto rowCount = fileDialogLayout->rowCount();

                QLabel label("Preview:");
                QLabel image("");

                fileDialogLayout->addWidget(&label, rowCount, 0);
                fileDialogLayout->addWidget(&image, rowCount, 1, 1, 2);

                connect(&fileDialog, &QFileDialog::currentChanged, this, [this, &image](const QString& filePath) -> void {
                    image.setPixmap(QPixmap::fromImage(getPreviewImageFromWorkspaceFile(filePath).scaledToWidth(650, Qt::SmoothTransformation)));
                });

                if (fileDialog.exec() == 0)
                    return;

                if (fileDialog.selectedFiles().count() != 1)
                    throw std::runtime_error("Only one file may be selected");

                filePath = fileDialog.selectedFiles().first();

                Application::current()->setSetting("Workspaces/WorkingDirectory", QFileInfo(filePath).absolutePath());
            }

            fromJsonFile(filePath);

            if (addToRecentWorkspaces)
                _recentWorkspacesAction.addRecentFilePath(filePath);
        }
        endLoadWorkspace();
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to load workspace", e);
    }
    catch (...)
    {
        exceptionMessageBox("Unable to load workspace");
    }
}

void WorkspaceManager::saveWorkspace(QString filePath /*= ""*/)
{
    try
    {
#ifdef WORKSPACE_MANAGER_VERBOSE
        qDebug() << __FUNCTION__ << filePath;
#endif
        setWorkspaceFilePath(filePath);

        beginSaveWorkspace();
        {
            if (filePath.isEmpty()) {

                QFileDialog fileDialog;

                fileDialog.setWindowIcon(Application::getIconFont("FontAwesome").getIcon("save"));
                fileDialog.setWindowTitle("Save workspace");
                fileDialog.setAcceptMode(QFileDialog::AcceptSave);
                fileDialog.setNameFilters({ "HDPS workspace files (*.hws)" });
                fileDialog.setDefaultSuffix(".hws");
                fileDialog.setOption(QFileDialog::DontUseNativeDialog, true);
                fileDialog.setDirectory(Application::current()->getSetting("Workspaces/WorkingDirectory", QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)).toString());

                fileDialog.exec();

                if (fileDialog.selectedFiles().count() != 1)
                    throw std::runtime_error("Only one file may be selected");

                filePath = fileDialog.selectedFiles().first();

                Application::current()->setSetting("Workspaces/WorkingDirectory", QFileInfo(filePath).absolutePath());
            }

            toJsonFile(filePath);

            setWorkspaceFilePath(filePath);

            _recentWorkspacesAction.addRecentFilePath(getWorkspaceFilePath());
        }
        endSaveWorkspace();
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to save workspace", e);
    }
    catch (...)
    {
        exceptionMessageBox("Unable to save workspace");
    }
}

void WorkspaceManager::saveWorkspaceAs()
{
#ifdef WORKSPACE_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    saveWorkspace();
}

void WorkspaceManager::fromVariantMap(const QVariantMap& variantMap)
{
    variantMapMustContain(variantMap, "DockManagers");

    const auto dockingManagersMap = variantMap["DockManagers"].toMap();

    variantMapMustContain(dockingManagersMap, "Main");
    variantMapMustContain(dockingManagersMap, "ViewPlugins");
        
    _mainDockManager->fromVariantMap(dockingManagersMap["Main"].toMap());
    _viewPluginsDockManager->fromVariantMap(dockingManagersMap["ViewPlugins"].toMap());
}

QVariantMap WorkspaceManager::toVariantMap() const
{
    const auto mainDockingManager           = _mainDockManager->toVariantMap();
    const auto viewPluginsDockingManager    = _viewPluginsDockManager->toVariantMap();

    const QVariantMap dockManagers = {
        { "Main", mainDockingManager },
        { "ViewPlugins", viewPluginsDockingManager }
    };

    QByteArray previewImageByteArray;
    QBuffer previewImageBuffer(&previewImageByteArray);

    toPreviewImage().save(&previewImageBuffer, "PNG");

    return {
        { "DockManagers", dockManagers },
        { "PreviewImage", QVariant::fromValue(previewImageByteArray.toBase64()) }
    };
}

void WorkspaceManager::addViewPlugin(plugin::ViewPlugin* viewPlugin, plugin::ViewPlugin* dockToViewPlugin /*= nullptr*/, DockAreaFlag dockArea /*= DockAreaFlag::Right*/)
{
    auto viewPluginDockWidget = new ViewPluginDockWidget(viewPlugin->getGuiName(), viewPlugin);

    if (viewPlugin->isSystemViewPlugin())
        _mainDockManager->addDockWidget(static_cast<DockWidgetArea>(dockArea), viewPluginDockWidget, _mainDockManager->findDockAreaWidget(dockToViewPlugin ? &dockToViewPlugin->getWidget() : nullptr));
    else
        _viewPluginsDockManager->addDockWidget(static_cast<DockWidgetArea>(dockArea), viewPluginDockWidget, dockToViewPlugin ? _viewPluginsDockManager->findDockAreaWidget(dockToViewPlugin) : nullptr);
}

void WorkspaceManager::isolateViewPlugin(plugin::ViewPlugin* viewPlugin, bool isolate)
{
#ifdef WORKSPACE_MANAGER_VERBOSE
    qDebug() << __FUNCTION__ << viewPlugin->getGuiName() << isolate;
#endif

    ViewPluginsDockWidget::isolate(viewPlugin, isolate);
}

QMenu* WorkspaceManager::getMenu(QWidget* parent /*= nullptr*/)
{
    auto menu = new QMenu("Workspace", parent);

    menu->setTitle("Workspace");
    menu->setIcon(_icon);
    menu->setToolTip("Workspace operations");
    menu->setEnabled(Application::core()->getProjectManager().hasProject());

    menu->addAction(&_newWorkspaceAction);
    menu->addAction(&_loadWorkspaceAction);
    menu->addAction(&_saveWorkspaceAction);
    menu->addAction(&_saveWorkspaceAsAction);
    menu->addSeparator();
    menu->addAction(&_importWorkspaceFromProjectAction);
    menu->addSeparator();
    menu->addMenu(_recentWorkspacesAction.getMenu());

    return menu;
}

void WorkspaceManager::createIcon()
{
    const auto size             = 128;
    const auto halfSize         = size / 2;
    const auto margin           = 12;
    const auto spacing          = 14;
    const auto halfSpacing      = spacing / 2;
    const auto lineThickness    = 7.0;

    QPixmap pixmap(size, size);

    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);

    painter.setWindow(0, 0, size, size);

    const auto drawWindow = [&](QRectF rectangle) -> void {
        painter.setBrush(Qt::black);
        painter.setPen(Qt::NoPen);
        painter.drawRect(rectangle);
    };

    drawWindow(QRectF(QPointF(margin, margin), QPointF(halfSize - halfSpacing, size - margin)));
    drawWindow(QRectF(QPointF(halfSize + halfSpacing, margin), QPointF(size - margin, halfSize - halfSpacing)));
    drawWindow(QRectF(QPointF(halfSize + halfSpacing, halfSize + halfSpacing), QPointF(size - margin, size - margin)));

    _icon = hdps::gui::createIcon(pixmap);
}

QWidget* WorkspaceManager::getWidget()
{
    return _mainDockManager.get();
}

QImage WorkspaceManager::toPreviewImage() const
{
    return _mainDockManager->grab().toImage();
}

QImage WorkspaceManager::getPreviewImageFromWorkspaceFile(const QString& workspaceFilePath) const
{
    QImage previewImage;

    try {
        if (!QFileInfo(workspaceFilePath).exists())
            throw std::runtime_error("File does not exist");

        QFile workspaceJsonFile(workspaceFilePath);

        if (!workspaceJsonFile.open(QIODevice::ReadOnly))
            throw std::runtime_error("Unable to open file for reading");

        QByteArray data = workspaceJsonFile.readAll();

        QJsonDocument jsonDocument;

        jsonDocument = QJsonDocument::fromJson(data);

        if (jsonDocument.isNull() || jsonDocument.isEmpty())
            throw std::runtime_error("JSON document is invalid");

        const auto workspaceVariantMap = jsonDocument.toVariant().toMap()["Workspace"].toMap();

        previewImage.loadFromData(QByteArray::fromBase64(workspaceVariantMap["PreviewImage"].toByteArray()));
        
        return previewImage;
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to retrieve preview image from workspace file", e);
        return previewImage;
    }
    catch (...)
    {
        exceptionMessageBox("Unable to retrieve preview image from workspace file");
        return previewImage;
    }
}

void WorkspaceManager::importWorkspaceFromProjectFile(QString filePath /*= ""*/, bool addToRecentWorkspaces /*= true*/)
{
    QTemporaryDir temporaryDirectory;

    const auto temporaryDirectoryPath = temporaryDirectory.path();

    Application::setSerializationTemporaryDirectory(temporaryDirectoryPath);
    Application::setSerializationAborted(false);

    if (filePath.isEmpty()) {
        QFileDialog fileDialog;

        fileDialog.setWindowIcon(Application::getIconFont("FontAwesome").getIcon("folder-open"));
        fileDialog.setWindowTitle("Load project");
        fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
        fileDialog.setFileMode(QFileDialog::ExistingFile);
        fileDialog.setNameFilters({ "HDPS project files (*.hdps)" });
        fileDialog.setDefaultSuffix(".hdps");
        fileDialog.setDirectory(Application::current()->getSetting("Projects/WorkingDirectory", QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)).toString());

        if (fileDialog.exec() == 0)
            return;

        if (fileDialog.selectedFiles().count() != 1)
            throw std::runtime_error("Only one file may be selected");

        filePath = fileDialog.selectedFiles().first();
    }

    Archiver archiver;

    QFileInfo workspaceFileInfo(temporaryDirectoryPath, "workspace.hws");

    archiver.extractSingleFile(filePath, "workspace.hws", workspaceFileInfo.absoluteFilePath());

    if (workspaceFileInfo.exists())
        loadWorkspace(workspaceFileInfo.absoluteFilePath(), false);
}

}
