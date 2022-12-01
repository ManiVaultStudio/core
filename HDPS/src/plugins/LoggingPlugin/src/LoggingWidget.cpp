#include "LoggingWidget.h"

#include <Application.h>

#include <QDebug>
#include <QHeaderView>
#include <QMenu>
#include <QGuiApplication>
#include <QAbstractEventDispatcher>

using namespace hdps;

LoggingWidget::LoggingWidget(QWidget* parent) :
    QWidget(parent),
    _model(),
    _filterModel(nullptr),
    _hierarchyWidget(this, "Log record", _model, &_filterModel),
    _idleUpdateConnection()
{
    setAutoFillBackground(true);

    auto layout = new QVBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(&_hierarchyWidget, 1);

    setLayout(layout);

    _hierarchyWidget.setWindowIcon(Application::getIconFont("FontAwesome").getIcon("scroll"));

    //_hierarchyWidget.getCollapseAllAction().setVisible(false);
    //_hierarchyWidget.getExpandAllAction().setVisible(false);

    auto& treeView = _hierarchyWidget.getTreeView();

    _idleUpdateConnection = connect(QAbstractEventDispatcher::instance(), &QAbstractEventDispatcher::awake, &_model, &LoggingModel::synchronizeLogRecords);

    //treeView.setRootIsDecorated(false);

    //qDebug() << "Row count: " << _model.rowCount();
    //_hierarchyWidget.setNoItemsDescription("Right-click > Import to load data into HDPS");

    //_treeView.setSortingEnabled(true);
    //_treeView.setContextMenuPolicy(Qt::CustomContextMenu);
    //_treeView.setModel(&_model);

    //_treeView.header()->setSortIndicator(-1, Qt::AscendingOrder);
    //
    //connect(&_treeView, &QTreeView::customContextMenuRequested, [this](const QPoint&)
    //    {
    //        const auto selectedRows = _treeView.selectionModel()->selectedRows();

    //        if (selectedRows.isEmpty())
    //            return;

    //        //const auto* const messageRecord = (row < 0) ? nullptr : _itemModel.GetMessageRecordAtRow(row);

    //        //QMenu contextMenu;

    //        //auto* const copyAction = contextMenu.addAction(tr("&Copy"), [this, messageRecord]
    //        //    {
    //        //        if (messageRecord != nullptr)
    //        //        {
    //        //            QString text;

    //        //            for (unsigned column{}; column < LogItemModel::numberOfColumns; ++column)
    //        //            {
    //        //                text.append(_itemModel.GetDataValueAtColumn(*messageRecord, column).toString())
    //        //                    .append(QLatin1Char('\t'));
    //        //            }
    //        //            text.back() = QLatin1Char('\n');

    //        //            QGuiApplication::clipboard()->setText(text);
    //        //        }
    //        //    });
    //        //copyAction->setEnabled(messageRecord != nullptr);
    //        //contextMenu.exec(QCursor::pos());
    //    });
}

QSize LoggingWidget::sizeHint() const
{
    return QSize(100, 100);
}
