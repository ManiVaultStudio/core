#pragma once

#include <QStyledItemDelegate>

class DataHierarchyTreeWidgetItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    DataHierarchyTreeWidgetItemDelegate(QObject* parent = nullptr);

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
};
