#include "StartPageActionDelegate.h"

#include <QPainter>

#ifdef _DEBUG
    #define START_PAGE_ACTION_DELEGATE
#endif

StartPageActionDelegate::StartPageActionDelegate(QObject* parent /*= nullptr*/) :
    QStyledItemDelegate(parent),
    _widget(),
    _mainLayout(),
    _topLayout(),
    _titleLabel(),
    _descriptionLabel(),
    _tagsLayout(),
    _tagLabels()
{
    _widget.setLayout(&_mainLayout);
    _widget.setObjectName("StartPageActionDelegateWidget");

    _mainLayout.setContentsMargins(4, 4, 4, 4);
    _mainLayout.setSpacing(3);

    _mainLayout.addWidget(&_iconLabel);
    _mainLayout.addLayout(&_textLayout);

    _textLayout.addLayout(&_topLayout);
    _textLayout.addWidget(&_descriptionLabel);
    _textLayout.addLayout(&_tagsLayout);

    //_topLayout.setContentsMargins(0, 0, 0, 0);
    _topLayout.addWidget(&_titleLabel);
    _topLayout.addWidget(&_commentsLabel);

    _topLayout.addWidget(&_titleLabel, 1);
    _topLayout.addWidget(&_commentsLabel);

    _commentsLabel.setAlignment(Qt::AlignTop);

    _tagsLayout.setContentsMargins(0, 0, 0, 0);
    _tagsLayout.setAlignment(Qt::AlignLeft);
}

#include <Application.h>

void StartPageActionDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    auto nonConstThis = const_cast<StartPageActionDelegate*>(this);

    nonConstThis->_widget.setStyleSheet(QString("QWidget#StartPageActionDelegateWidget { background-color: %1; }").arg(option.state & QStyle::State_MouseOver ? "rgba(0, 0, 0, 100)" : "transparent"));

    nonConstThis->_iconLabel.setPixmap(hdps::Application::getIconFont("FontAwesome").getIcon("file").pixmap(QSize(24, 24)));

    nonConstThis->_tagLabels.clear();

    nonConstThis->_titleLabel.setText(index.siblingAtColumn(static_cast<int>(StartPageActionsModel::Column::Title)).data(Qt::DisplayRole).toString());
    nonConstThis->_titleLabel.setStyleSheet("font-weight: bold;");

    const auto description = index.siblingAtColumn(static_cast<int>(StartPageActionsModel::Column::Description)).data(Qt::DisplayRole).toString();

    QFontMetrics metrix(nonConstThis->_descriptionLabel.font());
    
    const auto width = nonConstThis->_descriptionLabel.width() - 2;

    QString clippedText = metrix.elidedText(description, Qt::ElideMiddle, width);

    nonConstThis->_descriptionLabel.setText(clippedText);
    nonConstThis->_descriptionLabel.setHidden(description.isEmpty());

    nonConstThis->_commentsLabel.setText(index.siblingAtColumn(static_cast<int>(StartPageActionsModel::Column::Comments)).data(Qt::DisplayRole).toString());

    //const auto tags = index.siblingAtColumn(static_cast<int>(StartPageActionsModel::Column::Tags)).data(Qt::UserRole + 1).toStringList();

    //for (const auto& tag : tags) {
    //    auto label = new QLabel(tag);

    //    label->setStyleSheet("QLabel { \
    //        padding: 2px; \
    //        font-size: 8pt; \
    //        border-radius: 3px; \
    //        background: rgb(150, 150, 150); \
    //        color: rgb(40, 40, 40); \
    //    }");

    //    label->setMargin(1);

    //    nonConstThis->_tagLabels << QSharedPointer<QLabel>(label);
    //}

    //for (auto& tagLabel : nonConstThis->_tagLabels)
    //    nonConstThis->_tagsLayout.addWidget(tagLabel.get());

    nonConstThis->_widget.setFixedWidth(option.rect.size().width());

    painter->save();
    painter->translate(option.rect.topLeft());
    
    nonConstThis->_widget.render(painter, QPoint(), QRegion(), QWidget::DrawChildren);

    painter->restore();
}

QSize StartPageActionDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    return _widget.minimumSizeHint();
}
