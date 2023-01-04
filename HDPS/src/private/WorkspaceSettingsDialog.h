#pragma once

#include <QDialog>

#include <actions/GroupAction.h>

/**
 * Workspace settings class
 * 
 * Dialog class for editing workspace settings.
 * 
 * @author Thomas Kroes
 */
class WorkspaceSettingsDialog : public QDialog
{
public:

    /**
     * Construct a workspace settings dialog with \p parent
     * @param parent Pointer to parent widget
     */
    WorkspaceSettingsDialog(QWidget* parent = nullptr);

    /** Get preferred size */
    QSize sizeHint() const override {
        return QSize(700, 500);
    }

    /** Get minimum size hint*/
    QSize minimumSizeHint() const override {
        return sizeHint();
    }

private:
    hdps::gui::GroupAction      _groupAction;       /** Settings group action */
};
