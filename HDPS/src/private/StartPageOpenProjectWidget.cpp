#include "StartPageOpenProjectWidget.h"
#include "StartPageContentWidget.h"
#include "Archiver.h"

#include <Application.h>
#include <CoreInterface.h>

#include <util/icon.h>

#include <QDebug>
#include <QPainter>

using namespace hdps;
using namespace hdps::util;

StartPageOpenProjectWidget::StartPageOpenProjectWidget(QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _openCreateProjectWidget(this),
    _recentProjectsWidget(this),
    _exampleProjectsWidget(this),
    _recentProjectsAction(this),
    _leftAlignedIcon(),
    _rightAlignedIcon()
{
    auto layout = new QVBoxLayout();

    layout->addWidget(StartPageContentWidget::createHeaderLabel("Open & Create", "Open existing project and create new project"));
    layout->addWidget(&_openCreateProjectWidget);

    layout->addWidget(StartPageContentWidget::createHeaderLabel("Recent", "Recently opened project"));
    layout->addWidget(&_recentProjectsWidget);

    layout->addWidget(StartPageContentWidget::createHeaderLabel("Examples", "Open example project"));
    layout->addWidget(&_exampleProjectsWidget);

    layout->addStretch(1);

    setLayout(layout);

    _openCreateProjectWidget.getHierarchyWidget().getFilterNameAction().setVisible(false);
    _openCreateProjectWidget.getHierarchyWidget().getFilterGroupAction().setVisible(false);
    _openCreateProjectWidget.getHierarchyWidget().setFixedHeight(129);
    _openCreateProjectWidget.getHierarchyWidget().setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    _recentProjectsWidget.getHierarchyWidget().setItemTypeName("Recent Project");
    _exampleProjectsWidget.getHierarchyWidget().setItemTypeName("Example Project");

    createIconForDefaultProject(Qt::AlignLeft, _leftAlignedIcon);
    createIconForDefaultProject(Qt::AlignRight, _rightAlignedIcon);
}

void StartPageOpenProjectWidget::updateActions()
{
    auto& fontAwesome = Application::getIconFont("FontAwesome");

    _openCreateProjectWidget.getModel().reset();
    _recentProjectsWidget.getModel().reset();
    _exampleProjectsWidget.getModel().reset();

    StartPageAction openProjectStartPageAction(fontAwesome.getIcon("folder-open"), "Open project", "Open an existing project", "Open an existing project", "Browse to an existing project and open it", []() -> void {
        projects().openProject();
    });

    openProjectStartPageAction.setSubtitle("Open an existing project");

    _openCreateProjectWidget.getModel().add(openProjectStartPageAction);

    StartPageAction leftAlignedProjectStartPageAction(_leftAlignedIcon, "Left-aligned project", "Create project with standard plugins on the left", "Create project with data hierarchy and data properties plugins on the left", "", []() -> void {
        projects().newProject(Qt::AlignLeft);
    });

    StartPageAction rightAlignedProjectStartPageAction(_rightAlignedIcon, "Right-aligned project", "Create project with standard plugins on the right", "Create project with data hierarchy and data properties plugins on the right", "", []() -> void {
        projects().newProject(Qt::AlignRight);
    });

    leftAlignedProjectStartPageAction.setComments(leftAlignedProjectStartPageAction.getDescription());
    rightAlignedProjectStartPageAction.setComments(rightAlignedProjectStartPageAction.getDescription());

    _openCreateProjectWidget.getModel().add(leftAlignedProjectStartPageAction);
    _openCreateProjectWidget.getModel().add(rightAlignedProjectStartPageAction);

    _recentProjectsAction.initialize("Manager/Project/Recent", "Project", "Ctrl", Application::getIconFont("FontAwesome").getIcon("file"));

    for (const auto& recentFile : _recentProjectsAction.getRecentFiles()) {
        const auto recentFilePath = recentFile.getFilePath();
        
        QTemporaryDir temporaryDir;

        const auto recentProjectJsonFilePath = projects().extractProjectFileFromHdpsFile(recentFilePath, temporaryDir);

        Project project(recentProjectJsonFilePath);

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

    QStringList projectFilter("*.hdps");

    QDir exampleProjectsDirectory(QString("%1/examples/projects").arg(qApp->applicationDirPath()));

    const auto exampleProjects = exampleProjectsDirectory.entryList(projectFilter);

    for (const auto& exampleProject : exampleProjects) {
        const auto exampleProjectFilePath = QString("%1/examples/projects/%2").arg(qApp->applicationDirPath(), exampleProject);

        QTemporaryDir temporaryDir;

        const auto exampleProjectJsonFilePath = projects().extractProjectFileFromHdpsFile(exampleProjectFilePath, temporaryDir);

        Project project(exampleProjectJsonFilePath);

        StartPageAction exampleProjectStartPageAction(fontAwesome.getIcon("file-prescription"), project.getTitleAction().getString(), exampleProjectFilePath, project.getDescriptionAction().getString(), "", [exampleProjectFilePath]() -> void {
            projects().openProject(exampleProjectFilePath);
        });

        exampleProjectStartPageAction.setPreviewImage(projects().getPreviewImage(exampleProjectFilePath));
        exampleProjectStartPageAction.setTags(project.getTagsAction().getStrings());

        _exampleProjectsWidget.getModel().add(exampleProjectStartPageAction);
    }
}

void StartPageOpenProjectWidget::createIconForDefaultProject(const Qt::Alignment& alignment, QIcon& icon)
{
    const auto size             = 128.0;
    const auto halfSize         = size / 2.0;
    const auto margin           = 12.0;
    const auto spacing          = 14.0;
    const auto halfSpacing      = spacing / 2.0;
    const auto lineThickness    = 7.0;
    const auto offset           = 80.0;

    QPixmap pixmap(size, size);

    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);

    painter.setWindow(0, 0, size, size);

    const auto drawWindow = [&](QRectF rectangle) -> void {
        painter.setBrush(Qt::black);
        painter.setPen(Qt::NoPen);
        painter.drawRect(rectangle);
    };

    drawWindow(QRectF(QPointF(margin, margin), QPointF(offset - halfSpacing, size - margin)));
    drawWindow(QRectF(QPointF(offset + halfSpacing, margin), QPointF(size - margin, halfSize - halfSpacing)));
    drawWindow(QRectF(QPointF(offset + halfSpacing, halfSize + halfSpacing), QPointF(size - margin, size - margin)));

    QPixmap finalPixmap = pixmap;

    if (alignment == Qt::AlignLeft)
        finalPixmap = pixmap.transformed(QTransform().scale(-1, 1));

    icon = hdps::gui::createIcon(finalPixmap);
}
