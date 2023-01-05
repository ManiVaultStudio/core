#pragma once

#include <QDialog>

#include <actions/GroupAction.h>
#include <actions/TriggerAction.h>

/**
 * New project dialog class
 * 
 * Dialog class for creating new projects.
 * 
 * @author Thomas Kroes
 */
class NewProjectDialog : public QDialog
{
public:

    /**
     * Construct a dialog with \p parent
     * @param parent Pointer to parent widget
     */
    NewProjectDialog(QWidget* parent = nullptr);

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
    hdps::gui::TriggerAction    _okAction;          /** Action for exiting the dialog */
};
