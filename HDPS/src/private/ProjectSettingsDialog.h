#pragma once

#include <QDialog>

#include <actions/GroupAction.h>
#include <actions/TriggersAction.h>
#include <actions/TriggerAction.h>

/**
 * Project settings dialog class
 * 
 * Dialog class for editing project settings.
 * 
 * @author Thomas Kroes
 */
class ProjectSettingsDialog : public QDialog
{
public:

    /**
     * Construct a project settings dialog with \p parent
     * @param parent Pointer to parent widget
     */
    ProjectSettingsDialog(QWidget* parent = nullptr);

    /** Get preferred size */
    QSize sizeHint() const override {
        return QSize(600, 400);
    }

    /** Get minimum size hint*/
    QSize minimumSizeHint() const override {
        return sizeHint();
    }

private:
    hdps::gui::GroupAction      _groupAction;       /** Settings group action */
    hdps::gui::TriggersAction   _freezeAction;      /** Action for freezing/unfreezing the workspace */
    hdps::gui::TriggerAction    _okAction;          /** Action for exiting the dialog */
};
