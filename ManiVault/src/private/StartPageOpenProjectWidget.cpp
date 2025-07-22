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
#include <util/Icon.h>

#include <models/ProjectsTreeModel.h>

#include <QDebug>
#include <QPainter>
#include <QStandardPaths>
#include <QPromise>
#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrent>

using namespace mv;
using namespace mv::util;
using namespace mv::gui;

StartPageOpenProjectWidget::StartPageOpenProjectWidget(StartPageContentWidget* startPageContentWidget) :
    QWidget(startPageContentWidget),
    Serializable("OpenProjectWidget"),
    _startPageContentWidget(startPageContentWidget),
    _openCreateProjectWidget(this, "Open & Create"),
    _recentProjectsWidget(this, "Recent"),
    _projectsWidget(this, "Projects"),
    _projectsSettingsAction(this, "Data Source Names")
{
    _projectsWidget.hide();

    auto layout = new QVBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(&_openCreateProjectWidget);
    layout->addWidget(&_recentProjectsWidget);

    layout->addWidget(&_projectsWidget);

    _projectsSettingsAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);
    _projectsSettingsAction.setShowLabels(false);
    _projectsSettingsAction.setIconByName("globe");
    _projectsSettingsAction.setPopupSizeHint(QSize(550, 100));

    _projectsSettingsAction.addAction(const_cast<StringsAction*>(&mv::projects().getProjectsTreeModel().getDsnsAction()));

    _projectsWidget.getHierarchyWidget().setItemTypeName("Project");

    auto& toolbarAction = _projectsWidget.getHierarchyWidget().getToolbarAction();

    toolbarAction.addAction(&_projectsSettingsAction);
    toolbarAction.addAction(&_projectsFilterModel.getFilterGroupAction());
    toolbarAction.addAction(&_projectsFilterModel.getTagsFilterAction());

    _projectsFilterModel.setSourceModel(const_cast<ProjectsTreeModel*>(&mv::projects().getProjectsTreeModel()));

    setupProjectsModelSection();
    
    setLayout(layout);

    _openCreateProjectWidget.getHierarchyWidget().getFilterColumnAction().setCurrentText("Title");

    _openCreateProjectWidget.getHierarchyWidget().getFilterNameAction().setVisible(false);
    _openCreateProjectWidget.getHierarchyWidget().getFilterGroupAction().setVisible(false);
    _openCreateProjectWidget.getHierarchyWidget().setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    createCustomIcons();

    const auto toggleViews = [this]() -> void {
        _openCreateProjectWidget.setVisible(_startPageContentWidget->getToggleOpenCreateProjectAction().isChecked());
        _projectsWidget.setVisible(_startPageContentWidget->getToggleProjectDatabaseAction().isChecked());
        _recentProjectsWidget.setVisible(_startPageContentWidget->getToggleRecentProjectsAction().isChecked());
    };

    connect(&_startPageContentWidget->getToggleOpenCreateProjectAction(), &ToggleAction::toggled, this, toggleViews);
    connect(&_startPageContentWidget->getToggleProjectDatabaseAction(), &ToggleAction::toggled, this, toggleViews);
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
    setupRecentProjectsSection();
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

    auto openProjectPageAction = std::make_shared<PageAction>(StyledIcon("folder-open"), "Open project", "Open an existing project", "Open an existing project", "Browse to an existing project and open it", []() -> void {
        projects().openProject();
    });

    openProjectPageAction->setComments("Use the file browser to navigate to the project and open it");

    _openCreateProjectWidget.getModel().add(openProjectPageAction);

    auto blankProjectPageAction = std::make_shared<PageAction>(StyledIcon("file"), "Blank", "Create project without plugins", "Create project without any plugins", "", []() -> void {
        projects().newProject();
    });

    auto rightAlignedProjectPageAction = std::make_shared<PageAction>(_rightAlignedIcon, "Right-aligned", "Create project with standard plugins on the right", "Create project with data hierarchy and data properties plugins on the right", "", []() -> void {
        projects().newProject(Qt::AlignRight);
    });

    auto rightAlignedLoggingProjectPageAction = std::make_shared<PageAction>(_rightAlignedLoggingIcon, "Right-aligned with logging", "Create project with standard plugins on the right and logging at the bottom", "Create project with data hierarchy and data properties plugins on the right and a logging plugin at the bottom", "", []() -> void {
        projects().newProject(Qt::AlignRight, true);
    });

    auto leftAlignedProjectPageAction = std::make_shared<PageAction>(_leftAlignedIcon, "Left-aligned", "Create project with standard plugins on the left", "Create project with data hierarchy and data properties plugins on the left", "", []() -> void {
        projects().newProject(Qt::AlignLeft);
    });

    auto leftAlignedLoggingProjectPageAction = std::make_shared<PageAction>(_leftAlignedLoggingIcon, "Left-aligned with logging", "Create project with standard plugins on the left and logging at the bottom", "Create project with data hierarchy and data properties plugins on the left and a logging plugin at the bottom", "", []() -> void {
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

void StartPageOpenProjectWidget::setupRecentProjectsSection()
{
    auto& recentProjectsAction      = mv::projects().getRecentProjectsAction();
    auto& recentProjectsActionModel = static_cast<const RecentFilesAction&>(recentProjectsAction).getModel();
    auto& hierarchyWidget           = _recentProjectsWidget.getHierarchyWidget();

	hierarchyWidget.getFilterColumnAction().setCurrentText("FilePath");
	hierarchyWidget.setItemTypeName("Recent Project");
	hierarchyWidget.getToolbarAction().addAction(&recentProjectsAction);
	hierarchyWidget.setWindowIcon(StyledIcon("clock"));

    auto& toolbarAction = hierarchyWidget.getToolbarAction();

    toolbarAction.removeAllActions();
    toolbarAction.addAction(&_recentProjectsFilterModel.getTextFilterAction());
    toolbarAction.addAction(&recentProjectsAction.getEditAction());

	_recentProjectsFilterModel.setSourceModel(const_cast<RecentFilesListModel*>(&recentProjectsActionModel));

    const auto populateModel = [this, &recentProjectsAction, &recentProjectsActionModel]() -> void {
        _recentProjectsWidget.getModel().removeRows(0, _recentProjectsWidget.getModel().rowCount());

        auto clockIcon = StyledIcon("clock");

        for (int filterRowIndex = 0; filterRowIndex < _recentProjectsFilterModel.rowCount(); ++filterRowIndex) {
            const auto sourceIndex      = _recentProjectsFilterModel.mapToSource(_recentProjectsFilterModel.index(filterRowIndex, 0));
            const auto recentFile       = recentProjectsActionModel.getRecentFileFromIndex(sourceIndex);
            const auto recentFilePath   = recentFile.getFilePath();

            auto recentProjectPageAction = std::make_shared<PageAction>(clockIcon, QFileInfo(recentFilePath).baseName(), recentFilePath, "", "", [recentFilePath]() -> void {
                projects().openProject(recentFilePath);
            });

            recentProjectPageAction->setMetaData(recentFile.getDateTime().toString("dd/MM/yyyy hh:mm"));
            recentProjectPageAction->setPreviewImage(projects().getWorkspacePreview(recentFilePath));

            _recentProjectsWidget.getModel().add(recentProjectPageAction);

            auto future = QtConcurrent::run([recentFilePath]() -> QSharedPointer<ProjectMetaAction> {
                return Project::getProjectMetaActionFromProjectFilePath(recentFilePath);
			});

            auto* watcher = new QFutureWatcher<QSharedPointer<ProjectMetaAction>>();

            QObject::connect(watcher, &QFutureWatcher<QSharedPointer<ProjectMetaAction>>::finished, [watcher, recentProjectPageAction]() {
                const auto result = watcher->future().result();

                if (const auto projectMetaAction = watcher->future().result()) {
                    recentProjectPageAction->setDescription(projectMetaAction->getDescriptionAction().getString());
                    recentProjectPageAction->setComments(projectMetaAction->getCommentsAction().getString());
                    recentProjectPageAction->setTags(projectMetaAction->getTagsAction().getStrings());
                    recentProjectPageAction->setContributors(projectMetaAction->getContributorsAction().getStrings());
                }

                watcher->deleteLater();
			});

            watcher->setFuture(future);
        }
    };

    populateModel();

    connect(&_recentProjectsFilterModel, &RecentFilesFilterModel::layoutChanged, this, populateModel);
}

void StartPageOpenProjectWidget::setupProjectsModelSection()
{
    auto& hierarchyWidget = _projectsWidget.getHierarchyWidget();

	hierarchyWidget.getFilterColumnAction().setCurrentText("Title");
    hierarchyWidget.setItemTypeName("Project");
    hierarchyWidget.setWindowIcon(StyledIcon("file"));
    hierarchyWidget.getFilterColumnAction().setCurrentText("Title");

    auto& projectsTreeModel     = mv::projects().getProjectsTreeModel();
    auto& projectsPageTreeModel = _projectsWidget.getModel();

    const auto populateModel = [this, &projectsTreeModel, &projectsPageTreeModel]() -> void {
        projectsPageTreeModel.removeRows(0, projectsPageTreeModel.rowCount());

        for (int filterRowIndex = 0; filterRowIndex <= _projectsFilterModel.rowCount(); ++filterRowIndex) {
            const auto filterIndex = _projectsFilterModel.index(filterRowIndex, 0);
	        const auto sourceIndex = _projectsFilterModel.mapToSource(filterIndex);

        	if (!sourceIndex.isValid())
                continue;
            
            if (const auto project = projectsTreeModel.getProject(sourceIndex)) {
                auto projectPageAction = std::make_shared<PageAction>(StyledIcon("file"), project->getTitle(), project->getUrl().toString(), project->getSummary(), "", [project]() -> void {
                    projects().openProject(project->getUrl());
                });

                projectPageAction->setParentTitle(project->getGroup());
                projectPageAction->setTags(project->getTags());
                projectPageAction->createSubAction<PageCommentsSubAction>(sourceIndex);

                projectsPageTreeModel.add(projectPageAction);
            }

            const auto numberOfChildren = _projectsFilterModel.rowCount(filterIndex);

            if (numberOfChildren >= 1) {
                for (int childFilterRowIndex = 0; childFilterRowIndex <= numberOfChildren; ++childFilterRowIndex) {
                    const auto childFilterIndex = _projectsFilterModel.index(childFilterRowIndex, 0, filterIndex);
                    const auto childSourceIndex = _projectsFilterModel.mapToSource(childFilterIndex);

                    if (const auto project = projectsTreeModel.getProject(childSourceIndex)) {
                        auto projectPageAction = std::make_shared<PageAction>(StyledIcon("file"), project->getTitle(), project->getUrl().toString(), project->getSummary(), "", [project]() -> void {
                            projects().openProject(project->getUrl());
                        });

                        projectPageAction->setParentTitle(project->getGroup());
                        projectPageAction->setTags(project->getTags());
                        projectPageAction->createSubAction<PageCommentsSubAction>(sourceIndex);

                        projectsPageTreeModel.add(projectPageAction);
                    }
                }
            }
        }
    };

    populateModel();

    connect(&_projectsFilterModel, &ProjectsFilterModel::layoutChanged, this, populateModel);
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

void StartPageOpenProjectWidget::fromVariantMap(const QVariantMap& variantMap)
{
	Serializable::fromVariantMap(variantMap);

    _openCreateProjectWidget.fromParentVariantMap(variantMap);
    _recentProjectsWidget.fromParentVariantMap(variantMap);
    _projectsWidget.fromParentVariantMap(variantMap);
    _projectsFilterModel.fromParentVariantMap(variantMap);
}

QVariantMap StartPageOpenProjectWidget::toVariantMap() const
{
	auto variantMap = Serializable::toVariantMap();

    _openCreateProjectWidget.insertIntoVariantMap(variantMap);
    _recentProjectsWidget.insertIntoVariantMap(variantMap);
    _projectsWidget.insertIntoVariantMap(variantMap);
    _projectsFilterModel.insertIntoVariantMap(variantMap);

    return variantMap;
}
