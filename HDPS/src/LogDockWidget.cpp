// Its own header file:
#include "LogDockWidget.h"

#include "LogItemModel.h"

// Qt header files:
#include <QAbstractEventDispatcher>
#include <QClipboard>
#include <QGuiApplication>
#include <QHeaderView>
#include <QMenu>
#include <QMainWindow>
#include <QTreeView>
#include <QUuid>
#include <QVBoxLayout>

namespace
{
    int GetSelectedRow(const QAbstractItemView& view)
    {
        const auto* const selectionModel = view.selectionModel();

        if (selectionModel != nullptr)
        {
            const auto selection = selectionModel->selection();

            if (selection.size() == 1)
            {
                return selection.front().top();
            }
        }
        return -1;
    }
}

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
    explicit Data(DockableWidget& dockableWidget)
    {
        auto& treeView = *new QTreeView(&dockableWidget);

        auto layout = new QVBoxLayout();
        layout->setMargin(0);
        layout->setSpacing(0);
        layout->addWidget(&treeView);
        dockableWidget.setLayout(layout);

        treeView.setEditTriggers(QAbstractItemView::NoEditTriggers);
        treeView.setRootIsDecorated(false);
        treeView.setItemsExpandable(false);
        treeView.setSortingEnabled(true);
        treeView.setExpandsOnDoubleClick(false);
        treeView.header()->setSortIndicator(-1, Qt::AscendingOrder);
        treeView.setModel(&_itemModel);

        const auto numberOfColumns = int{ LogItemModel::numberOfColumns };

        for (int column{}; column < numberOfColumns; ++column)
        {
            treeView.resizeColumnToContents(column);
        }

        // Reset the view "on idle".
        m_awakeConnection = connect(
            QAbstractEventDispatcher::instance(),
            &QAbstractEventDispatcher::awake,
            [this, &treeView]
        {
            if (_itemModel.Reload())
            {
                treeView.reset();
            }
        });

        treeView.setContextMenuPolicy(Qt::CustomContextMenu);

        (void)connect(&treeView, &QTreeView::customContextMenuRequested,
            [this, &treeView](const QPoint &)
        {
            const auto row = GetSelectedRow(treeView);
            const auto* const messageRecord = (row < 0) ? nullptr : _itemModel.GetMessageRecordAtRow(row);

            QMenu contextMenu;

            auto* const copyAction = contextMenu.addAction(tr("&Copy"), [this, messageRecord]
            {
                if (messageRecord != nullptr)
                {
                    QString text;

                    for (unsigned column{}; column < LogItemModel::numberOfColumns; ++column)
                    {
                        text.append(_itemModel.GetDataValueAtColumn(*messageRecord, column).toString())
                            .append(QLatin1Char('\t'));
                    }
                    text.back() = QLatin1Char('\n');

                    QGuiApplication::clipboard()->setText(text);
                }
            });
            copyAction->setEnabled(messageRecord != nullptr);
            contextMenu.exec(QCursor::pos());
        });
    }

    ~Data()
    {
        disconnect(m_awakeConnection);
    }
};


LogDockWidget::LogDockWidget(const DockingLocation& location, QWidget* parent)
    :
    DockableWidget(location, parent),
    _data(std::make_unique<Data>(*this))
{
    // Generate a unique name for the widget to let Qt identify it and store/retrieve its state
    setObjectName(QString("Dockable Widget ") + QUuid::createUuid().toString());
}


LogDockWidget::~LogDockWidget() = default;

} // namespace gui

} // namespace hdps
