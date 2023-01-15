#include "StartPageOpenProjectWidget.h"
#include "StartPageContentWidget.h"

#include <Application.h>

#include <CoreInterface.h>

#include <QDebug>

using namespace hdps;

StartPageOpenProjectWidget::StartPageOpenProjectWidget(QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _openProjectWidget(this),
    _recentProjectsWidget(this),
    _recentProjectsAction(this)
{
    auto layout = new QVBoxLayout();

    layout->addWidget(StartPageContentWidget::createHeaderLabel("Open", "Open existing project"));
    layout->addWidget(&_openProjectWidget);

    layout->addWidget(StartPageContentWidget::createHeaderLabel("Recent", "Recently opened project"));
    layout->addWidget(&_recentProjectsWidget, 1);

    setLayout(layout);

    _openProjectWidget.getHierarchyWidget().getFilterNameAction().setVisible(false);
    _openProjectWidget.getHierarchyWidget().getFilterGroupAction().setVisible(false);
    _openProjectWidget.getHierarchyWidget().setFixedHeight(45);
    _openProjectWidget.getHierarchyWidget().setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
 
    _recentProjectsWidget.getHierarchyWidget().setItemTypeName("Recent Project");
}

void StartPageOpenProjectWidget::updateActions()
{
    auto& fontAwesome = Application::getIconFont("FontAwesome");

    _openProjectWidget.getModel().removeRows(0, _recentProjectsWidget.getModel().rowCount());
    _openProjectWidget.getModel().add(fontAwesome.getIcon("file"), "Open project", "Open an existing project", "", "Browse to an existing project and open it", []() -> void {
        projects().openProject();
    });

    _recentProjectsAction.initialize("Manager/Project/Recent", "Project", "Ctrl", Application::getIconFont("FontAwesome").getIcon("file"));

    _recentProjectsWidget.getModel().removeRows(0, _recentProjectsWidget.getModel().rowCount());

    for (const auto& recentFile : _recentProjectsAction.getRecentFiles()) {
        const auto recentFilePath = recentFile.getFilePath();

        _recentProjectsWidget.getModel().add(fontAwesome.getIcon("file"), QFileInfo(recentFilePath).baseName(), recentFilePath, recentFile.getDateTime().toString("dd/MM/yyyy hh:mm"), QString("Open %1").arg(recentFilePath), [recentFilePath]() -> void {
            projects().openProject(recentFilePath);
        });
    }
        
}
