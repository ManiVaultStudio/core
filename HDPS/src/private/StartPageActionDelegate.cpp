#include "StartPageActionDelegate.h"

#include <Application.h>

#include <QPainter>

#ifdef _DEBUG
    #define START_PAGE_ACTION_DELEGATE
#endif

using namespace hdps;

StartPageActionDelegate::StartPageActionDelegate(QObject* parent /*= nullptr*/) :
    QStyledItemDelegate(parent),
    _widget(new QWidget())
{
    

    //_mainLayout.setContentsMargins(4, 4, 4, 4);
    //_mainLayout.setSpacing(3);
    //_mainLayout.setAlignment(Qt::AlignTop);

    //_mainLayout.addWidget(&_iconLabel);
    //_mainLayout.addSpacing(4);
    //_mainLayout.addLayout(&_textLayout);
    //_mainLayout.addLayout(&_rightIconsLayout);

    //_iconLabel.setStyleSheet("background-color: red;");
    //_titleLabel.setStyleSheet("background-color: red;");
    //_descriptionLabel.setStyleSheet("background-color: red;");
    //_iconLabel.setAlignment(Qt::AlignTop);

    //_textLayout.setContentsMargins(0, 0, 0, 0);
    //_topLayout.setContentsMargins(0, 0, 0, 0);
    //_topLayout.setAlignment(Qt::AlignTop);
    //_textLayout.setAlignment(Qt::AlignTop);

    //_textLayout.addLayout(&_topLayout);
    //_textLayout.addWidget(&_descriptionLabel);
    //_textLayout.addLayout(&_tagsLayout);

    ////_topLayout.setContentsMargins(0, 0, 0, 0);
    //_topLayout.addWidget(&_titleLabel);
    //_topLayout.addWidget(&_commentsLabel);

    //_topLayout.addWidget(&_titleLabel, 1);
    //_topLayout.addWidget(&_commentsLabel);

    //_commentsLabel.setAlignment(Qt::AlignTop);

    //_tagsLayout.setContentsMargins(0, 0, 0, 0);
    //_tagsLayout.setAlignment(Qt::AlignLeft);
}

void StartPageActionDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    auto widget = const_cast<StartPageActionDelegate*>(this)->getWidget(option, index);

    widget->setFixedWidth(option.rect.size().width());

    painter->save();
    painter->translate(option.rect.topLeft());

    widget->render(painter, QPoint(), QRegion(), QWidget::DrawChildren);

    painter->restore();

    /*

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

    nonConstThis->_widget.setFixedWidth(option.rect.size().width());

    painter->save();
    painter->translate(option.rect.topLeft());
    
    nonConstThis->_widget.render(painter, QPoint(), QRegion(), QWidget::DrawChildren);

    painter->restore();
    */
}

QSize StartPageActionDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    return const_cast<StartPageActionDelegate*>(this)->getWidget(option, index)->minimumSizeHint();
}

QWidget* StartPageActionDelegate::getWidget(const QStyleOptionViewItem& option, const QModelIndex& index)
{
    auto widget             = new QWidget();
    auto mainLayout         = new QHBoxLayout();
    auto leftIconsLayout    = new QVBoxLayout();
    auto gridLayout         = new QGridLayout();

    mainLayout->setContentsMargins(0, 0, 0, 0);

    QMargins margins(3, 3, 3, 3);

    leftIconsLayout->setContentsMargins(margins);
    gridLayout->setContentsMargins(margins);

    leftIconsLayout->setAlignment(Qt::AlignTop);
    gridLayout->setAlignment(Qt::AlignTop);

    mainLayout->addLayout(leftIconsLayout);
    mainLayout->addLayout(gridLayout);

    widget->setObjectName("StartPageActionDelegateWidget");
    widget->setStyleSheet(QString("QWidget#StartPageActionDelegateWidget { background-color: %1; }").arg(option.state & QStyle::State_MouseOver ? "rgba(0, 0, 0, 100)" : "transparent"));
    widget->setLayout(mainLayout);

    const auto getIconLabel = [](const QIcon& icon, const QSize& size = QSize(24, 24)) -> QLabel* {
        auto label = new QLabel();

        label->setPixmap(icon.pixmap(size));

        return label;
    };

    // Left icons
    {
        leftIconsLayout->addWidget(getIconLabel(index.siblingAtColumn(static_cast<int>(StartPageActionsModel::Column::Icon)).data(Qt::UserRole + 1).value<QIcon>()));
    }

    // Text
    {
        const auto title        = index.siblingAtColumn(static_cast<int>(StartPageActionsModel::Column::Title)).data(Qt::EditRole).toString();
        const auto description  = index.siblingAtColumn(static_cast<int>(StartPageActionsModel::Column::Description)).data(Qt::EditRole).toString();
        const auto comments     = index.siblingAtColumn(static_cast<int>(StartPageActionsModel::Column::Comments)).data(Qt::EditRole).toString();

        auto titleLabel         = new QLabel(title);
        auto commentsLabel      = new QLabel(comments);
        auto descriptionLabel   = new QLabel(description);

        titleLabel->setStyleSheet("font-weight: bold");

        gridLayout->addWidget(titleLabel, 0, 0);
        gridLayout->addWidget(commentsLabel, 0, 1);
        gridLayout->addWidget(descriptionLabel, 1, 0, 1, 2);

        QFontMetrics titleMetrics(titleLabel->font()), descriptionMetrics(descriptionLabel->font());

        qDebug() << titleLabel->width() << descriptionLabel->width();

        titleLabel->setText(titleMetrics.elidedText(title, Qt::ElideMiddle, titleLabel->width() - 2));
        descriptionLabel->setText(descriptionMetrics.elidedText(description, Qt::ElideMiddle, descriptionLabel->width() - 2));

        gridLayout->setColumnStretch(0, 1);
    }

    

    return widget;
}
