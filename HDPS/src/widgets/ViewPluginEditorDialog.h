#pragma once

#include "ActionsWidget.h"

#include "models/ActionsListModel.h"
#include "models/ActionsHierarchyModel.h"

#include "actions/GroupsAction.h"
#include "actions/GroupAction.h"
#include "actions/GroupAction.h"

#include <QDialog>

namespace hdps {
    namespace plugin {
        class ViewPlugin;
    }
}

namespace hdps::gui {

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
    GroupsAction            _groupsAction;              /** Groups action */
    ActionsListModel        _actionsListModel;          /** List actions model of the view plugin action and its descendants */
    ActionsHierarchyModel   _actionsHierarchyModel;     /** Hierarchical actions model of the view plugin action and its descendants */
    ActionsWidget           _actionsHierarchyWidget;    /** Actions hierarchy widget (displays the actions hierarchy model) */
    GroupAction             _settingsAction;            /** Group action for miscellaneous settings actions */
};

}