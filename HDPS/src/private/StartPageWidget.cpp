#include "StartPageWidget.h"

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

StartPageWidget::StartPageWidget(QWidget* parent /*= nullptr*/) :
    OverlayWidget(parent),
    _layout(),
    _barLayout(),
    _backgroundImage(":/Images/StartPageBackground")
{
    setLayout(&_layout);
    setMinimumWidth(500);

    _layout.setContentsMargins(0, 0, 0, 0);
    _layout.setSpacing(0);

    _barLayout.addWidget(new HeaderWidget(this));
    //_barLayout.addWidget(createHorizontalDivider());
    _barLayout.addWidget(new ProjectsWidget(this));

    _layout.addStretch(1);
    _layout.addLayout(&_barLayout, 2);
    _layout.addStretch(1);

    getWidgetFader().setFadeInDuration(250);
    getWidgetFader().setFadeOutDuration(250);

    connect(this, &OverlayWidget::shown, this, [this]() -> void {
        setAttribute(Qt::WA_TransparentForMouseEvents, false);
    });

    connect(this, &OverlayWidget::hidden, this, [this]() -> void {
        setAttribute(Qt::WA_TransparentForMouseEvents, true);
    });
}

void StartPageWidget::paintEvent(QPaintEvent* paintEvent)
{
    QPainter painter(this);

    auto backgroundImage    = _backgroundImage.scaled(width(), height(), Qt::KeepAspectRatioByExpanding);
    auto centerOfWidget     = rect().center();
    auto pixmapRectangle    = backgroundImage.rect();

    pixmapRectangle.moveCenter(centerOfWidget);

    painter.drawPixmap(pixmapRectangle.topLeft(), QPixmap(backgroundImage));
}

void StartPageWidget::setWidgetBackgroundColorRole(QWidget* widget, const QPalette::ColorRole& colorRole)
{
    Q_ASSERT(widget != nullptr);

    QPalette palette;

    QStyleOption styleOption;

    styleOption.initFrom(widget);

    palette.setColor(QPalette::Window, styleOption.palette.color(QPalette::Normal, colorRole));

    widget->setStyleSheet(QString("background-color: %1").arg(styleOption.palette.color(QPalette::Normal, colorRole).name(QColor::HexArgb)));
}

StartPageWidget::HeaderWidget::HeaderWidget(QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _layout(),
    _headerLabel()
{
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

    StartPageWidget::setWidgetBackgroundColorRole(this, QPalette::Midlight);
}

StartPageWidget::ProjectActionWidget::ProjectActionWidget(const QIcon& icon, const QString& title, const QString& description, const QString& tooltip, const ActionCallBack& actionCallback, QWidget* parent /*= nullptr*/) :
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

    StartPageWidget::setWidgetBackgroundColorRole(this, QPalette::Midlight);
}

void StartPageWidget::ProjectActionWidget::enterEvent(QEnterEvent* event)
{
    StartPageWidget::setWidgetBackgroundColorRole(this, QPalette::Dark);

    event->ignore();
}

void StartPageWidget::ProjectActionWidget::leaveEvent(QEvent* event)
{
    StartPageWidget::setWidgetBackgroundColorRole(this, QPalette::Midlight);

    event->ignore();
}

void StartPageWidget::ProjectActionWidget::mousePressEvent(QMouseEvent* mouseEvent)
{
    _actionCallback();
}

StartPageWidget::ProjectsWidget::ProjectsWidget(QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _layout(),
    _leftColumnLayout(),
    _rightColumnLayout()
{
    StartPageWidget::setWidgetBackgroundColorRole(this, QPalette::Midlight);

    _layout.setContentsMargins(35, 35, 35, 35);

    _leftColumnLayout.setAlignment(Qt::AlignTop);
    _rightColumnLayout.setAlignment(Qt::AlignTop);

    _layout.addLayout(&_leftColumnLayout, 1);
    _layout.addLayout(&_rightColumnLayout, 1);

    createLeftColumn();
    createRightColumn();

    setLayout(&_layout);
}

void StartPageWidget::ProjectsWidget::createLeftColumn()
{
    _leftColumnLayout.addWidget(createHeaderLabel("Recent", "Recently opened HDPS projects"));
    _leftColumnLayout.addSpacerItem(new QSpacerItem(0, 10));
    _leftColumnLayout.addWidget(new RecentProjectsWidget(this));
}

void StartPageWidget::ProjectsWidget::createRightColumn()
{
    _rightColumnLayout.addWidget(createHeaderLabel("Open", "Project open options"));
    _rightColumnLayout.addSpacerItem(new QSpacerItem(0, 10));

    const auto title        = "Open project";
    const auto description  = "Open an existing project from disk";
    const auto tooltip      = "Use the file navigator to open an existing project from disk";

    _rightColumnLayout.addWidget(new ProjectActionWidget(Application::getIconFont("FontAwesome").getIcon("folder-open"), title, description, tooltip, []() {
        Application::core()->getProjectManager().loadProject();
    }));

    _rightColumnLayout.addSpacerItem(new QSpacerItem(0, 40));

    _rightColumnLayout.addWidget(createHeaderLabel("Import data", "Import data into HDPS"));
    _rightColumnLayout.addSpacerItem(new QSpacerItem(0, 10));
    //_rightColumnLayout.addWidget(new ImportDataWidget());
}

QLabel* StartPageWidget::ProjectsWidget::createHeaderLabel(const QString& title, const QString& tooltip)
{
    auto label = new QLabel(title);

    label->setAlignment(Qt::AlignLeft);
    label->setStyleSheet("QLabel { font-weight: 200; font-size: 13pt; }");
    label->setToolTip(tooltip);

    return label;
}

StartPageWidget::ScrollArea::ScrollArea(QWidget* parent /*= nullptr*/) :
    QScrollArea(parent)
{
}

void StartPageWidget::ScrollArea::enterEvent(QEvent* event)
{
    verticalScrollBar()->show();
}

void StartPageWidget::ScrollArea::leaveEvent(QEvent* event)
{
    verticalScrollBar()->hide();
}

void StartPageWidget::ScrollArea::resizeEvent(QResizeEvent* resizeEvent)
{
    QScrollArea::resizeEvent(resizeEvent);

    widget()->setFixedWidth(resizeEvent->size().width());
}

StartPageWidget::RecentProjectsWidget::RecentProjectsWidget(QWidget* parent /*= nullptr*/) :
    ScrollArea(parent),
    _containerWidget(this),
    _containerLayout()
{
    setFrameShape(QFrame::NoFrame);
    createContainerWidget();
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setWidget(&_containerWidget);

    StartPageWidget::setWidgetBackgroundColorRole(this, QPalette::Midlight);
}

void StartPageWidget::RecentProjectsWidget::createContainerWidget()
{
    StartPageWidget::setWidgetBackgroundColorRole(&_containerWidget, QPalette::Midlight);

    _containerLayout.setContentsMargins(0, 0, 0, 0);

    const auto recentProjects = Application::current()->getSetting("Manager/Project/Recent", QVariantList()).toList();

    for (const auto& recentProject : recentProjects) {
        const auto filePath     = recentProject.toMap()["FilePath"].toString();
        const auto title        = QFileInfo(filePath).baseName();
        const auto description  = filePath;
        const auto tooltip      = title + ", last opened on " + recentProject.toMap()["DateTime"].toDateTime().toString();

        if (!QFileInfo(filePath).exists())
            continue;

        _containerLayout.addWidget(new ProjectActionWidget(Application::getIconFont("FontAwesome").getIcon("file"), title, description, tooltip, [filePath]() {
            Application::core()->getProjectManager().loadProject(filePath);
        }));
    }

    _containerWidget.setLayout(&_containerLayout);
    _containerWidget.setVisible(!recentProjects.isEmpty());
}

StartPageWidget::ImportDataWidget::ImportDataWidget(QWidget* parent /*= nullptr*/) :
    ScrollArea(parent),
    _containerWidget(this),
    _containerLayout()
{
    setFrameShape(QFrame::NoFrame);
    createContainerWidget();
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setWidget(&_containerWidget);

    StartPageWidget::setWidgetBackgroundColorRole(this, QPalette::Midlight);
}

void StartPageWidget::ImportDataWidget::createContainerWidget()
{
    StartPageWidget::setWidgetBackgroundColorRole(&_containerWidget, QPalette::Midlight);

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
