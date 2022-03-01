#include "StartPageWidget.h"

#include <Application.h>
#include <widgets/Divider.h>

#include <QDebug>
#include <QFileInfo>
#include <QStyleOption>

namespace hdps
{

namespace gui
{

StartPageWidget::StartPageWidget(QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _mainLayout(),
    _centerLayout()
{
    setAutoFillBackground(true);
    setLayout(&_mainLayout);

    // Create palette and linear gradient for background
    auto palette    = QPalette();
    auto gradient   = QLinearGradient(0, 0, 0, 400);

    // Gradient gray scale at top and bottom
    const auto topGrayLevel     = 170;
    const auto bottomGrayLevel  = 200;

    // Set gradient stops
    gradient.setColorAt(0.0, QColor(topGrayLevel, topGrayLevel, topGrayLevel));
    gradient.setColorAt(1.0, QColor(bottomGrayLevel, bottomGrayLevel, bottomGrayLevel));

    // Set palette brush to linear gradient
    palette.setBrush(QPalette::Window, QBrush(gradient));

    // Apply the palette to the widget
    setPalette(palette);

    _mainLayout.setMargin(0);
    _mainLayout.setSpacing(0);

    _mainLayout.addStretch(1);
    _mainLayout.addLayout(&_centerLayout, 3);
    _mainLayout.addStretch(1);

    _centerLayout.addWidget(new LogoWidget());
    _centerLayout.addWidget(createHorizontalDivider());
    _centerLayout.addWidget(new ProjectsWidget());
}

void StartPageWidget::setWidgetBackgroundColorRole(QWidget* widget, const QPalette::ColorRole& colorRole)
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

StartPageWidget::FileActionWidget::FileActionWidget(const QString& icon, const QString& title, const QString& description, const ActionCallBack& actionCallback, QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _layout(),
    _fileLayout(),
    _iconLabel(),
    _titleLabel(title),
    _descriptionLabel(description),
    _actionCallback(actionCallback)
{
    setAutoFillBackground(true);

    _layout.setMargin(2);

    const auto& fontAwesome = Application::getIconFont("FontAwesome");

    //_iconLabel.setFixedWidth(30);
    _iconLabel.setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    _iconLabel.setFont(fontAwesome.getFont(11));
    _iconLabel.setText(fontAwesome.getIconCharacter(icon));
    _iconLabel.setStyleSheet("QLabel { padding-left: 1px; padding-right: 2px; }");

    _titleLabel.setStyleSheet("QLabel { font-size: 8pt; font-weight: bold; }");
    _descriptionLabel.setStyleSheet("QLabel { font-size: 7pt; }");

    // Create file layout
    _fileLayout.addWidget(&_titleLabel);
    _fileLayout.addWidget(&_descriptionLabel);

    _layout.setSpacing(0);
    _layout.addWidget(&_iconLabel);
    _layout.addLayout(&_fileLayout);
    _layout.addStretch(1);

    setLayout(&_layout);

    // Change the background color
    StartPageWidget::setWidgetBackgroundColorRole(this, QPalette::Midlight);
}

void StartPageWidget::FileActionWidget::enterEvent(QEvent* event)
{
    // Change the background color
    StartPageWidget::setWidgetBackgroundColorRole(this, QPalette::Dark);
}

void StartPageWidget::FileActionWidget::leaveEvent(QEvent* event)
{
    // Change the background color
    StartPageWidget::setWidgetBackgroundColorRole(this, QPalette::Midlight);
}

void StartPageWidget::FileActionWidget::mousePressEvent(QMouseEvent* mouseEvent)
{
    _actionCallback();
}

StartPageWidget::ProjectsWidget::ProjectsWidget(QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _layout(),
    _recentLayout(),
    _openLayout()
{
    setAutoFillBackground(true);

    // Change the background color
    StartPageWidget::setWidgetBackgroundColorRole(this, QPalette::Midlight);

    const auto createHeaderLabel = [](const QString& title) ->QLabel* {
        auto label = new QLabel(title);

        label->setAlignment(Qt::AlignLeft);
        label->setStyleSheet("QLabel { font-weight: bold; font-size: 11pt; color: #606060; }");
        label->setStyleSheet("QLabel { font-size: 11pt; }");

        return label;
    };

    _layout.setMargin(20);

    _layout.addLayout(&_recentLayout, 1);
    _layout.addLayout(&_openLayout, 1);

    _recentLayout.addWidget(createHeaderLabel("Recent"));
    _openLayout.addWidget(createHeaderLabel("Open"));

    // Get recent projects
    const auto recentProjects = Application::current()->getSetting("Projects/Recent", QVariantList()).toList();

    // Add click able label for each recent project
    for (const auto& recentProject : recentProjects) {

        // Get the recent project file path
        const auto recentProjectFilePath = recentProject.toString();

        // Check if the recent project exists on disk
        if (!QFileInfo(recentProjectFilePath).exists())
            continue;

        // Create recent project widget and add it to the layout
        _recentLayout.addWidget(new FileActionWidget("file", QFileInfo(recentProjectFilePath).baseName(), recentProjectFilePath, [recentProjectFilePath]() {
            Application::current()->loadProject(recentProjectFilePath);
        }));
    }

    _recentLayout.addStretch(1);

    // Add file action for opening a project from a picked location
    _openLayout.addWidget(new FileActionWidget("folder-open", QFileInfo("Open").baseName(), "Open an existing project from disk", []() {
        Application::current()->loadProject();
    }));

    _openLayout.addStretch(1);

    setLayout(&_layout);
}

StartPageWidget::LogoWidget::LogoWidget(QWidget* parent /*= nullptr*/) :
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
    StartPageWidget::setWidgetBackgroundColorRole(this, QPalette::Midlight);
}

}
}
