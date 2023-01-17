#include "StartPageActionDelegate.h"

#include <Application.h>

#include <QPainter>

#ifdef _DEBUG
    #define START_PAGE_ACTION_DELEGATE
#endif

using namespace hdps;

StartPageActionDelegate::StartPageActionDelegate(QObject* parent /*= nullptr*/) :
    QStyledItemDelegate(parent)
{
    _widget.setObjectName("StartPageActionDelegateWidget");

    _mainLayout.setContentsMargins(1, 1, 1, 1);
    _mainLayout.setSpacing(0);

    QMargins margins(3, 3, 3, 3);

    _iconLayout.setContentsMargins(margins);
    _textLayout.setContentsMargins(margins);

    _iconLayout.setAlignment(Qt::AlignTop);

    _textLayout.setAlignment(Qt::AlignTop);
    _textLayout.setSpacing(1);
    _textLayout.setColumnStretch(0, 1);

    _mainLayout.addLayout(&_iconLayout);
    _mainLayout.addLayout(&_textLayout);

    _iconLayout.addWidget(&_iconLabel);

    _iconLabel.setStyleSheet("padding-top: 3px;");
    _titleLabel.setStyleSheet("font-weight: bold;");

    _textLayout.addWidget(&_titleLabel, 0, 0);
    _textLayout.addWidget(&_commentsLabel, 0, 1);
    _textLayout.addWidget(&_descriptionLabel, 1, 0, 1, 2);
    
    _widget.setLayout(&_mainLayout);
}

void StartPageActionDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    auto nonConstThis = const_cast<StartPageActionDelegate*>(this);

    nonConstThis->_widget.setStyleSheet(QString("QWidget#StartPageActionDelegateWidget { background-color: %1; }").arg(option.state & QStyle::State_MouseOver ? "rgba(0, 0, 0, 100)" : "transparent"));
    nonConstThis->_widget.setFixedWidth(option.rect.size().width());

    nonConstThis->_iconLabel.setPixmap(index.siblingAtColumn(static_cast<int>(StartPageActionsModel::Column::Icon)).data(Qt::UserRole + 1).value<QIcon>().pixmap(QSize(24, 24)));

    const auto title        = index.siblingAtColumn(static_cast<int>(StartPageActionsModel::Column::Title)).data(Qt::EditRole).toString();
    const auto description  = index.siblingAtColumn(static_cast<int>(StartPageActionsModel::Column::Description)).data(Qt::EditRole).toString();
    const auto comments     = index.siblingAtColumn(static_cast<int>(StartPageActionsModel::Column::Comments)).data(Qt::EditRole).toString();

    QFontMetrics titleMetrics(_titleLabel.font()), descriptionMetrics(_descriptionLabel.font());

    nonConstThis->_titleLabel.setText(titleMetrics.elidedText(title, Qt::ElideMiddle, nonConstThis->_titleLabel.width() - 2));
    nonConstThis->_descriptionLabel.setText(descriptionMetrics.elidedText(description, Qt::ElideMiddle, nonConstThis->_descriptionLabel.width() - 2));
    nonConstThis->_commentsLabel.setText(comments);

    painter->save();
    painter->translate(option.rect.topLeft());

    nonConstThis->_widget.render(painter, QPoint(), QRegion(), QWidget::DrawChildren);

    painter->restore();
}

QSize StartPageActionDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    return const_cast<StartPageActionDelegate*>(this)->_widget.minimumSizeHint();
}
