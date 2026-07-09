// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"

#include <QList>
#include <QStandardItem>
#include <QStyledItemDelegate>
#include <QVariant>
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

    // paint() is intentionally not overridden; the model provides the icon via Qt::DecorationRole.
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
    void showDetailsBrowser(const QVariantMap& details);

    static void populateModel(
        QStandardItem* parent,
        const QString& key,
        const QVariant& value);

    static QList<QStandardItem*> makeRow(
        const QString& key,
        const QString& value = {});
};

} // namespace mv::workflow