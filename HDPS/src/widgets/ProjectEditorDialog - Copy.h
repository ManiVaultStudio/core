#pragma once

#include "ActionHierarchyWidget.h"

#include <actions/GroupsAction.h>

#include <QDialog>

using namespace hdps::gui;

namespace hdps {
    namespace plugin {
        class ViewPlugin;
    }
}

/**
 * Project editor editor dialog
 * 
 * Dialog class for editing project properties
 *
 * @author Thomas Kroes
 */
class ProjectEditorDialog : public QDialog
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent widget
     * @param viewPlugin Pointer to view plugin to edit
     */
    ProjectEditorDialog(QWidget* parent, hdps::plugin::ViewPlugin* viewPlugin);

    /** Get preferred size */
    QSize sizeHint() const override {
        return QSize(640, 480);
    }

    /** Get minimum size hint*/
    QSize minimumSizeHint() const override {
        return sizeHint();
    }

private:
    hdps::plugin::ViewPlugin*   _viewPlugin;                /** Pointer to view plugin to edit */
    ActionHierarchyWidget       _actionHierarchyWidget;     /** Widget for displaying an action hierarchy */
};
