#pragma once

#include "PluginManagerModel.h"
#include "PluginManagerFilterModel.h"

#include <widgets/HierarchyWidget.h>

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
     * @param datasets Datasets for grouping
     */
    PluginManagerDialog(QWidget* parent = nullptr);

    /** Get preferred size */
    QSize sizeHint() const override {
        return QSize(800, 600);
    }

    /** Get minimum size hint*/
    QSize minimumSizeHint() const override {
        return sizeHint();
    }

protected:
    PluginManagerModel          _model;                 /** Plugin manager model (interfaces with a plugin manager) */
    PluginManagerFilterModel    _filterModel;           /** Sorting and filtering model for the plugin manager model */
    hdps::gui::HierarchyWidget  _hierarchyWidget;       /** Widget for displaying the loaded plugins */
};