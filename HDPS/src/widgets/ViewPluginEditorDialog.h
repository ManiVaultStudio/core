#pragma once

#include "ActionsWidget.h"

#include <actions/GroupsAction.h>
#include <actions/GroupAction.h>

#include <QDialog>

using namespace hdps::gui;

namespace hdps {
    namespace plugin {
        class ViewPlugin;
    }
}

/**
 * View plugin editor dialog class
 * 
 * Dialog class for editing view plugin properties such as the actions and docking properties
 *
 * @author Thomas Kroes
 */
class ViewPluginEditorDialog : public QDialog
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent widget
     * @param viewPlugin Pointer to view plugin to edit
     */
    ViewPluginEditorDialog(QWidget* parent, hdps::plugin::ViewPlugin* viewPlugin);

    /** Get preferred size */
    QSize sizeHint() const override {
        return QSize(640, 480);
    }

    /** Get minimum size hint*/
    QSize minimumSizeHint() const override {
        return sizeHint();
    }

private:
    GroupsAction    _groupsAction;      /** Groups action */
    ActionsWidget   _actionsWidget;     /** Hierarchical actions widget */
};
