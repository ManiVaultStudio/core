#pragma once

#include <QDialog>

#include <actions/GroupsAction.h>
#include <actions/GroupAction.h>

namespace hdps::gui {

/**
 * Settings manager dialog class
 * 
 * Dialog class for modifying settings.
 *
 * @author Thomas Kroes
 */
class SettingsManagerDialog : public QDialog
{
public:

    /**
     * Construct a dialog for the settings manager with \p with parent widget
     * @param parent Pointer to parent widget
     */
    SettingsManagerDialog(QWidget* parent = nullptr);

    /** Get preferred size */
    QSize sizeHint() const override;

    /** Get minimum size hint*/
    QSize minimumSizeHint() const override {
        return sizeHint();
    }

private:
    GroupsAction    _groupsAction;              /** Groups action for all global settings sections */
    GroupAction     _globalPathsGroupAction;    /** Group action for global paths */
    GroupAction     _ioAction;                  /** Group action for IO related settings */
};

}