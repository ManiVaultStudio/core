// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "StartPageOpenProjectWidget.h"

#include "PageAction.h"
#include "StartPageContentWidget.h"

#include <Application.h>
#include <CoreInterface.h>
#include <ProjectMetaAction.h>

#include <util/StyledIcon.h>

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
    _projectCenterWidget(this, "Projects"),
    _recentProjectsAction(this, mv::projects().getSettingsPrefix() + "RecentProjects"),
    _projectCenterSettingsAction(this, "Projects source")
{
    auto layout = new QVBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(&_openCreateProjectWidget);
    layout->addWidget(&_recentProjectsWidget);
    layout->addWidget(&_projectCenterWidget);

    setLayout(layout);

    _openCreateProjectWidget.getHierarchyWidget().getFilterColumnAction().setCurrentText("Title");
    _recentProjectsWidget.getHierarchyWidget().getFilterColumnAction().setCurrentText("Title");
    _projectCenterWidget.getHierarchyWidget().getFilterColumnAction().setCurrentText("Title");

    _openCreateProjectWidget.getHierarchyWidget().getFilterNameAction().setVisible(false);
    _openCreateProjectWidget.getHierarchyWidget().getFilterGroupAction().setVisible(false);
    _openCreateProjectWidget.getHierarchyWidget().setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    _recentProjectsWidget.getHierarchyWidget().setItemTypeName("Recent Project");
    _recentProjectsWidget.getHierarchyWidget().getToolbarAction().addAction(&_recentProjectsAction);
    _recentProjectsAction.initialize("Manager/Project/Recent", "Project", "Ctrl");

    _projectCenterSettingsAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);
    _projectCenterSettingsAction.addAction(&mv::projects().getProjectCenterSourceUrlAction());
    _projectCenterSettingsAction.setIconByName("globe");
    _projectCenterSettingsAction.setPopupSizeHint(QSize(400, 10));

    _projectCenterWidget.getHierarchyWidget().setItemTypeName("Project");
    _projectCenterWidget.getHierarchyWidget().getToolbarAction().addAction(&_projectCenterSettingsAction);

    _projectCenterFilterModel.setSourceModel(const_cast<ProjectCenterModel*>(&mv::projects().getProjectCenterModel()));

    connect(&mv::projects().getProjectCenterModel(), &ProjectCenterModel::populatedFromSourceUrl, this, &StartPageOpenProjectWidget::updateProjectCenterActions);

    connect(&_recentProjectsAction, &RecentFilesAction::recentFilesChanged, this, &StartPageOpenProjectWidget::updateRecentActions);

    createCustomIcons();

    const auto toggleViews = [this]() -> void {
        _openCreateProjectWidget.setVisible(_startPageContentWidget->getToggleOpenCreateProjectAction().isChecked());
        _recentProjectsWidget.setVisible(_startPageContentWidget->getToggleRecentProjectsAction().isChecked());
    };

    connect(&_startPageContentWidget->getToggleOpenCreateProjectAction(), &ToggleAction::toggled, this, toggleViews);
    connect(&_startPageContentWidget->getToggleRecentProjectsAction(), &ToggleAction::toggled, this, toggleViews);

    toggleViews();
}

bool StartPageOpenProjectWidget::event(QEvent* event)
{
    if (event->type() == QEvent::ApplicationPaletteChange)
        updateCustomStyle();

    return QWidget::event(event);
}

void StartPageOpenProjectWidget::updateActions()
{
    updateOpenCreateActions();
    updateRecentActions();
}

void StartPageOpenProjectWidget::createIconForDefaultProject(const Qt::Alignment& alignment, QIcon& icon, bool logging /*= false*/)
{
    const auto size             = 128.0;
    const auto margin           = 12.0;
    const auto spacing          = 14.0;
    const auto halfSpacing      = spacing / 2.0;
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
    _openCreateProjectWidget.getModel().reset();

    PageAction openProjectPageAction(StyledIcon("folder-open"), "Open project", "Open an existing project", "Open an existing project", "Browse to an existing project and open it", []() -> void {
        projects().openProject();
    });

    openProjectPageAction.setComments("Use the file browser to navigate to the project and open it");

    _openCreateProjectWidget.getModel().add(openProjectPageAction);

    PageAction blankProjectPageAction(StyledIcon("file"), "Blank", "Create project without plugins", "Create project without any plugins", "", []() -> void {
        projects().newProject();
    });

    PageAction rightAlignedProjectPageAction(_rightAlignedIcon, "Right-aligned", "Create project with standard plugins on the right", "Create project with data hierarchy and data properties plugins on the right", "", []() -> void {
        projects().newProject(Qt::AlignRight);
    });

    PageAction rightAlignedLoggingProjectPageAction(_rightAlignedLoggingIcon, "Right-aligned with logging", "Create project with standard plugins on the right and logging at the bottom", "Create project with data hierarchy and data properties plugins on the right and a logging plugin at the bottom", "", []() -> void {
        projects().newProject(Qt::AlignRight, true);
    });

    PageAction leftAlignedProjectPageAction(_leftAlignedIcon, "Left-aligned", "Create project with standard plugins on the left", "Create project with data hierarchy and data properties plugins on the left", "", []() -> void {
        projects().newProject(Qt::AlignLeft);
    });

    PageAction leftAlignedLoggingProjectPageAction(_leftAlignedLoggingIcon, "Left-aligned with logging", "Create project with standard plugins on the left and logging at the bottom", "Create project with data hierarchy and data properties plugins on the left and a logging plugin at the bottom", "", []() -> void {
        projects().newProject(Qt::AlignLeft, true);
    });

    //blankProjectPageAction.setComments(blankProjectPageAction.getDescription());
    //rightAlignedProjectPageAction.setComments(rightAlignedProjectPageAction.getDescription());
    //rightAlignedLoggingProjectPageAction.setComments(rightAlignedLoggingProjectPageAction.getDescription());
    //leftAlignedProjectPageAction.setComments(leftAlignedProjectPageAction.getDescription());
    //leftAlignedLoggingProjectPageAction.setComments(leftAlignedLoggingProjectPageAction.getDescription());

    _openCreateProjectWidget.getModel().add(blankProjectPageAction);
    _openCreateProjectWidget.getModel().add(rightAlignedProjectPageAction);
    _openCreateProjectWidget.getModel().add(rightAlignedLoggingProjectPageAction);
    _openCreateProjectWidget.getModel().add(leftAlignedProjectPageAction);
    _openCreateProjectWidget.getModel().add(leftAlignedLoggingProjectPageAction);
}

void StartPageOpenProjectWidget::updateRecentActions()
{
    _recentProjectsWidget.getModel().reset();

    auto clockIcon = StyledIcon("clock");

    const auto recentFiles = _recentProjectsAction.getRecentFiles();

    if(!recentFiles.isEmpty())
        qDebug() << "Looking for recent projects...";

    for (const auto& recentFile : recentFiles) {
        const auto recentFilePath = recentFile.getFilePath();

        const auto projectMetaAction = Project::getProjectMetaActionFromProjectFilePath(recentFilePath);

        if (projectMetaAction.isNull()) {
            PageAction recentProjectPageAction(clockIcon, QFileInfo(recentFilePath).baseName(), recentFilePath, recentFilePath, "", [recentFilePath]() -> void {
                projects().openProject(recentFilePath);
            });

            _recentProjectsWidget.getModel().add(recentProjectPageAction);
        }
        else {
            qDebug() << "Found project: " << QFileInfo(recentFilePath).baseName();

            PageAction recentProjectPageAction(clockIcon, QFileInfo(recentFilePath).baseName(), recentFilePath, projectMetaAction->getDescriptionAction().getString(), "", [recentFilePath]() -> void {
                projects().openProject(recentFilePath);
            });

            recentProjectPageAction.setComments(projectMetaAction->getCommentsAction().getString());
            recentProjectPageAction.setTags(projectMetaAction->getTagsAction().getStrings());
            recentProjectPageAction.setMetaData(recentFile.getDateTime().toString("dd/MM/yyyy hh:mm"));
            recentProjectPageAction.setPreviewImage(projects().getWorkspacePreview(recentFilePath));
            recentProjectPageAction.setContributors(projectMetaAction->getContributorsAction().getStrings());

            _recentProjectsWidget.getModel().add(recentProjectPageAction);
        }
    }
}

void StartPageOpenProjectWidget::updateProjectCenterActions()
{
    _projectCenterWidget.getModel().reset();
    
    const auto& projectCenterModel = mv::projects().getProjectCenterModel();

    for (int filterRowIndex = 0; _projectCenterFilterModel.rowCount() > filterRowIndex; ++filterRowIndex) {
        const auto filterIndex = _projectCenterFilterModel.index(filterRowIndex, 0);
        const auto sourceIndex = _projectCenterFilterModel.mapToSource(filterIndex);

    	if (sourceIndex.isValid()) {
            if (const auto project = projectCenterModel.getProject(sourceIndex)) {
                PageAction recentProjectPageAction(StyledIcon("file"), project->getTitle(), project->getUrl().toString(), project->getSummary(), "", [project]() -> void {
                    projects().openProject(project->getUrl());
				});

                recentProjectPageAction.setTags(project->getTags());
                //recentProjectPageAction.setMetaData(project->getDate().toString("dd/MM/yyyy hh:mm"));

            	_projectCenterWidget.getModel().add(recentProjectPageAction);
            }
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
