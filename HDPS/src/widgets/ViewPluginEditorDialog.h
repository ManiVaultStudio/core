#pragma once

#include "ActionHierarchyWidget.h"

#include <actions/WidgetAction.h>
#include <actions/ToggleAction.h>

#include <QDialog>

using namespace hdps::gui;

namespace hdps {
    namespace plugin {
        class ViewPlugin;
    }
}

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
    hdps::plugin::ViewPlugin*   _viewPlugin;                /** Pointer to view plugin to edit */
    //ActionHierarchyWidget       _actionHierarchyWidget;     /** Widget for displaying an action hierarchy */
};
