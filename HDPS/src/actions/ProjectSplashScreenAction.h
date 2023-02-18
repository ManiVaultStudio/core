#pragma once

#include "HorizontalGroupAction.h"
#include "ToggleAction.h"
#include "IntegralAction.h"
#include "TriggerAction.h"
#include "GroupAction.h"
#include "ColorAction.h"

#include <QDialog>
#include <QPropertyAnimation>

namespace hdps {
    class Project;
}

namespace hdps::gui {

/**
 * Splash screen action class
 *
 * Action class for configuring and displaying a project splash screen
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

        void open() override;
        void animate(bool reverse);

        void accept() override;

        QSize sizeHint() const override;

    private:
        const ProjectSplashScreenAction&    _projectSplashScreenAction;
        QPropertyAnimation                  _opacityAnimation;
        QPropertyAnimation                  _positionAnimation;

        const int animationDuration = 400;
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

    /**
     * Get parent project
     * @return Reference to parent project
     */
    const Project& getProject() const;

public: // Serialization

    /**
     * Load project from variant
     * @param Variant representation of the project
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save project to variant
     * @return Variant representation of the project
     */
    QVariantMap toVariantMap() const override;

public:
    const ToggleAction& getEnabledAction() const { return _enabledAction; }
    const IntegralAction& getDurationAction() const { return _durationAction; }
    const ColorAction& getBackgroundColorAction() const { return _backgroundColorAction; }
    const GroupAction& getEditAction() const { return _editAction; }
    const TriggerAction& getShowSplashScreenAction() const { return _showSplashScreenAction; }

    ToggleAction& getEnabledAction() { return _enabledAction; }
    IntegralAction& getDurationAction() { return _durationAction; }
    ColorAction& getBackgroundColorAction() { return _backgroundColorAction; }
    GroupAction& getEditAction() { return _editAction; }
    TriggerAction& getShowSplashScreenAction() { return _showSplashScreenAction; }

private:
    const Project&      _project;                   /** Reference to project which owns this action */
    ToggleAction        _enabledAction;             /** Action to toggle the splash screen on/off */
    IntegralAction      _durationAction;            /** Action to control the display duration */
    ColorAction         _backgroundColorAction;     /** Action to control the background color of the splash screen */
    GroupAction         _editAction;                /** Group action for editing the splash screen */
    TriggerAction       _showSplashScreenAction;    /** Trigger action to show the splash screen */
    Dialog              _splashScreenDialog;        /** Splash screen dialog */

    friend class Project;
};

}
