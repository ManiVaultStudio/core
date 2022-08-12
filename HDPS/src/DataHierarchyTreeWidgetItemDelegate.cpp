#include "DataHierarchyTreeWidgetItemDelegate.h"
#include "DataHierarchyModelItem.h"

#include <QDebug>
#include <QPainter>

using namespace hdps;

DataHierarchyTreeWidgetItemDelegate::DataHierarchyTreeWidgetItemDelegate(QObject* parent /*= nullptr*/) :
    QStyledItemDelegate(parent)
{
}

void DataHierarchyTreeWidgetItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    //QItemDelegate::paint(painter, option, index);

    const auto name = index.model()->data(index, Qt::DisplayRole).toString();

    const auto supplementaryIcons = QList<QIcon>({
        qvariant_cast<QIcon>(index.model()->data(index, DataHierarchyModelItem::SupplementaryIcons::Icon1)),
        qvariant_cast<QIcon>(index.model()->data(index, DataHierarchyModelItem::SupplementaryIcons::Icon2)),
        qvariant_cast<QIcon>(index.model()->data(index, DataHierarchyModelItem::SupplementaryIcons::Icon3))
    });
    
    QList<QIcon> icons{ qvariant_cast<QIcon>(index.model()->data(index, Qt::DecorationRole)) };

    for (const auto& supplementaryIcon : supplementaryIcons)
        if (!supplementaryIcon.isNull())
            icons << supplementaryIcon;

    QStyleOptionViewItem styleOption;

     QStyleOptionViewItem myoption = option;

    //drawBackground(painter, myoption, index);

     if (option.state & QStyle::State_MouseOver) {
         auto brush = index.data(Qt::BackgroundRole);
         qDebug() << brush;
         painter->fillRect(option.rect, myoption.palette.color(QPalette::Current, QPalette::Highlight));
         //QItemDelegate::paint(painter, myoption, index);
     }

    const auto spacing = 3u;

    auto offset = 0u;

    for (const auto& icon : icons) {
        //drawDecoration(painter, myoption, QRect(option.rect.topLeft() + QPoint(offset + 2, 2), option.decorationSize), icon.pixmap(option.decorationSize));
        offset += spacing + option.decorationSize.width();
    }

    //auto displayRect = option.rect;

    //displayRect.setTopLeft(option.rect.topLeft() + QPoint(offset + spacing, 0));

    //drawDisplay(painter, myoption, displayRect, name);

    QStyledItemDelegate::paint(painter, option, index);

    //QStyleOptionViewItem itemOption(option);

    //initStyleOption(&itemOption, index);
    //itemOption.text = "Test Text";  // override text

    //QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &itemOption, painter, nullptr);
}
