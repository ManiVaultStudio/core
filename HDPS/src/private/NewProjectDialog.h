#pragma once

#include <QDialog>

#include "WorkspaceInventoryModel.h"

#include <widgets/HierarchyWidget.h>

#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QStyledItemDelegate>

/**
 * Workspace delegate class
 *
 * Delegate class for custom workspace display in a cell.
 *
 * @author Thomas Kroes
 */
class WorkspaceDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    using QStyledItemDelegate::QStyledItemDelegate;

    /**
     * Paint the delegate using \p painter, \p option and model \p index
     * @param painter Pointer to painter
     * @param option Style option
     * @param index Model index to paint
     */
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

    /**
     * Gives the size hint for \p option and model \p index
     * @param option Style option
     * @param index Model index to compute the size hint for
     * @return Size hint
     */
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;

private:

    /**
     * Get HTML for \p index
     * @return HTML string
     */
    QString getHtml(const QModelIndex& index) const;
};

/**
 * New project dialog class
 * 
 * Dialog class for creating new projects.
 * 
 * @author Thomas Kroes
 */
class NewProjectDialog final : public QDialog
{
public:

    /**
     * Construct a dialog with \p parent
     * @param parent Pointer to parent widget
     */
    NewProjectDialog(QWidget* parent = nullptr);

    /** Get preferred size */
    QSize sizeHint() const override {
        return QSize(600, 400);
    }

    /** Get minimum size hint*/
    QSize minimumSizeHint() const override {
        return sizeHint();
    }

private:
    WorkspaceInventoryModel     _model;                 /** Inventory model which contains available workspaces */
    QSortFilterProxyModel       _filterModel;           /** Sorting and filtering model for the workspaces model */
    hdps::gui::HierarchyWidget  _hierarchyWidget;       /** Widget for displaying the available workspaces */
    hdps::gui::TriggerAction    _createAction;          /** Action for creating the project with the selected workspace */
    hdps::gui::TriggerAction    _cancelAction;          /** Action for exiting the dialog without creating a new project */
};
