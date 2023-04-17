#include "StartPageOpenProjectWidget.h"
#include "StartPageContentWidget.h"
#include "Archiver.h"

#include <Application.h>
#include <CoreInterface.h>

#include <util/Icon.h>

#include <QDebug>
#include <QPainter>

using namespace hdps;
using namespace hdps::util;
using namespace hdps::gui;

StartPageOpenProjectWidget::StartPageOpenProjectWidget(StartPageContentWidget* startPageContentWidget) :
    QWidget(startPageContentWidget),
    _startPageContentWidget(startPageContentWidget),
    _openCreateProjectWidget(this, "Open & Create"),
    _recentProjectsWidget(this, "Recent"),
    _exampleProjectsWidget(this, "Examples"),
    _recentProjectsAction(this),
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

    _openCreateProjectWidget.getHierarchyWidget().getFilterNameAction().setVisible(false);
    _openCreateProjectWidget.getHierarchyWidget().getFilterGroupAction().setVisible(false);
    _openCreateProjectWidget.getHierarchyWidget().setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    _recentProjectsWidget.getHierarchyWidget().setItemTypeName("Recent Project");
    _exampleProjectsWidget.getHierarchyWidget().setItemTypeName("Example Project");

    _recentProjectsWidget.getHierarchyWidget().getToolbarLayout().addWidget(_recentProjectsAction.getEditAction().createWidget(this, TriggerAction::Icon));

    _recentProjectsAction.initialize("Manager/Project/Recent", "Project", "Ctrl", Application::getIconFont("FontAwesome").getIcon("file"));

    connect(&_recentProjectsAction, &RecentFilesAction::recentFilesChanged, this, &StartPageOpenProjectWidget::updateRecentActions);

    createIconForDefaultProject(Qt::AlignLeft, _leftAlignedIcon);
    createIconForDefaultProject(Qt::AlignLeft, _leftAlignedLoggingIcon, true);
    createIconForDefaultProject(Qt::AlignRight, _rightAlignedIcon);
    createIconForDefaultProject(Qt::AlignRight, _rightAlignedLoggingIcon, true);

    const auto toggleViews = [this]() -> void {
        _openCreateProjectWidget.setVisible(_startPageContentWidget->getToggleOpenCreateProjectAction().isChecked());
        _recentProjectsWidget.setVisible(_startPageContentWidget->getToggleRecentProjectsAction().isChecked());
        _exampleProjectsWidget.setVisible(_startPageContentWidget->getToggleExampleProjectsAction().isChecked());
    };

    connect(&_startPageContentWidget->getToggleOpenCreateProjectAction(), &ToggleAction::toggled, this, toggleViews);
    connect(&_startPageContentWidget->getToggleRecentProjectsAction(), &ToggleAction::toggled, this, toggleViews);
    connect(&_startPageContentWidget->getToggleExampleProjectsAction(), &ToggleAction::toggled, this, toggleViews);

    toggleViews();
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
        painter.setBrush(Qt::black);
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

    icon = hdps::gui::createIcon(finalPixmap);
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

    auto& fontAwesome = Application::getIconFont("FontAwesome");

    for (const auto& recentFile : _recentProjectsAction.getRecentFiles()) {
        const auto recentFilePath = recentFile.getFilePath();

        QTemporaryDir temporaryDir;

        const auto recentProjectJsonFilePath = projects().extractProjectFileFromHdpsFile(recentFilePath, temporaryDir);

        Project project(recentProjectJsonFilePath, true);

        StartPageAction recentProjectStartPageAction(fontAwesome.getIcon("clock"), QFileInfo(recentFilePath).baseName(), recentFilePath, project.getDescriptionAction().getString(), "", [recentFilePath]() -> void {
            projects().openProject(recentFilePath);
        });

        recentProjectStartPageAction.setComments(project.getCommentsAction().getString());
        recentProjectStartPageAction.setTags(project.getTagsAction().getStrings());
        recentProjectStartPageAction.setMetaData(recentFile.getDateTime().toString("dd/MM/yyyy hh:mm"));
        recentProjectStartPageAction.setPreviewImage(projects().getPreviewImage(recentFilePath));
        recentProjectStartPageAction.setContributors(project.getContributorsAction().getStrings());

        _recentProjectsWidget.getModel().add(recentProjectStartPageAction);
    }
}

void StartPageOpenProjectWidget::updateExamplesActions()
{
    _exampleProjectsWidget.getModel().reset();

    auto& fontAwesome = Application::getIconFont("FontAwesome");

    QStringList projectFilter("*.hdps");

    QDir exampleProjectsDirectory(QString("%1/examples/projects").arg(qApp->applicationDirPath()));

    const auto exampleProjects = exampleProjectsDirectory.entryList(projectFilter);

    for (const auto& exampleProject : exampleProjects) {
        const auto exampleProjectFilePath = QString("%1/examples/projects/%2").arg(qApp->applicationDirPath(), exampleProject);

        QTemporaryDir temporaryDir;

        const auto exampleProjectJsonFilePath = projects().extractProjectFileFromHdpsFile(exampleProjectFilePath, temporaryDir);

        Project project(exampleProjectJsonFilePath, true);

        StartPageAction exampleProjectStartPageAction(fontAwesome.getIcon("file-prescription"), project.getTitleAction().getString(), exampleProjectFilePath, project.getDescriptionAction().getString(), "", [exampleProjectFilePath]() -> void {
            projects().openProject(exampleProjectFilePath);
        });

        exampleProjectStartPageAction.setComments(project.getCommentsAction().getString());
        exampleProjectStartPageAction.setTags(project.getTagsAction().getStrings());
        exampleProjectStartPageAction.setPreviewImage(projects().getPreviewImage(exampleProjectFilePath));
        exampleProjectStartPageAction.setContributors(project.getContributorsAction().getStrings());

        _exampleProjectsWidget.getModel().add(exampleProjectStartPageAction);
    }
}
