// Its own header file:
#include "LogDockWidget.h"

#include "LogItemModel.h"

// Qt header files:
#include <QAbstractEventDispatcher>
#include <QHeaderView>
#include <QMainWindow>
#include <QTreeView>


namespace hdps
{
namespace gui
{

class LogDockWidget::Data
{
private:
    LogItemModel _itemModel;
    QMetaObject::Connection m_awakeConnection;

public:
    explicit Data(QDockWidget& dockWidget)
    {
        auto* treeView = new QTreeView(&dockWidget);
        dockWidget.setWidget(treeView);

        treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        treeView->setRootIsDecorated(false);
        treeView->setItemsExpandable(false);
        treeView->setSortingEnabled(true);
        treeView->setExpandsOnDoubleClick(false);
        treeView->header()->setSortIndicator(-1, Qt::AscendingOrder);
        treeView->setModel(&_itemModel);

        const auto numberOfColumns = int{ LogItemModel::GetNumberOfColumns() };

        for (int column{}; column < numberOfColumns; ++column)
        {
            treeView->resizeColumnToContents(column);
        }

        // Reset the view "on idle".
        m_awakeConnection = connect(
            QAbstractEventDispatcher::instance(),
            &QAbstractEventDispatcher::awake,
            [this, treeView]
        {
            _itemModel.Reload();
            treeView->reset();
        });
    }

    ~Data()
    {
        disconnect(m_awakeConnection);
    }
};


LogDockWidget::LogDockWidget(QMainWindow& mainWindow)
    :
    QDockWidget(&mainWindow),
    _data(std::make_unique<Data>(*this))
{
}


LogDockWidget::~LogDockWidget() = default;

} // namespace gui

} // namespace hdps
