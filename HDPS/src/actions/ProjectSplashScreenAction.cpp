#include "ProjectSplashScreenAction.h"
#include "Project.h"

#include <QPropertyAnimation>

namespace hdps::gui {

ProjectSplashScreenAction::ProjectSplashScreenAction(QObject* parent, const Project& project) :
    HorizontalGroupAction(parent, "Splash Screen"),
    _project(project),
    _enabledAction(this, "Enabled"),
    _durationAction(this, "Duration", 1000, 10000, 3000),
    _previewAction(this, "Preview")
{
    _enabledAction.setStretch(1);
    _enabledAction.setToolTip("Show splash screen at startup");

    _durationAction.setToolTip("Duration of the splash screen in milliseconds");
    _durationAction.setSuffix("ms");

    _previewAction.setToolTip("Preview the splash screen");

    addAction(_enabledAction);
    addAction(_previewAction);

    connect(&_previewAction, &TriggerAction::triggered, this, [this]() -> void {
        Dialog previeweDialog(*this);

        previeweDialog.exec();
    });
}

QString ProjectSplashScreenAction::getTypeString() const
{
    return "ProjectSplashScreen";
}

ProjectSplashScreenAction::Dialog::Dialog(const ProjectSplashScreenAction& projectSplashScreenAction, QWidget* parent /*= nullptr*/) :
    QDialog(parent),
    _projectSplashScreenAction(projectSplashScreenAction)
{
}

int ProjectSplashScreenAction::Dialog::exec()
{
    setWindowOpacity(0.0);

    QPropertyAnimation* opacityPropertyAnimation = new QPropertyAnimation(this, "windowOpacity");

    opacityPropertyAnimation->setDuration(_projectSplashScreenAction.getDurationAction().getValue());
    opacityPropertyAnimation->setEasingCurve(QEasingCurve::OutBack);
    opacityPropertyAnimation->setStartValue(0.0);
    opacityPropertyAnimation->setEndValue(1.0);
    opacityPropertyAnimation->start(QAbstractAnimation::DeleteWhenStopped);

    return QDialog::exec();
}

}