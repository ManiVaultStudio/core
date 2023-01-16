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
 * Delegate class for custom start page action display in a cell.
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
    QWidget                         _widget;                    /** Delegate widget */
    QHBoxLayout                     _mainLayout;                /** Delegate widget layout */
    QLabel                          _iconLabel;                 /** Label for the left icon */
    QVBoxLayout                     _textLayout;                /** Layout for the text labels on the right */
    QHBoxLayout                     _topLayout;                 /** Layout for top labels */
    QLabel                          _titleLabel;                /** Title label */
    QLabel                          _descriptionLabel;          /** Description label */
    QLabel                          _commentsLabel;             /** Comments label */
    QHBoxLayout                     _tagsLayout;                /** Tags layout */
    QList<QSharedPointer<QLabel>>   _tagLabels;                 /** Tag labels */
};
