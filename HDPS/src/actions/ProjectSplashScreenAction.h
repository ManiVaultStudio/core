#pragma once

#include "HorizontalGroupAction.h"
#include "ToggleAction.h"
#include "IntegralAction.h"
#include "TriggerAction.h"

#include <QDialog>

namespace hdps {
    class Project;
}

namespace hdps::gui {

/**
 * Splash screen action class
 *
 * Action class for configuring a project splash screen
 *
 * @author Thomas Kroes
 */
class ProjectSplashScreenAction : public HorizontalGroupAction
{
    Q_OBJECT
    
protected:

    class Dialog final : public QDialog {
    public:
        Dialog(const ProjectSplashScreenAction& projectSplashScreenAction, QWidget* parent = nullptr);

        int exec() override;

    private:
        const ProjectSplashScreenAction& _projectSplashScreenAction;
    };

protected:

    /**
     * Construct with \p parent and \p project
     * @param parent Pointer to parent object
     * @param project Reference to project which owns this action
     */
    ProjectSplashScreenAction(QObject* parent, const Project& project);

    /**
     * Get type string
     * @return Widget action type in string format
     */
    QString getTypeString() const override;

public:
    const ToggleAction& getEnabledAction() const { return _enabledAction; }
    const IntegralAction& getDurationAction() const { return _durationAction; }
    const TriggerAction& getPreviewAction() const { return _previewAction; }

    ToggleAction& getEnabledAction() { return _enabledAction; }
    IntegralAction& getDurationAction() { return _durationAction; }
    TriggerAction& getPreviewAction() { return _previewAction; }

private:
    const Project&  _project;           /** Reference to project which owns this action */
    ToggleAction    _enabledAction;     /** Action to toggle the splash screen on/off */
    IntegralAction  _durationAction;    /** Action to control the display duration */
    TriggerAction   _previewAction;     /** Action to preview the splash screen */

    friend class Project;
};

}
