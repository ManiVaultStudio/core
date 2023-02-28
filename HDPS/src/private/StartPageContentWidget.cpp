#include "StartPageContentWidget.h"
#include "StartPageWidget.h"

#include <Application.h>

#include <QDebug>

using namespace hdps;
using namespace hdps::gui;

StartPageContentWidget::StartPageContentWidget(QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _mainLayout(),
    _collumnsLayout(),
    _toolbarLayout(),
    _compactViewAction(this, "Compact"),
    _toggleOpenCreateProjectAction(this, "Open & Create"),
    _toggleRecentProjectsAction(this, "Recent Projects"),
    _toggleExampleProjectsAction(this, "Examples"),
    _toggleProjectFromWorkspaceAction(this, "Project From Workspace"),
    _toggleProjectFromDataAction(this, "Project From Data"),
    _toggleTutorialVideosAction(this, "Instructional Videos"),
    _settingsAction(this, "Settings"),
    _openProjectWidget(this),
    _getStartedWidget(this)
{
    setAutoFillBackground(true);

    StartPageWidget::setWidgetBackgroundColorRole(this, QPalette::Midlight);

    _compactViewAction.setSettingsPrefix("StartPage/ToggleCompactView");
    _toggleOpenCreateProjectAction.setSettingsPrefix("StartPage/ToggleOpenCreateProject");
    _toggleRecentProjectsAction.setSettingsPrefix("StartPage/ToggleRecentProjects");
    _toggleExampleProjectsAction.setSettingsPrefix("StartPage/ToggleExampleProjects");
    _toggleProjectFromWorkspaceAction.setSettingsPrefix("StartPage/ToggleProjectFromWorkspace");
    _toggleProjectFromDataAction.setSettingsPrefix("StartPage/ToggleProjectFromData");
    _toggleTutorialVideosAction.setSettingsPrefix("StartPage/ToggleTutorialVideos");

    _settingsAction.setIcon(Application::getIconFont("FontAwesome").getIcon("eye"));
    _settingsAction.setText("Toggle Views");

    _settingsAction.addAction(&_toggleOpenCreateProjectAction);
    _settingsAction.addAction(&_toggleRecentProjectsAction);
    _settingsAction.addAction(&_toggleExampleProjectsAction);
    _settingsAction.addAction(&_toggleProjectFromWorkspaceAction);
    _settingsAction.addAction(&_toggleProjectFromDataAction);
    _settingsAction.addAction(&_toggleTutorialVideosAction);

    _collumnsLayout.setContentsMargins(35, 35, 35, 35);
    _toolbarLayout.setContentsMargins(35, 10, 35, 10);

    _collumnsLayout.addWidget(&_openProjectWidget);
    _collumnsLayout.addWidget(&_getStartedWidget);

    _mainLayout.addLayout(&_collumnsLayout, 1);
    _mainLayout.addLayout(&_toolbarLayout);

    _toolbarLayout.addWidget(_compactViewAction.createWidget(this));
    _toolbarLayout.addStretch(1);
    _toolbarLayout.addWidget(_settingsAction.createCollapsedWidget(this));

    setLayout(&_mainLayout);

    connect(&_compactViewAction, &ToggleAction::toggled, this, &StartPageContentWidget::updateActions);
}

QLabel* StartPageContentWidget::createHeaderLabel(const QString& title, const QString& tooltip)
{
    auto label = new QLabel(title);

    label->setAlignment(Qt::AlignLeft);
    label->setStyleSheet("QLabel { font-weight: 200; font-size: 13pt; }");
    label->setToolTip(tooltip);

    return label;
}

void StartPageContentWidget::updateActions()
{
    StartPageAction::setCompactView(_compactViewAction.isChecked());

    _openProjectWidget.updateActions();
    _getStartedWidget.updateActions();
}
