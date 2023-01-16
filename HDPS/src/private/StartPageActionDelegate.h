#pragma once

#include "StartPageActionsModel.h"
#include "StartPageActionsFilterModel.h"

#include <QStyledItemDelegate>

/**
 * Workspace delegate class
 *
 * Delegate class for custom start page action display in a cell.
 *
 * @author Thomas Kroes
 */
class StartPageActionDelegate : public QStyledItemDelegate
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
