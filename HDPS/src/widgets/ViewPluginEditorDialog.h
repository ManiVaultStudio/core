#pragma once

#include "ActionHierarchyWidget.h"

#include <actions/WidgetAction.h>
#include <actions/ToggleAction.h>

#include <QDialog>

using namespace hdps::gui;

/**
 * View plugin editor hierarchy dialog
 * 
 * Dialog class for configuring a view plugin
 *
 * @author Thomas Kroes
 */
class ViewPluginEditorDialog : public QDialog
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent widget
     * @param rootAction Pointer to non-owning root action
     */
    ViewPluginEditorDialog(QWidget* parent, WidgetAction* rootAction);

    /** Get preferred size */
    QSize sizeHint() const override {
        return QSize(640, 480);
    }

    /** Get minimum size hint*/
    QSize minimumSizeHint() const override {
        return sizeHint();
    }

private:
    WidgetAction*           _action;                    /** Pointer to view plugin action to edit */
    ActionHierarchyWidget   _actionHierarchyWidget;     /** Widget for displaying an action hierarchy */
    ToggleAction            _mayCloseAction;            /** Action for toggling whether a view plugin may be closed */
};
