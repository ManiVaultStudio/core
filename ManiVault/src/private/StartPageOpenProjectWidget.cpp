// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "StartPageOpenProjectWidget.h"

#include "StartPageAction.h"
#include "StartPageContentWidget.h"

#include <Application.h>
#include <CoreInterface.h>
#include <ProjectMetaAction.h>

#include <util/Icon.h>

#include <QDebug>
#include <QPainter>

using namespace mv;
using namespace mv::util;
using namespace mv::gui;

StartPageOpenProjectWidget::StartPageOpenProjectWidget(StartPageContentWidget* startPageContentWidget) :
    QWidget(startPageContentWidget),
    _startPageContentWidget(startPageContentWidget),
    _openCreateProjectWidget(this, "Open & Create"),
    _recentProjectsWidget(this, "Recent"),
    _exampleProjectsWidget(this, "Examples"),
    _recentProjectsAction(this, mv::projects().getSettingsPrefix() + "RecentProjects"),
    _leftAlignedIcon(),
    _leftAlignedLoggingIcon(),
    _rightAlignedIcon(),
    _rightAlignedLoggingIcon()
{
    auto layout = new QVBoxLayout();

    
    layout->addWidget(&_openCreateProjectWidget);
    layout->addWidget(&_recentProjectsWidget);
    layout->addWidget(&_exampleProjectsWidget);

    setLayout(layout);

    _openCreateProjectWidget.getHierarchyWidget().getFilterColumnAction().setCurrentText("Title");
    _recentProjectsWidget.getHierarchyWidget().getFilterColumnAction().setCurrentText("Title");
    _exampleProjectsWidget.getHierarchyWidget().getFilterColumnAction().setCurrentText("Title");

    _openCreateProjectWidget.getHierarchyWidget().getFilterNameAction().setVisible(false);
    _openCreateProjectWidget.getHierarchyWidget().getFilterGroupAction().setVisible(false);
    _openCreateProjectWidget.getHierarchyWidget().setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    _recentProjectsWidget.getHierarchyWidget().setItemTypeName("Recent Project");
    _exampleProjectsWidget.getHierarchyWidget().setItemTypeName("Example Project");

    _recentProjectsWidget.getHierarchyWidget().getToolbarAction().addAction(&_recentProjectsAction);

    _recentProjectsAction.initialize("Manager/Project/Recent", "Project", "Ctrl", Application::getIconFont("FontAwesome").getIcon("file"));

    connect(&_recentProjectsAction, &RecentFilesAction::recentFilesChanged, this, &StartPageOpenProjectWidget::updateRecentActions);

    createCustomIcons();

    const auto toggleViews = [this]() -> void {
        _openCreateProjectWidget.setVisible(_startPageContentWidget->getToggleOpenCreateProjectAction().isChecked());
        _recentProjectsWidget.setVisible(_startPageContentWidget->getToggleRecentProjectsAction().isChecked());
        _exampleProjectsWidget.setVisible(_startPageContentWidget->getToggleExampleProjectsAction().isChecked());
    };

    connect(&_startPageContentWidget->getToggleOpenCreateProjectAction(), &ToggleAction::toggled, this, toggleViews);
    connect(&_startPageContentWidget->getToggleRecentProjectsAction(), &ToggleAction::toggled, this, toggleViews);
    connect(&_startPageContentWidget->getToggleExampleProjectsAction(), &ToggleAction::toggled, this, toggleViews);

    toggleViews();
    
    connect(qApp, &QApplication::paletteChanged, this, &StartPageOpenProjectWidget::updateCustomStyle);
}

void StartPageOpenProjectWidget::updateActions()
{
    updateOpenCreateActions();
    updateRecentActions();
    updateExamplesActions();
}

void StartPageOpenProjectWidget::createIconForDefaultProject(const Qt::Alignment& alignment, QIcon& icon, bool logging /*= false*/)
{
    const auto size             = 128.0;
    const auto margin           = 12.0;
    const auto spacing          = 14.0;
    const auto halfSpacing      = spacing / 2.0;
    const auto lineThickness    = 7.0;
    const auto offset           = 80.0;
    const auto loggingHeight    = logging ? 25.0 : 0.0;
    const auto halfSize         = logging ? margin + ((size - margin - margin - loggingHeight - spacing) / 2.0) : size / 2.0;

    QPixmap pixmap(size, size);

    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);

    painter.setWindow(0, 0, size, size);

    const auto drawWindow = [&](QRectF rectangle) -> void {
        painter.setBrush(qApp->palette().text().color());
        painter.setPen(Qt::NoPen);
        painter.drawRect(rectangle);
    };

    drawWindow(QRectF(QPointF(margin, margin), QPointF(offset - halfSpacing, size - margin - loggingHeight - (logging ? spacing : 0.0))));

    drawWindow(QRectF(QPointF(offset + halfSpacing, margin), QPointF(size - margin, halfSize - halfSpacing)));
    drawWindow(QRectF(QPointF(offset + halfSpacing, halfSize + halfSpacing), QPointF(size - margin, size - margin - loggingHeight - (logging ? spacing : 0.0))));

    if (logging)
        drawWindow(QRectF(QPointF(margin, size - margin - loggingHeight), QPointF(size - margin, size - margin)));

    QPixmap finalPixmap = pixmap;

    if (alignment == Qt::AlignLeft)
        finalPixmap = pixmap.transformed(QTransform().scale(-1, 1));

    icon = mv::gui::createIcon(finalPixmap);
}

void StartPageOpenProjectWidget::updateOpenCreateActions()
{
    auto& fontAwesome = Application::getIconFont("FontAwesome");

    _openCreateProjectWidget.getModel().reset();

    StartPageAction openProjectStartPageAction(fontAwesome.getIcon("folder-open"), "Open project", "Open an existing project", "Open an existing project", "Browse to an existing project and open it", []() -> void {
        projects().openProject();
    });

    openProjectStartPageAction.setComments("Use the file browser to navigate to the project and open it");

    _openCreateProjectWidget.getModel().add(openProjectStartPageAction);

    StartPageAction blankProjectStartPageAction(fontAwesome.getIcon("file"), "Blank", "Create project without plugins", "Create project without any plugins", "", []() -> void {
        projects().newProject();
    });

    StartPageAction rightAlignedProjectStartPageAction(_rightAlignedIcon, "Right-aligned", "Create project with standard plugins on the right", "Create project with data hierarchy and data properties plugins on the right", "", []() -> void {
        projects().newProject(Qt::AlignRight);
    });

    StartPageAction rightAlignedLoggingProjectStartPageAction(_rightAlignedLoggingIcon, "Right-aligned with logging", "Create project with standard plugins on the right and logging at the bottom", "Create project with data hierarchy and data properties plugins on the right and a logging plugin at the bottom", "", []() -> void {
        projects().newProject(Qt::AlignRight, true);
    });

    StartPageAction leftAlignedProjectStartPageAction(_leftAlignedIcon, "Left-aligned", "Create project with standard plugins on the left", "Create project with data hierarchy and data properties plugins on the left", "", []() -> void {
        projects().newProject(Qt::AlignLeft);
    });

    StartPageAction leftAlignedLoggingProjectStartPageAction(_leftAlignedLoggingIcon, "Left-aligned with logging", "Create project with standard plugins on the left and logging at the bottom", "Create project with data hierarchy and data properties plugins on the left and a logging plugin at the bottom", "", []() -> void {
        projects().newProject(Qt::AlignLeft, true);
    });

    //blankProjectStartPageAction.setComments(blankProjectStartPageAction.getDescription());
    //rightAlignedProjectStartPageAction.setComments(rightAlignedProjectStartPageAction.getDescription());
    //rightAlignedLoggingProjectStartPageAction.setComments(rightAlignedLoggingProjectStartPageAction.getDescription());
    //leftAlignedProjectStartPageAction.setComments(leftAlignedProjectStartPageAction.getDescription());
    //leftAlignedLoggingProjectStartPageAction.setComments(leftAlignedLoggingProjectStartPageAction.getDescription());

    _openCreateProjectWidget.getModel().add(blankProjectStartPageAction);
    _openCreateProjectWidget.getModel().add(rightAlignedProjectStartPageAction);
    _openCreateProjectWidget.getModel().add(rightAlignedLoggingProjectStartPageAction);
    _openCreateProjectWidget.getModel().add(leftAlignedProjectStartPageAction);
    _openCreateProjectWidget.getModel().add(leftAlignedLoggingProjectStartPageAction);
}

void StartPageOpenProjectWidget::updateRecentActions()
{
    _recentProjectsWidget.getModel().reset();

    auto clockIcon = Application::getIconFont("FontAwesome").getIcon("clock");

    qDebug() << "Looking for recent projects...";

    for (const auto& recentFile : _recentProjectsAction.getRecentFiles()) {
        const auto recentFilePath = recentFile.getFilePath();

        const auto projectMetaAction = Project::getProjectMetaActionFromProjectFilePath(recentFilePath);

        if (projectMetaAction.isNull()) {
            StartPageAction recentProjectStartPageAction(clockIcon, QFileInfo(recentFilePath).baseName(), recentFilePath, recentFilePath, "", [recentFilePath]() -> void {
                projects().openProject(recentFilePath);
            });

            _recentProjectsWidget.getModel().add(recentProjectStartPageAction);
        }
        else {
            qDebug() << "Found project: " << QFileInfo(recentFilePath).baseName();

            StartPageAction recentProjectStartPageAction(clockIcon, QFileInfo(recentFilePath).baseName(), recentFilePath, projectMetaAction->getDescriptionAction().getString(), "", [recentFilePath]() -> void {
                projects().openProject(recentFilePath);
            });

            recentProjectStartPageAction.setComments(projectMetaAction->getCommentsAction().getString());
            recentProjectStartPageAction.setTags(projectMetaAction->getTagsAction().getStrings());
            recentProjectStartPageAction.setMetaData(recentFile.getDateTime().toString("dd/MM/yyyy hh:mm"));
            recentProjectStartPageAction.setPreviewImage(projects().getWorkspacePreview(recentFilePath));
            recentProjectStartPageAction.setContributors(projectMetaAction->getContributorsAction().getStrings());

            _recentProjectsWidget.getModel().add(recentProjectStartPageAction);
        }
    }

}

void StartPageOpenProjectWidget::updateExamplesActions()
{
    _exampleProjectsWidget.getModel().reset();

    auto& fontAwesome = Application::getIconFont("FontAwesome");

    QStringList projectFilter("*.mv");

    QDir exampleProjectsDirectory(QString("%1/examples/projects").arg(qApp->applicationDirPath()));

    const auto exampleProjects = exampleProjectsDirectory.entryList(projectFilter);

    for (const auto& exampleProject : exampleProjects) {
        const auto exampleProjectFilePath = QString("%1/examples/projects/%2").arg(qApp->applicationDirPath(), exampleProject);

        const auto projectMetaAction = Project::getProjectMetaActionFromProjectFilePath(exampleProjectFilePath);

        if (projectMetaAction.isNull()) {
            StartPageAction exampleProjectStartPageAction(fontAwesome.getIcon("file-prescription"), QFileInfo(exampleProjectFilePath).baseName(), exampleProjectFilePath, exampleProjectFilePath, "", [exampleProjectFilePath]() -> void {
                projects().openProject(exampleProjectFilePath);
            });

            _exampleProjectsWidget.getModel().add(exampleProjectStartPageAction);
        }
        else {
            StartPageAction exampleProjectStartPageAction(fontAwesome.getIcon("file-prescription"), projectMetaAction->getTitleAction().getString(), exampleProjectFilePath, projectMetaAction->getDescriptionAction().getString(), "", [exampleProjectFilePath]() -> void {
                projects().openProject(exampleProjectFilePath);
            });

            exampleProjectStartPageAction.setComments(projectMetaAction->getCommentsAction().getString());
            exampleProjectStartPageAction.setTags(projectMetaAction->getTagsAction().getStrings());
            exampleProjectStartPageAction.setPreviewImage(projects().getWorkspacePreview(exampleProjectFilePath));
            exampleProjectStartPageAction.setContributors(projectMetaAction->getContributorsAction().getStrings());

            _exampleProjectsWidget.getModel().add(exampleProjectStartPageAction);
        }
    }
}

void StartPageOpenProjectWidget::createCustomIcons()
{
    createIconForDefaultProject(Qt::AlignLeft, _leftAlignedIcon);
    createIconForDefaultProject(Qt::AlignLeft, _leftAlignedLoggingIcon, true);
    createIconForDefaultProject(Qt::AlignRight, _rightAlignedIcon);
    createIconForDefaultProject(Qt::AlignRight, _rightAlignedLoggingIcon, true);
}

void StartPageOpenProjectWidget::updateCustomStyle()
{
    createCustomIcons();
    updateActions();
}
