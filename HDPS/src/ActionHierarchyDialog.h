#pragma once

#include <actions/WidgetAction.h>

#include <QDialog>

using namespace hdps::gui;

/**
 * Action hierarchy dialog
 * 
 * Dialog class for configuring an action and its children in a tree widget
 *
 * @author Thomas Kroes
 */
class ActionHierarchyDialog : public QDialog
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent widget
     * @param action Pointer to action to edit
     */
    ActionHierarchyDialog(QWidget* parent, WidgetAction* action);

    /** Get preferred size */
    QSize sizeHint() const override {
        return QSize(640, 480);
    }

    /** Get minimum size hint*/
    QSize minimumSizeHint() const override {
        return sizeHint();
    }

private:
    WidgetAction*   _action;    /** Pointer to action to edit */
};
