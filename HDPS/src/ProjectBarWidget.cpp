#include "ProjectBarWidget.h"

#include <Application.h>
#include <CoreInterface.h>

#include <widgets/Divider.h>

#include <QDebug>
#include <QFileInfo>
#include <QStyleOption>

using namespace hdps;
using namespace hdps::gui;

ProjectBarWidget::ProjectBarWidget(QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _layout(),
    _barLayout()
{
    setAutoFillBackground(true);
    setLayout(&_layout);
    setMinimumWidth(500);

    _layout.setMargin(0);
    _layout.setSpacing(0);

    _layout.addStretch(1);
    _layout.addLayout(&_barLayout, 2);
    _layout.addStretch(1);

    _barLayout.addWidget(new HeaderWidget());
    //_barLayout.addWidget(createHorizontalDivider());
    _barLayout.addWidget(new ProjectsWidget());
}

void ProjectBarWidget::setWidgetBackgroundColorRole(QWidget* widget, const QPalette::ColorRole& colorRole)
{
    Q_ASSERT(widget != nullptr);

    // Create palette for changing the background color
    QPalette palette;

    // Get color from style option
    QStyleOption styleOption;

    // Initialize the style options from this widget
    styleOption.init(widget);

    // Set the palette color for the background
    palette.setColor(QPalette::Window, styleOption.palette.color(QPalette::Normal, colorRole));

    // Apply the palette
    widget->setPalette(palette);
}

ProjectBarWidget::HeaderWidget::HeaderWidget(QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _layout(),
    _headerLabel()
{
    setAutoFillBackground(true);
    setLayout(&_layout);

    const int pixelRatio = devicePixelRatio();

    QString iconName = ":/Images/AppBackground256";

    if (pixelRatio > 1)
        iconName = ":/Images/AppBackground512";

    if (pixelRatio > 2)
        iconName = ":/Images/AppBackground1024";

    _headerLabel.setPixmap(QPixmap(iconName).scaled(200, 200));
    _headerLabel.setAlignment(Qt::AlignCenter);

    _layout.setMargin(36);
    _layout.addWidget(&_headerLabel);

    // Change the background color
    ProjectBarWidget::setWidgetBackgroundColorRole(this, QPalette::Midlight);
}

ProjectBarWidget::ProjectActionWidget::ProjectActionWidget(const QIcon& icon, const QString& title, const QString& description, const ActionCallBack& actionCallback, QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _layout(),
    _fileLayout(),
    _iconLabel(),
    _titleLabel(title),
    _descriptionLabel(description),
    _actionCallback(actionCallback)
{
    setAutoFillBackground(true);

    const auto& fontAwesome = Application::getIconFont("FontAwesome");

    _iconLabel.setPixmap(icon.pixmap(QSize(32, 32)));
    _iconLabel.setFixedWidth(24);

    _titleLabel.setStyleSheet("QLabel { font-size: 9pt; font-weight: bold; }");
    _descriptionLabel.setStyleSheet("QLabel { font-size: 8pt; }"); // padding-top: 2px;

    // Create file layout
    _fileLayout.addWidget(&_titleLabel);
    _fileLayout.addWidget(&_descriptionLabel);

    _layout.setMargin(1);
    _layout.setSpacing(0);
    _layout.addWidget(&_iconLabel);
    _layout.addLayout(&_fileLayout);
    _layout.addStretch(1);

    setLayout(&_layout);

    // Change the background color
    ProjectBarWidget::setWidgetBackgroundColorRole(this, QPalette::Midlight);
}

void ProjectBarWidget::ProjectActionWidget::enterEvent(QEvent* event)
{
    // Change the background color
    ProjectBarWidget::setWidgetBackgroundColorRole(this, QPalette::Dark);
}

void ProjectBarWidget::ProjectActionWidget::leaveEvent(QEvent* event)
{
    // Change the background color
    ProjectBarWidget::setWidgetBackgroundColorRole(this, QPalette::Midlight);
}

void ProjectBarWidget::ProjectActionWidget::mousePressEvent(QMouseEvent* mouseEvent)
{
    _actionCallback();
}

ProjectBarWidget::ProjectsWidget::ProjectsWidget(QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _layout(),
    _leftColumnLayout(),
    _rightColumnLayout()
{
    setAutoFillBackground(true);

    // Change the background color
    ProjectBarWidget::setWidgetBackgroundColorRole(this, QPalette::Midlight);

    _layout.setMargin(50);

    _leftColumnLayout.setAlignment(Qt::AlignTop);
    _rightColumnLayout.setAlignment(Qt::AlignTop);

    _layout.addLayout(&_leftColumnLayout, 1);
    _layout.addLayout(&_rightColumnLayout, 1);

    // Create left and right column
    createLeftColumn();
    createRightColumn();

    // Apply the layout
    setLayout(&_layout);
}

void ProjectBarWidget::ProjectsWidget::createLeftColumn()
{
    // Add header and widget for recent projects
    _leftColumnLayout.addWidget(createHeaderLabel("Recent", "Recently opened HDPS projects"));
    _leftColumnLayout.addSpacerItem(new QSpacerItem(0, 10));
    _leftColumnLayout.addWidget(new RecentProjectsWidget());

    // Stretch to bottom
    _leftColumnLayout.addStretch(1);
}

void ProjectBarWidget::ProjectsWidget::createRightColumn()
{
    // Add header
    _rightColumnLayout.addWidget(createHeaderLabel("Open", "Project open options"));
    _rightColumnLayout.addSpacerItem(new QSpacerItem(0, 10));

    // Add file action for opening a project from a picked location
    _rightColumnLayout.addWidget(new ProjectActionWidget(Application::getIconFont("FontAwesome").getIcon("folder-open"), QFileInfo("Open").baseName(), "Open an existing project from disk", []() {
        Application::current()->loadProject();
    }));

    _rightColumnLayout.addSpacerItem(new QSpacerItem(0, 40));

    // Add header and widget for import data
    _rightColumnLayout.addWidget(createHeaderLabel("Import data", "Import data into HDPS"));
    _rightColumnLayout.addSpacerItem(new QSpacerItem(0, 10));
    _rightColumnLayout.addWidget(new ImportDataWidget());

    // Stretch to bottom
    _rightColumnLayout.addStretch(1);
}

QLabel* ProjectBarWidget::ProjectsWidget::createHeaderLabel(const QString& title, const QString& tooltip)
{
    auto label = new QLabel(title);

    label->setAlignment(Qt::AlignLeft);
    label->setStyleSheet("QLabel { font-weight: bold; font-size: 11pt; color: #606060; }");
    label->setStyleSheet("QLabel { font-size: 11pt; }");

    return label;
}

ProjectBarWidget::RecentProjectsWidget::RecentProjectsWidget(QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _layout(),
    _scrollArea()
{
    setAutoFillBackground(true);

    // Change the background color
    ProjectBarWidget::setWidgetBackgroundColorRole(this, QPalette::Midlight);

    const auto createHeaderLabel = [](const QString& title) ->QLabel* {
        auto label = new QLabel(title);

        label->setAlignment(Qt::AlignLeft);
        label->setStyleSheet("QLabel { font-weight: bold; font-size: 11pt; color: #606060; }");
        label->setStyleSheet("QLabel { font-size: 11pt; }");

        return label;
    };

    _layout.setMargin(0);

    // Get recent projects
    const auto recentProjects = Application::current()->getSetting("Projects/Recent", QVariantList()).toList();

    // Add click able label for each recent project
    for (const auto& recentProject : recentProjects) {

        // Get the recent project file path
        const auto recentProjectFilePath = recentProject.toMap()["FilePath"].toString();

        // Check if the recent project exists on disk
        if (!QFileInfo(recentProjectFilePath).exists())
            continue;

        // Create recent project widget and add it to the layout
        _layout.addWidget(new ProjectActionWidget(Application::getIconFont("FontAwesome").getIcon("file"), QFileInfo(recentProjectFilePath).baseName(), recentProjectFilePath, [recentProjectFilePath]() {
            Application::current()->loadProject(recentProjectFilePath);
        }));
    }

    setLayout(&_layout);

    _scrollArea.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _scrollArea.setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    _scrollArea.setWidget(this);
}

ProjectBarWidget::ImportDataWidget::ImportDataWidget(QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _layout()
{
    setAutoFillBackground(true);

    _layout.setMargin(0);

    // Change the background color
    ProjectBarWidget::setWidgetBackgroundColorRole(this, QPalette::Midlight);

    for (auto pluginKind : Application::core()->getPluginKindsByPluginTypeAndDataTypes(plugin::Type::LOADER)) {

        // Create import data option
        _layout.addWidget(new ProjectActionWidget(Application::core()->getPluginIcon(pluginKind), Application::core()->getPluginGuiName(pluginKind), "Import data", [pluginKind]() {
            Application::core()->importDataset(pluginKind);
        }));
    }

    setLayout(&_layout);
}
