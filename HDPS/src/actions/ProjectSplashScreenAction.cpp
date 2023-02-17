#include "ProjectSplashScreenAction.h"
#include "Project.h"

#include <QPropertyAnimation>

namespace hdps::gui {

ProjectSplashScreenAction::ProjectSplashScreenAction(QObject* parent, const Project& project) :
    HorizontalGroupAction(parent, "Splash Screen"),
    _project(project),
    _enabledAction(this, "Enabled"),
    _previewAction(this, "Preview")
{
    *this << _enabledAction;
    *this << _previewAction;

    _enabledAction.setStretch(1);

    connect(&_previewAction, &TriggerAction::triggered, this, [this]() -> void {
        Dialog previeweDialog(*this);

        previeweDialog.exec();
    });
}

QString ProjectSplashScreenAction::getTypeString() const
{
    return "ProjectSplashScreen";
}

int ProjectSplashScreenAction::Dialog::exec()
{
    setWindowOpacity(0.0);
    
    QPropertyAnimation* opacityPropertyAnimation = new QPropertyAnimation(this, "windowOpacity");
    
    opacityPropertyAnimation->setDuration(5000); // will take 5 seconds
    opacityPropertyAnimation->setEasingCurve(QEasingCurve::OutBack); // just demonstration, there are a lot of curves to choose
    opacityPropertyAnimation->setStartValue(0.0);
    opacityPropertyAnimation->setEndValue(1.0);
    opacityPropertyAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    
    return QDialog::exec();
}

ProjectSplashScreenAction::Dialog::Dialog(const ProjectSplashScreenAction& projectSplashScreenAction, QWidget* parent /*= nullptr*/) :
    QDialog(parent),
    _projectSplashScreenAction(projectSplashScreenAction)
{

}

}