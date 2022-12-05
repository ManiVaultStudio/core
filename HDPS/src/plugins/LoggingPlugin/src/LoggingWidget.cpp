#include "LoggingWidget.h"

#include <Application.h>

#include <QDebug>
#include <QHeaderView>
#include <QMenu>
#include <QGuiApplication>
#include <QClipboard>
#include <QAbstractEventDispatcher>

using namespace hdps;
using namespace hdps::util;

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

    _hierarchyWidget.getCollapseAllAction().setVisible(false);
    _hierarchyWidget.getExpandAllAction().setVisible(false);
    
    auto& settingsGroupAction = _hierarchyWidget.getSettingsGroupAction();

    settingsGroupAction.setVisible(true);

    settingsGroupAction << _model.getWordWrapAction();

    auto& filterGroupAction = _hierarchyWidget.getFilterGroupAction();

    filterGroupAction.setLabelWidthFixed(60);
    filterGroupAction.setPopupSizeHint(QSize(340, 10));

    filterGroupAction << _filterModel.getFilterTypeAction();

    auto& treeView = _hierarchyWidget.getTreeView();

    _idleUpdateConnection = connect(QAbstractEventDispatcher::instance(), &QAbstractEventDispatcher::awake, &_model, &LoggingModel::synchronizeLogRecords);

    treeView.setSortingEnabled(true);
    treeView.setRootIsDecorated(false);
    treeView.setColumnHidden(static_cast<int>(LoggingModel::Column::Category), true);
    treeView.setColumnHidden(static_cast<int>(LoggingModel::Column::FileAndLine), true);
    treeView.setColumnHidden(static_cast<int>(LoggingModel::Column::Function), true);

    auto treeViewHeader = treeView.header();

    treeViewHeader->resizeSection(static_cast<int>(LoggingModel::Column::Number), 50);
    treeViewHeader->resizeSection(static_cast<int>(LoggingModel::Column::Type), 60);

    treeViewHeader->setSectionResizeMode(static_cast<int>(LoggingModel::Column::Number), QHeaderView::Fixed);
    treeViewHeader->setSectionResizeMode(static_cast<int>(LoggingModel::Column::Type), QHeaderView::Fixed);

    connect(&treeView, &QTreeView::customContextMenuRequested, [this, &treeView](const QPoint& point)
    {
        const auto selectedRows = treeView.selectionModel()->selectedRows();

        if (selectedRows.isEmpty())
            return;

        QMenu contextMenu;

        auto* const copyAction = contextMenu.addAction(tr("&Copy"), [this, selectedRows] {
            QStringList messageRecordsString;

            for (const auto& selectedRow : selectedRows) {
                const auto index            = _filterModel.mapToSource(selectedRows.first());
                const auto messageRecord    = static_cast<MessageRecord*>(index.internalPointer());

                messageRecordsString << messageRecord->toString();
            }
            
            QGuiApplication::clipboard()->setText(messageRecordsString.join("\n"));
        });
        
        contextMenu.exec(QCursor::pos());
    });
}

QSize LoggingWidget::sizeHint() const
{
    return QSize(100, 100);
}
