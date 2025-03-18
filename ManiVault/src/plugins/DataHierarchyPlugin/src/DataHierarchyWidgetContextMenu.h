// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <Dataset.h>
#include <PluginType.h>

#include <QMenu>

#include <actions/TriggerAction.h>
#include <actions/IntegralAction.h>

class QAction;

using namespace mv;
using namespace mv::plugin;

/**
 * Data hierarchy widget context menu
 * 
 * Constructs a data hierarchy widget context menu based on the current dataset selection
 * 
 * @author Thomas Kroes
 */
class DataHierarchyWidgetContextMenu final : public QMenu
{
public:

    /**
     * Construct with \p parent widget and \p selectedDatasets
     * @param parent Parent widget
     * @param selectedDatasets Selected datasets in the data hierarchy widget
     */
    DataHierarchyWidgetContextMenu(QWidget* parent, Datasets selectedDatasets);

private:

    /**
     * Add menus for \p pluginType
     * Goes over all loaded plugins of \p pluginType and builds a hierarchical menu structure (based on plugin title)
     * @parsm Pointer to menu for \p pluginType
     */
    void addMenusForPluginType(plugin::Type pluginType);

    /**
     * Get action for item grouping
     * @return Pointer to action for item grouping
     */
    QAction* getGroupAction();

    /**
     * Get action for selection grouping
     * @return Pointer to action for selection grouping
     */
    QAction* getSelectionGroupAction();

    /**
     * Get menu for item locking
     * @return Pointer to menu for item locking
     */
    QMenu* getLockMenu();

    /**
     * Get menu for item unlocking
     * @return Pointer to menu for item unlocking
     */
    QMenu* getUnlockMenu();

    /**
     * Get menu for item hiding
     * @return Pointer to menu for item hiding
     */
    QMenu* getHideMenu();

    /**
     * Get menu for item unhiding
     * @return Pointer to menu for item unhiding
     */
    QMenu* getUnhideMenu();

private:
    Datasets    _allDatasets;           /** All datasets in the data hierarchy */
    Datasets    _selectedDatasets;      /** Selected datasets in the data hierarchy widget */
};

/**
 * Helper dialog for selection index selection
 */
class SelectionGroupIndexDialog : public QDialog
{
    Q_OBJECT
public:
    SelectionGroupIndexDialog(QWidget* parent);

    std::int32_t getSelectionGroupIndex() {
        return selectionIndexAction.getValue();
    }

signals:
    void closeDialog(bool onlyIndices);

public slots:
    // Pass selected data set name from SelectionGroupIndexDialog to BinExporter (dialogClosed)
    void closeDialogAction() {
        emit closeDialog(confirmButton.isChecked());
    }

private:
    gui::IntegralAction      selectionIndexAction;
    gui::TriggerAction       confirmButton;
};
