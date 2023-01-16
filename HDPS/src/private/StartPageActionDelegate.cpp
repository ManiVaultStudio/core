#include "StartPageActionDelegate.h"

#include <QPainter>
#include <QTextDocument>

#ifdef _DEBUG
    #define START_PAGE_ACTION_DELEGATE
#endif

void StartPageActionDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QStyleOptionViewItem options = option;

    initStyleOption(&options, index);

    painter->save();

    QTextDocument textDocument;

    textDocument.setHtml(getHtml(index));

    options.text = "";
    options.widget->style()->drawControl(QStyle::CE_ItemViewItem, &options, painter);

    painter->translate(options.rect.left(), options.rect.top());

    QRect clip(0, 0, options.rect.width(), options.rect.height());

    textDocument.drawContents(painter, clip);

    painter->restore();

    QStyledItemDelegate::paint(painter, option, index);
}

QSize StartPageActionDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QStyleOptionViewItem options = option;

    initStyleOption(&options, index);

    QTextDocument textDocument;

    textDocument.setHtml(getHtml(index));

    textDocument.setTextWidth(options.rect.width());

    return QSize(textDocument.idealWidth(), textDocument.size().height());
}

QString StartPageActionDelegate::getHtml(const QModelIndex& index) const
{
    QString style = " \
        <style> \
            body { \
                padding: 25px; \
            } \
            span#tag { \
                background-color: silver; \
            } \
            p { \
                margin: 3px; \
            } \
            p#tags { \
                margin-top: 5px; \
            } \
        </style>";

    QString title, subTitle;

    switch (static_cast<StartPageActionsModel::Column>(index.column())) {
        case StartPageActionsModel::Column::SummaryDelegate:
        {
            title       = "<b>" + index.siblingAtColumn(static_cast<int>(StartPageActionsModel::Column::Title)).data(Qt::DisplayRole).toString() + "</b>";
            subTitle    = index.siblingAtColumn(static_cast<int>(StartPageActionsModel::Column::Description)).data(Qt::DisplayRole).toString();

            if (subTitle.isEmpty())
                subTitle = "No description";

            break;
        }

        case StartPageActionsModel::Column::CommentsDelegate:
        {
            title       = index.siblingAtColumn(static_cast<int>(StartPageActionsModel::Column::Comments)).data(Qt::DisplayRole).toString().replace(" ", "&nbsp;") + "&nbsp;";
            subTitle    = "";

            break;
        }
    }
 
    return QString("\
        <html> \
            <head> \
            %1 \
            </head> \
            <body> \
                <p>%2</p> \
                <p>%3</p> \
            </body> \
        </html> \
    ").arg(style, title, subTitle);
}
