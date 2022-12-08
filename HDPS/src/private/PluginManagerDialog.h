#pragma once

#include "LoadedPluginsModel.h"
#include "LoadedPluginsFilterModel.h"

#include <widgets/HierarchyWidget.h>

#include <QDialog>

/**
 * Loaded plugins dialog class
 * 
 * Dialog class for inspecting the loaded plugins and the created plugin instances
 *
 * @author Thomas Kroes
 */
class LoadedPluginsDialog : public QDialog
{
public:

    /**
     * Construct from \p parent
     * @param parent Pointer to parent widget
     * @param datasets Datasets for grouping
     */
    LoadedPluginsDialog(QWidget* parent = nullptr);

    /** Get preferred size */
    QSize sizeHint() const override {
        return QSize(800, 600);
    }

    /** Get minimum size hint*/
    QSize minimumSizeHint() const override {
        return sizeHint();
    }

protected:
    LoadedPluginsModel          _model;                 /** Model for loaded plugins (interfaces with a plugin manager) */
    LoadedPluginsFilterModel    _filterModel;           /** Sorting and filtering model the loaded plugins model */
    hdps::gui::HierarchyWidget  _hierarchyWidget;       /** Widget for displaying the loaded plugins */
};
