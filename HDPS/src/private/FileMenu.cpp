#include "FileMenu.h"

#include <Application.h>

using namespace hdps;

FileMenu::FileMenu(QWidget* parent /*= nullptr*/) :
    QMenu(parent),
    _openProjectAction("Open", this),
    _saveProjectAction("Save project", this),
    _saveProjectAsAction("Save project as", this),
    _recentProjectsMenu(this),
    _clearDatasetsAction("Clear datasets", this),
    _importDataMenu(this),
    _exitAction("Exit", this)
{
    setTitle("File");
    setToolTip("File operations");

    addAction(&_openProjectAction);
    addAction(&_saveProjectAction);
    addAction(&_saveProjectAsAction);
    addSeparator();
    addMenu(&_recentProjectsMenu);
    addSeparator();
    addAction(&_clearDatasetsAction);
    addMenu(&_importDataMenu);
    addSeparator();
    addAction(&_exitAction);

    _openProjectAction.setIcon(Application::getIconFont("FontAwesome").getIcon("folder-open"));
    _saveProjectAction.setIcon(Application::getIconFont("FontAwesome").getIcon("save"));
    _saveProjectAsAction.setIcon(Application::getIconFont("FontAwesome").getIcon("save"));
    _clearDatasetsAction.setIcon(Application::getIconFont("FontAwesome").getIcon("trash"));
    _importDataMenu.setIcon(Application::getIconFont("FontAwesome").getIcon("file-import"));
    _exitAction.setIcon(Application::getIconFont("FontAwesome").getIcon("sign-out-alt"));

    _openProjectAction.setToolTip("Open project from disk");
    _saveProjectAction.setToolTip("Save project to disk");
    _saveProjectAsAction.setToolTip("Save project to disk in a chosen location");
    _clearDatasetsAction.setToolTip("Reset the data model");
    _exitAction.setToolTip("Exit the HDPS application");

    connect(&_openProjectAction, &QAction::triggered, this, []() -> void {
        Application::current()->loadProject();
    });

    connect(&_saveProjectAction, &QAction::triggered, [this](bool) {
        Application::current()->saveProject(Application::current()->getCurrentProjectFilePath());
    });

    connect(&_saveProjectAsAction, &QAction::triggered, [this](bool) {
        Application::current()->saveProject();
    });

    connect(&_clearDatasetsAction, &QAction::triggered, this, [this]() -> void {
        const auto loadedDatasets = Application::core()->requestAllDataSets();

        if (!loadedDatasets.empty()) {

            if (Application::current()->getSetting("ConfirmDataRemoval", true).toBool()) {
                Application::core()->removeAllDatasets();

                //// Ask for confirmation dialog
                //DataRemoveAction::ConfirmDataRemoveDialog confirmDataRemoveDialog(nullptr, "Remove all datasets", loadedDatasets);

                //// Show the confirm data removal dialog
                //confirmDataRemoveDialog.exec();

                //// Remove dataset and children from the core if accepted
                //if (confirmDataRemoveDialog.result() == 1)
                //    Application::core()->removeAllDatasets();
            }
        }
    });

    connect(&_exitAction, SIGNAL(triggered()), this, SLOT(close()));

    //QObject::connect(findLogFileAction, &QAction::triggered, [this](bool) {
    //    const auto filePath = Logger::GetFilePathName();

    //    if (!hdps::util::ShowFileInFolder(filePath))
    //    {
    //        QMessageBox::information(this,
    //            QObject::tr("Log file not found"),
    //            QObject::tr("The log file is not found:\n%1").arg(filePath));
    //    }
    //});

    //QObject::connect(logViewAction, &QAction::triggered, [this](const bool checked) {
    //    if (checked) {
    //        _loggingDockWidget->setWidget(new LogDockWidget(*this));
    //        _loggingDockArea->show();
    //    }
    //    else {
    //        delete _loggingDockWidget->takeWidget();
    //        _loggingDockArea->hide();
    //    }
    //});

    // Update read-only status of various actions when the main file menu is opened
    connect(this, &QMenu::aboutToShow, this, [this]() -> void {
        const auto hasDatasets = Application::core()->requestAllDataSets().size();

        _saveProjectAction.setEnabled(!Application::current()->getCurrentProjectFilePath().isEmpty());
        _saveProjectAsAction.setEnabled(hasDatasets);
        _clearDatasetsAction.setEnabled(hasDatasets);

        _recentProjectsMenu.updateActions();

        _saveProjectAsAction.setEnabled(true);
    });
}
