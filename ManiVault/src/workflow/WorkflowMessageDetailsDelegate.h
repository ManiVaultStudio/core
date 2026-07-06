// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"

#include <QList>
#include <QStandardItem>

namespace mv::workflow
{

/**
 * @brief Delegate for viewing workflow message details.
 *
 * Provides an interactive visualization for the details associated with a
 * workflow message. Cells containing diagnostic information are rendered as an
 * action that, when activated, opens a dialog for browsing the underlying
 * QVariant-based data in a hierarchical tree view. Messages without additional
 * details are rendered using the default delegate behavior.
 *
 * The delegate is intended for use with the details column of the workflow
 * messages view.
 */
class WorkflowMessageDetailsDelegate final : public QStyledItemDelegate
{
    Q_OBJECT

public:
    using QStyledItemDelegate::QStyledItemDelegate;

    /**
     * @brief Paints the details cell.
     *
     * Renders the default cell appearance for messages without details and a
     * button-like control for messages containing diagnostic information.
     *
     * @param painter Painter used to render the item.
     * @param option Style options describing the item.
     * @param index Model index identifying the item to paint.
     */
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

    /**
     * @brief Handles user interaction with the details cell.
     *
     * Detects activation of the details action and opens a dialog for
     * inspecting the associated diagnostic information.
     *
     * @param event User input event.
     * @param model Model containing the item.
     * @param option Style options describing the item.
     * @param index Model index identifying the item.
     *
     * @return True if the event was handled; otherwise false.
     */
    bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index) override;

private:

    /**
	 * @brief Opens a browser for inspecting workflow message details.
	 *
	 * Presents the supplied diagnostic details in a hierarchical view, allowing
	 * nested QVariantMap and QVariantList values to be explored interactively.
	 *
	 * @param details Diagnostic details to display.
	 */
    static void showDetailsBrowser(const QVariantMap& details);

    /**
     * @brief Populates a tree widget with QVariant data.
     *
     * Recursively converts QVariantMap and QVariantList values into a tree
     * representation. Primitive values are displayed as leaf nodes.
     *
     * @param parent Parent tree item to populate.
     * @param value QVariant value to add to the tree.
     */
    static void populateTree(QTreeWidgetItem* parent, const QVariant& value);
};

} // namespace mv::workflow