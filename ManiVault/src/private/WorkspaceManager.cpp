// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WorkspaceManager.h"
#include "ViewPluginDockWidget.h"
#include "ViewMenu.h"
#include "LoadSystemViewMenu.h"
#include "DockComponentsFactory.h"
#include "Archiver.h"
#include "WorkspaceSettingsDialog.h"

#include "Application.h"
#include "CoreInterface.h"

#include "actions/StringsAction.h"

#include "util/Serialization.h"
#include "util/Icon.h"

#include "widgets/FileDialog.h"

#include <QMainWindow>
#include <QPainter>
#include <QStandardPaths>
#include <QBuffer>
#include <QOpenGLWidget>
#include <QEventLoop>

#include <exception>

#ifdef _DEBUG
    //#define WORKSPACE_MANAGER_VERBOSE
#endif

using namespace ads;

using namespace mv;
using namespace mv::plugin;
using namespace mv::util;
using namespace mv::gui;

namespace mv
{

WorkspaceManager::WorkspaceManager(QObject* parent) :
    AbstractWorkspaceManager(parent),
    _resetWorkspaceAction(this, "Reset"),
    _importWorkspaceAction(this, "Import"),
    _exportWorkspaceAction(this, "Export"),
    _exportWorkspaceAsAction(this, "Export As..."),
    _editWorkspaceSettingsAction(this, "Workspace Settings..."),
    _importWorkspaceFromProjectAction(this, "Import from project"),
    _recentWorkspacesAction(this, getSettingsPrefix() + "RecentWorkspaces")
{
    // Temporary solution for https://github.com/ManiVaultStudio/core/issues/274
    new QOpenGLWidget();

    //CDockManager::setConfigFlag(CDockManager::FocusHighlighting, true);
    CDockManager::setAutoHideConfigFlags(CDockManager::DefaultAutoHideConfig);
    //CDockManager::setAutoHideConfigFlag(CDockManager::AutoHideShowOnMouseOver, true);

    setObjectName("WorkspaceManager");

    ads::CDockComponentsFactory::setFactory(new DockComponentsFactory());

    _resetWorkspaceAction.setShortcut(QKeySequence("Ctrl+Alt+R"));
    _resetWorkspaceAction.setShortcutContext(Qt::ApplicationShortcut);
    _resetWorkspaceAction.setIconByName("undo");
    _resetWorkspaceAction.setToolTip("Reset the current workspace");

    _importWorkspaceAction.setShortcut(QKeySequence("Ctrl+Alt+I"));
    _importWorkspaceAction.setShortcutContext(Qt::ApplicationShortcut);
    _importWorkspaceAction.setIconByName("file-import");
    _importWorkspaceAction.setToolTip("Open workspace from disk");

    _exportWorkspaceAction.setShortcut(QKeySequence("Ctrl+Alt+E"));
    _exportWorkspaceAction.setShortcutContext(Qt::ApplicationShortcut);
    _exportWorkspaceAction.setIconByName("file-export");
    _exportWorkspaceAction.setToolTip("Export workspace to disk");

    _exportWorkspaceAsAction.setShortcut(QKeySequence("Ctrl+Alt+Shift+E"));
    _exportWorkspaceAsAction.setShortcutContext(Qt::ApplicationShortcut);
    _exportWorkspaceAsAction.setIconByName("file-export");
    _exportWorkspaceAsAction.setToolTip("Export workspace under a new file to disk");

    _editWorkspaceSettingsAction.setShortcut(QKeySequence("Ctrl+Alt+P"));
    _editWorkspaceSettingsAction.setShortcutContext(Qt::ApplicationShortcut);
    _editWorkspaceSettingsAction.setIconByName("cog");

    _importWorkspaceFromProjectAction.setIconByName("file-archive");
    _importWorkspaceFromProjectAction.setToolTip("Import workspace from project");

    auto mainWindow = Application::topLevelWidgets().first();

    mainWindow->addAction(&_resetWorkspaceAction);
    mainWindow->addAction(&_importWorkspaceAction);
    mainWindow->addAction(&_exportWorkspaceAction);
    mainWindow->addAction(&_exportWorkspaceAsAction);
    mainWindow->addAction(&_importWorkspaceFromProjectAction);
    mainWindow->addAction(&_editWorkspaceSettingsAction);

    connect(&_resetWorkspaceAction, &TriggerAction::triggered, [this](bool) {
        newWorkspace();
    });

    connect(&_importWorkspaceAction, &TriggerAction::triggered, [this](bool) {
        loadWorkspace();
    });

    connect(&_exportWorkspaceAction, &TriggerAction::triggered, [this](bool) {
        saveWorkspace(getWorkspaceFilePath());
    });

    connect(&_exportWorkspaceAsAction, &TriggerAction::triggered, [this](bool) {
        saveWorkspaceAs();
    });

    connect(&_editWorkspaceSettingsAction, &TriggerAction::triggered, this, [this]() -> void {
        auto* workspaceSettingsDialog = new WorkspaceSettingsDialog();
        connect(workspaceSettingsDialog, &WorkspaceSettingsDialog::finished, workspaceSettingsDialog, &WorkspaceSettingsDialog::deleteLater);
        workspaceSettingsDialog->open();
    });

    connect(&_importWorkspaceFromProjectAction, &TriggerAction::triggered, [this](bool) {
        importWorkspaceFromProjectFile("", false);
    });

    createIcon();

    _recentWorkspacesAction.initialize("Manager/Workspace/Recent", "Workspace", "Ctrl+Alt");

    connect(&_recentWorkspacesAction, &RecentFilesAction::triggered, this, [this](const QString& filePath) -> void {
        loadWorkspace(filePath);
    });

    QFile styleSheetFile(":/styles/ads_light.css");

    styleSheetFile.open(QIODevice::ReadOnly);

    QTextStream styleSheetStream(&styleSheetFile);

    _styleSheet = styleSheetStream.readAll();

    styleSheetFile.close();
}

WorkspaceManager::~WorkspaceManager()
{
    reset();
}

void WorkspaceManager::initialize()
{
#ifdef WORKSPACE_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    AbstractWorkspaceManager::initialize();

    if (isInitialized())
        return;

    beginInitialization();
    {
        const auto updateActionsReadOnly = [this]() -> void {
            _resetWorkspaceAction.setEnabled(!getLockingAction().isLocked());
            _importWorkspaceAction.setEnabled(!getLockingAction().isLocked());
            _importWorkspaceFromProjectAction.setEnabled(!getLockingAction().isLocked());
            _exportWorkspaceAction.setEnabled(true);
            _exportWorkspaceAsAction.setEnabled(true);
            _exportWorkspaceAsAction.setEnabled(!getWorkspaceFilePath().isEmpty());
            _recentWorkspacesAction.setEnabled(!getLockingAction().isLocked());
        };

        connect(this, &WorkspaceManager::workspaceLoaded, this, updateActionsReadOnly);
        connect(this, &WorkspaceManager::workspaceSaved, this, updateActionsReadOnly);
        connect(&getLockingAction(), &LockingAction::lockedChanged, this, updateActionsReadOnly);

        updateActionsReadOnly();

        getLockingAction().setWhat("Layout");

        _mainDockManager        = new DockManager("System view plugins");
        _viewPluginsDockManager = new DockManager("View plugins");
        _viewPluginsDockWidget  = new ViewPluginsDockWidget(_viewPluginsDockManager);

        _mainDockManager->setObjectName("MainDockManager");
        _viewPluginsDockManager->setObjectName("ViewPluginsDockManager");

        auto viewPluginsDockArea = _mainDockManager->setCentralWidget(_viewPluginsDockWidget.get());

        viewPluginsDockArea->setAllowedAreas(DockWidgetArea::NoDockWidgetArea);

        connect(&Application::core()->getPluginManager(), &AbstractPluginManager::pluginAboutToBeDestroyed, this, [this](plugin::Plugin* plugin) -> void {
            const auto viewPlugin = dynamic_cast<ViewPlugin*>(plugin);

            if (!viewPlugin)
                return;

            if (_mainDockManager && viewPlugin->isSystemViewPlugin())
                _mainDockManager->removeViewPluginDockWidget(viewPlugin);
            else if (_viewPluginsDockManager)
                _viewPluginsDockManager->removeViewPluginDockWidget(viewPlugin);
        });

        connect(&Application::core()->getProjectManager(), &AbstractProjectManager::projectCreated, this, [this]() -> void {
            newWorkspace();
        });
    }
    endInitialization();
}

void WorkspaceManager::reset()
{
#ifdef WORKSPACE_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    if (isCoreDestroyed())
        return;

    beginReset();
    {
		for (auto plugin : Application::core()->getPluginManager().getPluginsByType(plugin::Type::VIEW))
			plugin->destroy();

        _mainDockManager->reset();
        _viewPluginsDockManager->reset();
    }
    endReset();
}

QIcon WorkspaceManager::getIcon() const
{
    return _icon;
}

void WorkspaceManager::newWorkspace()
{
    try
    {
#ifdef WORKSPACE_MANAGER_VERBOSE
        qDebug() << __FUNCTION__;
#endif

        createWorkspace();
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to create new workspace", e);
    }
    catch (...)
    {
        exceptionMessageBox("Unable to create new workspace");
    }
}

void WorkspaceManager::loadWorkspace(QString filePath /*= ""*/, bool addToRecentWorkspaces /*= true*/)
{
    try
    {
#ifdef WORKSPACE_MANAGER_VERBOSE
        qDebug() << __FUNCTION__ << filePath;
#endif

        createWorkspace();

        setWorkspaceFilePath(filePath);

        beginLoadWorkspace();
        {
            if (filePath.isEmpty()) {
                FileOpenDialog fileOpenDialog;

                fileOpenDialog.setWindowTitle("Load ManiVault Workspace");
                fileOpenDialog.setNameFilters({ "ManiVault workspace files (*.json)" });
                fileOpenDialog.setDefaultSuffix(".json");
                fileOpenDialog.setDirectory(Application::current()->getSetting("Workspaces/WorkingDirectory", QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)).toString());
                fileOpenDialog.setOption(QFileDialog::DontUseNativeDialog, true);

                StringAction descriptionAction(this, "Description");
                StringAction tagsAction(this, "Tags");
                StringAction commentsAction(this, "Comments");

                descriptionAction.setEnabled(false);
                tagsAction.setEnabled(false);
                commentsAction.setEnabled(false);

                auto fileDialogLayout   = dynamic_cast<QGridLayout*>(fileOpenDialog.layout());
                auto rowCount           = fileDialogLayout->rowCount();

                fileDialogLayout->addWidget(descriptionAction.createLabelWidget(&fileOpenDialog), rowCount, 0);
                fileDialogLayout->addWidget(descriptionAction.createWidget(&fileOpenDialog), rowCount, 1, 1, 2);

                fileDialogLayout->addWidget(tagsAction.createLabelWidget(&fileOpenDialog), rowCount + 1, 0);
                fileDialogLayout->addWidget(tagsAction.createWidget(&fileOpenDialog), rowCount + 1, 1, 1, 2);

                fileDialogLayout->addWidget(commentsAction.createLabelWidget(&fileOpenDialog), rowCount + 2, 0);
                fileDialogLayout->addWidget(commentsAction.createWidget(&fileOpenDialog), rowCount + 2, 1, 1, 2);

                connect(&fileOpenDialog, &QFileDialog::currentChanged, this, [&](const QString& filePath) -> void {
                    if (!QFileInfo(filePath).isFile())
                        return;

                    Workspace workspace(filePath);

                    descriptionAction.setString(workspace.getDescriptionAction().getString());
                    tagsAction.setString(workspace.getTagsAction().getStrings().join(", "));
                    commentsAction.setString(workspace.getDescriptionAction().getString());
                });

                fileOpenDialog.open();

                QEventLoop eventLoop;
                QObject::connect(&fileOpenDialog, &QDialog::finished, &eventLoop, &QEventLoop::quit);
                eventLoop.exec();

                if (fileOpenDialog.result() != QDialog::Accepted)
                    return;

                if (fileOpenDialog.selectedFiles().count() != 1)
                    throw std::runtime_error("Only one file may be selected");

                filePath = fileOpenDialog.selectedFiles().first();

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

void WorkspaceManager::importWorkspaceFromProjectFile(QString projectFilePath /*= ""*/, bool addToRecentWorkspaces /*= true*/)
{
    QTemporaryDir temporaryDirectory(QDir::cleanPath(Application::current()->getTemporaryDir().path() + QDir::separator() + "ImportWorkspace"));

    const auto temporaryDirectoryPath = temporaryDirectory.path();

    Application::setSerializationAborted(false);

    if (projectFilePath.isEmpty()) {
        FileOpenDialog fileOpenDialog;

        fileOpenDialog.setWindowTitle("Import ManiVault Workspace From Project");
        fileOpenDialog.setNameFilters({ "ManiVault project files (*.mv)" });
        fileOpenDialog.setDefaultSuffix(".mv");
        fileOpenDialog.setDirectory(Application::current()->getSetting("Projects/WorkingDirectory", QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)).toString());
        fileOpenDialog.open();

        QEventLoop eventLoop;
        QObject::connect(&fileOpenDialog, &QDialog::finished, &eventLoop, &QEventLoop::quit);
        eventLoop.exec();

        if (fileOpenDialog.result() != QDialog::Accepted)
            return;

        if (fileOpenDialog.selectedFiles().count() != 1)
            throw std::runtime_error("Only one file may be selected");

        projectFilePath = fileOpenDialog.selectedFiles().first();
    }

    const QString workspaceFile("workspace.json");

    QFileInfo workspaceFileInfo(temporaryDirectoryPath, workspaceFile);

    try
    {
        Archiver archiver;

        archiver.extractSingleFile(projectFilePath, workspaceFile, workspaceFileInfo.absoluteFilePath());
    }
    catch (const std::runtime_error& e)
    {
        qDebug() << "WorkspaceManager: exception caught in importWorkspaceFromProjectFile, given file path " << projectFilePath << ": " << e.what();
        return;
    }

    if (workspaceFileInfo.exists())
        loadWorkspace(workspaceFileInfo.absoluteFilePath(), false);
}

void WorkspaceManager::saveWorkspace(QString filePath /*= ""*/, bool addToRecentWorkspaces /*= true*/)
{
    try
    {
#ifdef WORKSPACE_MANAGER_VERBOSE
        qDebug() << __FUNCTION__ << filePath;
#endif
        setWorkspaceFilePath(filePath);

        beginSaveWorkspace();
        {
            auto currentWorkspace = getCurrentWorkspace();

            if (filePath.isEmpty()) {

                FileSaveDialog fileSaveDialog;

                fileSaveDialog.setWindowTitle("Export ManiVault Workspace");
                fileSaveDialog.setNameFilters({ "ManiVault workspace files (*.json)" });
                fileSaveDialog.setDefaultSuffix(".json");
                fileSaveDialog.setDirectory(Application::current()->getSetting("Workspaces/WorkingDirectory", QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)).toString());

                auto fileDialogLayout   = dynamic_cast<QGridLayout*>(fileSaveDialog.layout());
                auto rowCount           = fileDialogLayout->rowCount();

                auto& titleAction = currentWorkspace->getTitleAction();

                fileDialogLayout->addWidget(titleAction.createLabelWidget(nullptr), rowCount, 0);

                GroupAction settingsGroupAction(this, "Group");

                settingsGroupAction.setIconByName("cog");
                settingsGroupAction.setToolTip("Edit workspace settings");
                settingsGroupAction.setPopupSizeHint(QSize(420, 320));
                settingsGroupAction.setLabelSizingType(GroupAction::LabelSizingType::Auto);

                settingsGroupAction.addAction(&currentWorkspace->getTitleAction());
                settingsGroupAction.addAction(&currentWorkspace->getDescriptionAction());
                settingsGroupAction.addAction(&currentWorkspace->getTagsAction());
                settingsGroupAction.addAction(&currentWorkspace->getCommentsAction());

                auto titleLayout = new QHBoxLayout();

                titleLayout->addWidget(titleAction.createWidget(&fileSaveDialog));
                titleLayout->addWidget(settingsGroupAction.createCollapsedWidget(&fileSaveDialog));

                fileDialogLayout->addLayout(titleLayout, rowCount, 1, 1, 2);

                fileSaveDialog.open();

                QEventLoop eventLoop;
                QObject::connect(&fileSaveDialog, &QDialog::finished, &eventLoop, &QEventLoop::quit);
                eventLoop.exec();

                if (fileSaveDialog.result() != QDialog::Accepted)
                    return;

                if (fileSaveDialog.selectedFiles().count() != 1)
                    throw std::runtime_error("Only one file may be selected");

                filePath = fileSaveDialog.selectedFiles().first();

                Application::current()->setSetting("Workspaces/WorkingDirectory", QFileInfo(filePath).absolutePath());
            }

            toJsonFile(filePath);

            setWorkspaceFilePath(filePath);

            if (addToRecentWorkspaces)
                _recentWorkspacesAction.addRecentFilePath(getWorkspaceFilePath());

            QFileInfo workspacePreviewFileInfo(QFileInfo(filePath).absoluteDir(), "workspace.jpg");

            toPreviewImage().save(workspacePreviewFileInfo.absoluteFilePath());
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

void WorkspaceManager::addViewPlugin(plugin::ViewPlugin* viewPlugin, plugin::ViewPlugin* dockToViewPlugin /*= nullptr*/, DockAreaFlag dockArea /*= DockAreaFlag::Right*/)
{
    Q_ASSERT(viewPlugin);

    if (!viewPlugin)
        return;

    auto viewPluginDockWidget = new ViewPluginDockWidget(viewPlugin->getGuiName(), viewPlugin);

    if (viewPlugin->isSystemViewPlugin())
        _mainDockManager->addViewPluginDockWidget(static_cast<DockWidgetArea>(dockArea), viewPluginDockWidget, dockToViewPlugin ? _mainDockManager->findDockAreaWidget(dockToViewPlugin) : nullptr);
    else
        _viewPluginsDockManager->addViewPluginDockWidget(static_cast<DockWidgetArea>(dockArea), viewPluginDockWidget, dockToViewPlugin ? _viewPluginsDockManager->findDockAreaWidget(dockToViewPlugin) : nullptr);

    if (projects().isOpeningProject() || projects().isImportingProject())
        return;

    viewPlugin->getPresetsAction().loadDefaultPreset();
}

void WorkspaceManager::addViewPluginFloated(plugin::ViewPlugin* viewPlugin)
{
    Q_ASSERT(viewPlugin);

    if (!viewPlugin)
        return;

    auto viewPluginDockWidget = new ViewPluginDockWidget(viewPlugin->getGuiName(), viewPlugin);

    if (viewPlugin->isSystemViewPlugin())
        _mainDockManager->addDockWidgetFloating(viewPluginDockWidget);
    else
        _viewPluginsDockManager->addDockWidgetFloating(viewPluginDockWidget);
}

void WorkspaceManager::isolateViewPlugin(plugin::ViewPlugin* viewPlugin, bool isolate)
{
#ifdef WORKSPACE_MANAGER_VERBOSE
    qDebug() << __FUNCTION__ << viewPlugin->getGuiName() << isolate;
#endif

    ViewPluginsDockWidget::isolate(viewPlugin, isolate);
}

QWidget* WorkspaceManager::getWidget()
{
    return  _mainDockManager->getWidget();
}

bool WorkspaceManager::hasWorkspace() const
{
    return getCurrentWorkspace() != nullptr;
}

const Workspace* WorkspaceManager::getCurrentWorkspace() const
{
    return _workspace.get();
}

Workspace* WorkspaceManager::getCurrentWorkspace()
{
    return _workspace.get();
}

void WorkspaceManager::fromVariantMap(const QVariantMap& variantMap)
{
    auto& projectSerializationTask = projects().getProjectSerializationTask();

    _mainDockManager->setSerializationTask(&projectSerializationTask.getSystemViewPluginsTask());
    _viewPluginsDockManager->setSerializationTask(&projectSerializationTask.getViewPluginsTask());

    getCurrentWorkspace()->fromVariantMap(variantMap);

    variantMapMustContain(variantMap, "DockManagers");

    const auto dockingManagersMap = variantMap["DockManagers"].toMap();

    variantMapMustContain(dockingManagersMap, "Main");
    variantMapMustContain(dockingManagersMap, "ViewPlugins");

    _mainDockManager->fromVariantMap(dockingManagersMap["Main"].toMap());
    _viewPluginsDockManager->fromVariantMap(dockingManagersMap["ViewPlugins"].toMap());

    for (auto viewPluginDockWidget : _mainDockManager->getViewPluginDockWidgets(true))
    	viewPluginDockWidget->restoreViewPluginState();

    for (auto viewPluginDockWidget : _viewPluginsDockManager->getViewPluginDockWidgets(true))
        viewPluginDockWidget->restoreViewPluginState();
}

QVariantMap WorkspaceManager::toVariantMap() const
{
    auto& projectSerializationTask = projects().getProjectSerializationTask();

    _mainDockManager->setSerializationTask(&projectSerializationTask.getSystemViewPluginsTask());
    _viewPluginsDockManager->setSerializationTask(&projectSerializationTask.getViewPluginsTask());

    auto currentWorkspaceMap = getCurrentWorkspace()->toVariantMap();

    QVariantMap dockManagers{
        { "Main", _mainDockManager->toVariantMap() },
        { "ViewPlugins", _viewPluginsDockManager->toVariantMap() }
    };

    currentWorkspaceMap.insert({
        { "DockManagers", dockManagers }
    });

    return currentWorkspaceMap;
}

QMenu* WorkspaceManager::getMenu(QWidget* parent /*= nullptr*/)
{
    auto menu = new QMenu("Workspace", parent);

    menu->setTitle("Workspace");
    menu->setIcon(_icon);
    menu->setToolTip("Workspace operations");
    menu->setEnabled(Application::core()->getProjectManager().hasProject());

    menu->addAction(&_importWorkspaceAction);
    menu->addAction(&_importWorkspaceFromProjectAction);
    menu->addAction(&_exportWorkspaceAction);
    menu->addAction(&_exportWorkspaceAsAction);
    menu->addSeparator();
    menu->addAction(&_resetWorkspaceAction);
    menu->addSeparator();
    menu->addAction(&_editWorkspaceSettingsAction);
    menu->addSeparator();
    menu->addMenu(_recentWorkspacesAction.getMenu());

    return menu;
}

void WorkspaceManager::createWorkspace()
{
    emit workspaceAboutToBeCreated();
    {
        reset();

        _workspace.reset(new Workspace());
    }
    emit workspaceCreated(*_workspace);
}

void WorkspaceManager::createIcon()
{
    constexpr auto size             = 128;
    constexpr auto halfSize         = size / 2;
    constexpr auto margin           = 12;
    constexpr auto spacing          = 14;
    constexpr auto halfSpacing      = spacing / 2;

    QPixmap pixmap(size, size);

    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);

    painter.setWindow(0, 0, size, size);

    const auto drawWindow = [&](const QRectF& rectangle) -> void {
        painter.setBrush(Qt::black);
        painter.setPen(Qt::NoPen);
        painter.drawRect(rectangle);
    };

    drawWindow(QRectF(QPointF(margin, margin), QPointF(halfSize - halfSpacing, size - margin)));
    drawWindow(QRectF(QPointF(halfSize + halfSpacing, margin), QPointF(size - margin, halfSize - halfSpacing)));
    drawWindow(QRectF(QPointF(halfSize + halfSpacing, halfSize + halfSpacing), QPointF(size - margin, size - margin)));

    _icon = gui::createIcon(pixmap);
}

WorkspaceLocations WorkspaceManager::getWorkspaceLocations(const WorkspaceLocation::Types& types /*= WorkspaceLocation::Type::All*/)
{
    WorkspaceLocations workspaceLocations;

    if (types.testFlag(WorkspaceLocation::Type::BuiltIn)) {
        QStringList workspaceFilter("*.json");

        QDir workspaceExamplesDirectory(QString("%1/examples/workspaces/").arg(qApp->applicationDirPath()));

        for (const auto& workspaceFileName : workspaceExamplesDirectory.entryList(workspaceFilter))
            workspaceLocations << WorkspaceLocation(workspaceFileName, QString("%1/%2").arg(workspaceExamplesDirectory.absolutePath(), workspaceFileName), WorkspaceLocation::Type::BuiltIn);
    }

    if (types.testFlag(WorkspaceLocation::Type::Path)) {

    }

    if (types.testFlag(WorkspaceLocation::Type::Recent)) {
        _recentWorkspacesAction.updateRecentFilePaths();

        for (const auto& recentFilePath : _recentWorkspacesAction.getRecentFilePaths()) {
            Workspace workspace(recentFilePath);

            workspaceLocations << WorkspaceLocation(workspace.getTitleAction().getString(), recentFilePath, WorkspaceLocation::Type::Recent);
        }
    }
    
    return workspaceLocations;
}

void WorkspaceManager::setViewPluginDockWidgetPermissionsGlobally(const util::DockWidgetPermissions& dockWidgetPermissions /*= util::DockWidgetPermission::All*/, bool set /*= true*/)
{
    for (auto plugin : plugins().getPluginsByType(plugin::Type::VIEW)) {
        auto viewPlugin = dynamic_cast<ViewPlugin*>(plugin);

        if (dockWidgetPermissions.testFlag(DockWidgetPermission::MayClose))
            viewPlugin->getMayCloseAction().setChecked(set);

        if (dockWidgetPermissions.testFlag(DockWidgetPermission::MayFloat))
            viewPlugin->getMayFloatAction().setChecked(set);

        if (dockWidgetPermissions.testFlag(DockWidgetPermission::MayMove))
            viewPlugin->getMayMoveAction().setChecked(set);
    }
}

bool WorkspaceManager::mayLock() const
{
    for (auto plugin : plugins().getPluginsByType(plugin::Type::VIEW))
        if (!dynamic_cast<mv::plugin::ViewPlugin*>(plugin)->getLockingAction().isLocked())
            return true;

    return false;
}

bool WorkspaceManager::mayUnlock() const
{
    for (auto plugin : plugins().getPluginsByType(plugin::Type::VIEW))
        if (dynamic_cast<mv::plugin::ViewPlugin*>(plugin)->getLockingAction().isLocked())
            return true;

    return false;
}

QStringList WorkspaceManager::getViewPluginNames(const QString& workspaceJsonFile) const
{
    QFile jsonFile(workspaceJsonFile);

    if (!jsonFile.open(QIODevice::ReadOnly))
        return {};

    QByteArray data = jsonFile.readAll();

    QJsonDocument jsonDocument = QJsonDocument::fromJson(data);

    if (jsonDocument.isNull() || jsonDocument.isEmpty())
        return {};

    const auto variantMap       = jsonDocument.toVariant().toMap();
    const auto workspaceMap     = variantMap["Workspace"].toMap();
    const auto dockManagersMap  = workspaceMap["DockManagers"].toMap();

    const auto getViewPluginNamesFromDockManager = [](const QVariantMap& dockManagerMap) -> QStringList {
        QStringList viewPluginNames;

        for (const auto& viewPluginDockWidgetVariant : dockManagerMap["ViewPluginDockWidgets"].toList()) {
            const auto viewPluginMap = viewPluginDockWidgetVariant.toMap()["ViewPlugin"].toMap();

            viewPluginNames << viewPluginMap["GuiName"].toMap()["Value"].toString();
        }

        return viewPluginNames;
    };

    QStringList viewPluginNames;

    viewPluginNames << getViewPluginNamesFromDockManager(dockManagersMap["Main"].toMap());
    viewPluginNames << getViewPluginNamesFromDockManager(dockManagersMap["ViewPlugins"].toMap());

    return viewPluginNames;

}

QImage WorkspaceManager::toPreviewImage() const
{
    return _mainDockManager->grab().toImage();
}

}