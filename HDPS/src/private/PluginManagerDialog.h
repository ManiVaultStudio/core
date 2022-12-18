#pragma once

#include "PluginManagerModel.h"
#include "PluginManagerFilterModel.h"

#include <widgets/HierarchyWidget.h>

#include <actions/TriggerAction.h>

#include <QDialog>

/**
 * Plugin manager dialog class
 * 
 * Dialog class for inspecting the loaded plugins and the created plugin instances.
 * It shows the plugin factories at the top level and the plugin instances below it.
 *
 * @author Thomas Kroes
 */
class PluginManagerDialog : public QDialog
{
public:

    /**
     * Construct a plugin manager dialog with \p parent
     * @param parent Pointer to parent widget
     */
    PluginManagerDialog(QWidget* parent = nullptr);

    /** Get preferred size */
    QSize sizeHint() const override {
        return QSize(640, 320);
    }

    /** Get minimum size hint*/
    QSize minimumSizeHint() const override {
        return sizeHint();
    }

    /** Show the plugin manager dialog modally */
    static void managePlugins();

private:
    PluginManagerModel          _model;                 /** Plugin manager model (interfaces with a plugin manager) */
    PluginManagerFilterModel    _filterModel;           /** Sorting and filtering model for the plugin manager model */
    hdps::gui::HierarchyWidget  _hierarchyWidget;       /** Widget for displaying the loaded plugins */
    hdps::gui::TriggerAction    _okAction;              /** Action for exiting the dialog */
};
