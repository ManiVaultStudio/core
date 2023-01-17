#pragma once

#include "StartPageActionsModel.h"
#include "StartPageActionsFilterModel.h"

#include <QStyledItemDelegate>
#include <QScopedPointer>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScopedPointer>
#include <QLabel>

/**
 * Workspace delegate class
 *
 * Delegate class for custom start page action display in a view.
 *
 * @author Thomas Kroes
 */
class StartPageActionDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    
    /**
     * Construct delegate with \p parent object
     * @param parent Pointer to parent object
     */
    StartPageActionDelegate(QObject* parent = nullptr);

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
    QWidget         _widget;
    QHBoxLayout     _mainLayout;
    QVBoxLayout     _iconLayout;
    QLabel          _iconLabel;
    QGridLayout     _textLayout;
    QLabel          _titleLabel;
    QLabel          _descriptionLabel;
    QLabel          _commentsLabel;
};
