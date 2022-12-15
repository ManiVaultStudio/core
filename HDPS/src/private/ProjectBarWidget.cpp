#include "ProjectBarWidget.h"

#include <Application.h>
#include <CoreInterface.h>

#include <widgets/Divider.h>

#include <QDebug>
#include <QFileInfo>
#include <QStyleOption>
#include <QScrollBar>
#include <QResizeEvent>
#include <QPainter>

using namespace hdps;
using namespace hdps::gui;

ProjectBarWidget::ProjectBarWidget(QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _layout(),
    _barLayout(),
    _backgroundImage(":/Images/StartPageBackground")
{
    setLayout(&_layout);
    setMinimumWidth(500);

    _layout.setContentsMargins(0, 0, 0, 0);
    _layout.setSpacing(0);

    _layout.addStretch(1);
    _layout.addLayout(&_barLayout, 2);
    _layout.addStretch(1);

    _barLayout.addWidget(new HeaderWidget());
    //_barLayout.addWidget(createHorizontalDivider());
    _barLayout.addWidget(new ProjectsWidget());
}

void ProjectBarWidget::paintEvent(QPaintEvent* paintEvent)
{
    QPainter painter(this);

    // Get scaled background image and rectangles for positioning the background image
    auto backgroundImage    = _backgroundImage.scaled(width(), height(), Qt::KeepAspectRatioByExpanding);
    auto centerOfWidget     = rect().center();
    auto pixmapRectangle    = backgroundImage.rect();

    // Position in the center
    pixmapRectangle.moveCenter(centerOfWidget);

    // Draw the background
    painter.drawPixmap(pixmapRectangle.topLeft(), QPixmap(backgroundImage));
}

void ProjectBarWidget::setWidgetBackgroundColorRole(QWidget* widget, const QPalette::ColorRole& colorRole)
{
    Q_ASSERT(widget != nullptr);

    // Create palette for changing the background color
    QPalette palette;

    // Get color from style option
    QStyleOption styleOption;

    // Initialize the style options from this widget
    styleOption.initFrom(widget);

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

    _headerLabel.setPixmap(QPixmap(iconName).scaled(256, 256));
    _headerLabel.setAlignment(Qt::AlignCenter);

    _layout.setContentsMargins(50, 50, 50, 50);
    _layout.addWidget(&_headerLabel);

    // Change the background color
    ProjectBarWidget::setWidgetBackgroundColorRole(this, QPalette::Midlight);
}

ProjectBarWidget::ProjectActionWidget::ProjectActionWidget(const QIcon& icon, const QString& title, const QString& description, const QString& tooltip, const ActionCallBack& actionCallback, QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _layout(),
    _fileLayout(),
    _iconLabel(),
    _titleLabel(title),
    _descriptionLabel(description),
    _actionCallback(actionCallback)
{
    setAutoFillBackground(true);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setToolTip(tooltip);
    
    const auto& fontAwesome = Application::getIconFont("FontAwesome");

    if (!icon.isNull()) {
        _iconLabel.setPixmap(icon.pixmap(QSize(24, 24)));
        _iconLabel.setFixedWidth(24);
        _iconLabel.setStyleSheet("QLabel { margin-left: 2px}");
    }

    _titleLabel.setStyleSheet("QLabel { font-size: 9pt; font-weight: bold; }");
    _descriptionLabel.setStyleSheet("QLabel { font-size: 8pt; }");

    // Create file layout
    _fileLayout.addWidget(&_titleLabel);
    _fileLayout.addWidget(&_descriptionLabel);

    _layout.setContentsMargins(1, 3, 3, 3);
    _layout.setSpacing(2);
    _layout.addWidget(&_iconLabel);
    _layout.addLayout(&_fileLayout);
    _layout.addStretch(1);

    setLayout(&_layout);

    // Change the background color
    ProjectBarWidget::setWidgetBackgroundColorRole(this, QPalette::Midlight);
}

void ProjectBarWidget::ProjectActionWidget::enterEvent(QEnterEvent* event)
{
    // Change the background color
    ProjectBarWidget::setWidgetBackgroundColorRole(this, QPalette::Dark);

    event->ignore();
}

void ProjectBarWidget::ProjectActionWidget::leaveEvent(QEvent* event)
{
    // Change the background color
    ProjectBarWidget::setWidgetBackgroundColorRole(this, QPalette::Midlight);

    event->ignore();
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

    _layout.setContentsMargins(35, 35, 35, 35);
    //_layout.setSpacing(30);

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
}

void ProjectBarWidget::ProjectsWidget::createRightColumn()
{
    // Add header
    _rightColumnLayout.addWidget(createHeaderLabel("Open", "Project open options"));
    _rightColumnLayout.addSpacerItem(new QSpacerItem(0, 10));

    // Establish title, description and tooltip
    const auto title        = "Open project";
    const auto description  = "Open an existing project from disk";
    const auto tooltip      = "Use the file navigator to open an existing project from disk";

    // Add file action for opening a project from a picked location
    _rightColumnLayout.addWidget(new ProjectActionWidget(Application::getIconFont("FontAwesome").getIcon("folder-open"), title, description, tooltip, []() {
        Application::core()->getProjectManager().loadProject();
    }));

    _rightColumnLayout.addSpacerItem(new QSpacerItem(0, 40));

    // Add header and widget for import data
    _rightColumnLayout.addWidget(createHeaderLabel("Import data", "Import data into HDPS"));
    _rightColumnLayout.addSpacerItem(new QSpacerItem(0, 10));
    //_rightColumnLayout.addWidget(new ImportDataWidget());
}

QLabel* ProjectBarWidget::ProjectsWidget::createHeaderLabel(const QString& title, const QString& tooltip)
{
    auto label = new QLabel(title);

    label->setAlignment(Qt::AlignLeft);
    label->setStyleSheet("QLabel { font-weight: 200; font-size: 13pt; }");
    label->setToolTip(tooltip);

    return label;
}

ProjectBarWidget::ScrollArea::ScrollArea(QWidget* parent /*= nullptr*/) :
    QScrollArea(parent)
{
}

void ProjectBarWidget::ScrollArea::enterEvent(QEvent* event)
{
    verticalScrollBar()->show();
}

void ProjectBarWidget::ScrollArea::leaveEvent(QEvent* event)
{
    verticalScrollBar()->hide();
}

void ProjectBarWidget::ScrollArea::resizeEvent(QResizeEvent* resizeEvent)
{
    QScrollArea::resizeEvent(resizeEvent);

    widget()->setFixedWidth(resizeEvent->size().width());
}

ProjectBarWidget::RecentProjectsWidget::RecentProjectsWidget(QWidget* parent /*= nullptr*/) :
    ScrollArea(parent),
    _containerWidget(this),
    _containerLayout()
{
    setAutoFillBackground(true);
    setFrameShape(QFrame::NoFrame);
    createContainerWidget();
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setWidget(&_containerWidget);

    ProjectBarWidget::setWidgetBackgroundColorRole(this, QPalette::Midlight);
}

void ProjectBarWidget::RecentProjectsWidget::createContainerWidget()
{
    ProjectBarWidget::setWidgetBackgroundColorRole(&_containerWidget, QPalette::Midlight);

    _containerWidget.setAutoFillBackground(true);
    _containerLayout.setContentsMargins(0, 0, 0, 0);

    // Get recent projects
    const auto recentProjects = Application::current()->getSetting("Projects/Recent", QVariantList()).toList();

    // Add click able label for each recent project
    for (const auto& recentProject : recentProjects) {

        // Establish file path, title, description and tooltip
        const auto filePath     = recentProject.toMap()["FilePath"].toString();
        const auto title        = QFileInfo(filePath).baseName();
        const auto description  = filePath;
        const auto tooltip      = title + ", last opened on " + recentProject.toMap()["DateTime"].toDateTime().toString();

        // Check if the recent project exists on disk
        if (!QFileInfo(filePath).exists())
            continue;

        // Create recent project widget and add it to the layout
        _containerLayout.addWidget(new ProjectActionWidget(Application::getIconFont("FontAwesome").getIcon("file"), title, description, tooltip, [filePath]() {
            Application::core()->getProjectManager().loadProject(filePath);
        }));
    }

    _containerWidget.setLayout(&_containerLayout);
    _containerWidget.setVisible(!recentProjects.isEmpty());
}

ProjectBarWidget::ImportDataWidget::ImportDataWidget(QWidget* parent /*= nullptr*/) :
    ScrollArea(parent),
    _containerWidget(this),
    _containerLayout()
{
    setAutoFillBackground(true);
    setFrameShape(QFrame::NoFrame);
    createContainerWidget();
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setWidget(&_containerWidget);

    ProjectBarWidget::setWidgetBackgroundColorRole(this, QPalette::Midlight);
}

void ProjectBarWidget::ImportDataWidget::createContainerWidget()
{
    ProjectBarWidget::setWidgetBackgroundColorRole(&_containerWidget, QPalette::Midlight);

    _containerWidget.setAutoFillBackground(true);
    _containerLayout.setContentsMargins(0, 0, 0, 0);

    const auto loaderPluginKinds = Application::core()->getPluginManager().getPluginKindsByPluginTypes({ plugin::Type::LOADER });

    for (const auto& pluginKind : loaderPluginKinds) {

        const auto title        = Application::core()->getPluginManager().getPluginGuiName(pluginKind);
        const auto description  = "Import data";
        const auto tooltip      = "Import data into HDPS with the " + pluginKind;

        _containerLayout.addWidget(new ProjectActionWidget(QIcon(), title, description, tooltip, [pluginKind]() {
            Application::core()->getPluginManager().requestPlugin(pluginKind);
        }));
    }

    _containerWidget.setLayout(&_containerLayout);
    _containerWidget.setVisible(!loaderPluginKinds.isEmpty());
}
