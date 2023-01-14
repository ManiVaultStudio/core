#include "StartPageOpenProjectWidget.h"
#include "StartPageActionDelegate.h"
#include "StartPageContentWidget.h"

#include <Application.h>

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
    layout->addWidget(&_recentProjectsWidget);

    layout->addStretch(1);

    setLayout(layout);

    _openProjectWidget.getHierarchyWidget().getFilterNameAction().setVisible(false);
    _openProjectWidget.getHierarchyWidget().getFilterGroupAction().setVisible(false);
    _openProjectWidget.getHierarchyWidget().setFixedHeight(40);

    //qDebug() << _openProjectWidget.getHierarchyWidget().getTreeView().rowHeight(_openProjectWidget.getModel().index(0, 0));

    _recentProjectsWidget.getHierarchyWidget().getFilterGroupAction().setVisible(true);
}

void StartPageOpenProjectWidget::updateActions()
{
    auto& fontAwesome = Application::getIconFont("FontAwesome");

    _openProjectWidget.getModel().removeRows(0, _recentProjectsWidget.getModel().rowCount());
    _openProjectWidget.getModel().add(fontAwesome.getIcon("file"), "Open project", "Browse to an existing project and open it");

    _recentProjectsAction.initialize("Manager/Project/Recent", "Project", "Ctrl", Application::getIconFont("FontAwesome").getIcon("file"));

    _recentProjectsWidget.getModel().removeRows(0, _recentProjectsWidget.getModel().rowCount());

    for (const auto& recentFilePath : _recentProjectsAction.getRecentFilePaths())
        _recentProjectsWidget.getModel().add(fontAwesome.getIcon("file"), QFileInfo(recentFilePath).baseName(), recentFilePath);
}
