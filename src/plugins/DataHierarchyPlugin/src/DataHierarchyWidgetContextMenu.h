// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <Dataset.h>
#include <PluginType.h>

#include <actions/IntegralAction.h>
#include <actions/OptionAction.h>
#include <actions/StringAction.h>
#include <actions/TriggerAction.h>

#include <QMenu>
#include <QDialog>

class QAction;

/**
 * @brief Context menu for selected data hierarchy datasets.
 * 
 * DataHierarchyWidgetContextMenu builds the actions available for the current
 * dataset selection, including plugin launch menus, script menus, grouping,
 * locking, visibility, and selection-based grouping helpers.
 * 
 * @maintainer Thomas Kroes, Alexander Vieth
 */
class DataHierarchyWidgetContextMenu final : public QMenu
{
public:

    /**
     * @brief Constructs a context menu for the selected datasets.
     * @param parent Parent widget.
     * @param selectedDatasets Selected datasets in the data hierarchy widget.
     */
    DataHierarchyWidgetContextMenu(QWidget* parent, mv::Datasets selectedDatasets);

private:

    /**
     * @brief Adds plugin menus for a plugin type.
     *
     * Loaded plugins are grouped into a hierarchical menu structure based on
     * plugin title.
     *
     * @param pluginType Plugin type to add menus for.
     */
    void addMenusForPluginType(mv::plugin::Type pluginType);

    /** Adds menus for available scripts. */
    void addMenusForScripts();

    /**
     * @brief Creates the item grouping action.
     * @return Action for grouping selected items.
     */
    QAction* getGroupAction();

    /**
     * @brief Creates the selection grouping action.
     * @return Action for grouping by selection index.
     */
    QAction* getSelectionGroupAction();

    /**
     * @brief Creates the selection-pattern grouping action.
     * @return Action for grouping by selection pattern.
     */
    QAction* getSelectionGroupPatternAction();

    /**
     * @brief Creates the item locking menu.
     * @return Menu for locking selected items.
     */
    QMenu* getLockMenu();

    /**
     * @brief Creates the item unlocking menu.
     * @return Menu for unlocking selected items.
     */
    QMenu* getUnlockMenu();

    /**
     * @brief Creates the item hiding menu.
     * @return Menu for hiding selected items.
     */
    QMenu* getHideMenu();

    /**
     * @brief Creates the item unhiding menu.
     * @return Menu for unhiding selected items.
     */
    QMenu* getUnhideMenu();

private:

    mv::Datasets    _allDatasets;           /**< All datasets in the data hierarchy */
    mv::Datasets    _selectedDatasets;      /**< Selected datasets in the data hierarchy widget */
};

/**
 * @brief Dialog for choosing a selection group index.
 *
 * @maintainer Alexander Vieth
 */
class SelectionGroupIndexDialog : public QDialog
{
    Q_OBJECT

public:

    /**
     * @brief Constructs a selection group index dialog.
     * @param parent Parent widget.
     */
    SelectionGroupIndexDialog(QWidget* parent);

    /**
     * @brief Returns the selected group index.
     * @return Selection group index.
     */
    [[nodiscard]] std::int32_t getSelectionGroupIndex() const {
        return _selectionIndexAction.getValue();
    }

signals:

    /**
     * @brief Emitted when the dialog should close.
     * @param onlyIndices Whether only selection indices should be used.
     */
    void closeDialog(bool onlyIndices);

private:

    mv::gui::IntegralAction      _selectionIndexAction;      /**< Action for setting the selection group index */
    mv::gui::TriggerAction       _confirmAction;             /**< Action that confirms grouping */
};

/**
 * @brief Dialog for choosing a selection group index and pattern.
 *
 * @maintainer Alexander Vieth
 */
class SelectionPatternGroupIndexDialog : public QDialog
{
    Q_OBJECT

public:

    /**
     * @brief Constructs a selection pattern group dialog.
     * @param parent Parent widget.
     */
    SelectionPatternGroupIndexDialog(QWidget* parent);

    /**
     * @brief Returns the selected group index.
     * @return Selection group index.
     */
    [[nodiscard]] std::int32_t getSelectionGroupIndex() const {
        return _selectionIndexAction.getValue();
    }

    /**
     * @brief Returns the selected grouping pattern.
     * @return Selection group pattern.
     */
    [[nodiscard]] QString getSelectionGroupPattern() const {
        return _selectionPatternAction.getString();
    }

    /**
     * @brief Returns the selected pattern matching option.
     * @return Selection group option index.
     */
    [[nodiscard]] std::int32_t getSelectionGroupOption() const {
        return _selectionOptionAction.getCurrentIndex();
    }

signals:

    /**
     * @brief Emitted when the dialog should close.
     * @param onlyIndices Whether only selection indices should be used.
     */
    void closeDialog(bool onlyIndices);

private:

    mv::gui::IntegralAction     _selectionIndexAction;      /**< Action for setting the selection group index */
    mv::gui::StringAction       _selectionPatternAction;    /**< Action for setting the selection group pattern */
    mv::gui::OptionAction       _selectionOptionAction;     /**< Action for choosing prefix or suffix matching */
    mv::gui::StringAction       _infoTextAction;            /**< Action for displaying explanatory text */
    mv::gui::TriggerAction      _confirmAction;             /**< Action that confirms grouping */
};
