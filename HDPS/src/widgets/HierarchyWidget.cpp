#include "HierarchyWidget.h"
#include "Application.h"

#include <QDebug>
#include <QVBoxLayout>
#include <QSortFilterProxyModel>
#include <QHeaderView>

#include <stdexcept>

using namespace hdps::util;

namespace hdps
{

namespace gui
{

HierarchyWidget::HierarchyWidget(QWidget* parent, const QString& itemTypeName, QAbstractItemModel& model, QSortFilterProxyModel* filterModel /*= nullptr*/, bool showToolbar /*= true*/, bool showOverlay /*= true*/) :
    QWidget(parent),
    _itemTypeName(itemTypeName),
    _model(model),
    _filterModel(filterModel),
    _selectionModel(_filterModel != nullptr ? _filterModel : &_model),
    _treeView(this),
    _overlayWidget(this),
    _nameFilterAction(this, "Name filter"),
    _filterGroupAction(this),
    _expandAllAction(this, "Expand all"),
    _collapseAllAction(this, "Collapse all"),
    _selectAllAction(this, "Select all"),
    _selectNoneAction(this, "Select none"),
    _selectionGroupAction(this)
{
    _nameFilterAction.setPlaceHolderString(QString("Search for %1 by name or regular expression...").arg(_itemTypeName.toLower()));
    _nameFilterAction.setSearchMode(true);
    _nameFilterAction.setClearable(true);

    _filterGroupAction.setText(QString("%1 filtering").arg(_itemTypeName));
    _filterGroupAction.setIcon(Application::getIconFont("FontAwesome").getIcon("filter"));
    _filterGroupAction.setToolTip("Adjust filtering parameters");

    _expandAllAction.setIcon(Application::getIconFont("FontAwesome").getIcon("angle-double-down"));
    _expandAllAction.setToolTip(QString("Expand all %1s in the hierarchy").arg(_itemTypeName.toLower()));
    _expandAllAction.setDefaultWidgetFlags(TriggerAction::Icon);

    _collapseAllAction.setIcon(Application::getIconFont("FontAwesome").getIcon("angle-double-up"));
    _collapseAllAction.setToolTip(QString("Collapse all %1s in the hierarchy").arg(_itemTypeName.toLower()));
    _collapseAllAction.setDefaultWidgetFlags(TriggerAction::Icon);

    _selectAllAction.setToolTip(QString("Select all %1s in the hierarchy").arg(_itemTypeName.toLower()));
    _selectNoneAction.setToolTip(QString("De-select all %1s in the hierarchy").arg(_itemTypeName.toLower()));

    _selectionGroupAction.setText("Selection");
    _selectionGroupAction.setIcon(Application::getIconFont("FontAwesome").getIcon("mouse-pointer"));

    _selectionGroupAction << _selectAllAction;
    _selectionGroupAction << _selectNoneAction;

    auto layout = new QVBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    if (showToolbar) {
        auto toolbarLayout = new QHBoxLayout();

        toolbarLayout->setSpacing(3);

        toolbarLayout->addWidget(_nameFilterAction.createWidget(this), 1);
        toolbarLayout->addWidget(_filterGroupAction.createCollapsedWidget(this));
        toolbarLayout->addWidget(_expandAllAction.createWidget(this));
        toolbarLayout->addWidget(_collapseAllAction.createWidget(this));
        toolbarLayout->addWidget(_selectionGroupAction.createCollapsedWidget(this));

        layout->addLayout(toolbarLayout);
    }
    
    layout->addWidget(&_treeView);

    setLayout(layout);

    if (_filterModel) {
        _filterModel->setSourceModel(&_model);
        _treeView.setModel(_filterModel);
    }
    else {
        _treeView.setModel(&_model);
    }

    _treeView.setContextMenuPolicy(Qt::CustomContextMenu);
    _treeView.setSelectionModel(&_selectionModel);
    _treeView.setDragEnabled(true);
    _treeView.setDragDropMode(QAbstractItemView::DragOnly);
    _treeView.setSelectionBehavior(QAbstractItemView::SelectRows);
    _treeView.setSelectionMode(QAbstractItemView::ExtendedSelection);
    _treeView.setRootIsDecorated(true);
    _treeView.setItemsExpandable(true);
    _treeView.setIconSize(QSize(14, 14));
    
    auto header = _treeView.header();

    header->setMinimumSectionSize(18);
    header->setIconSize(QSize(4, 10));

    const auto numberOfRowsChanged = [this]() -> void {
        const auto hasItems = _model.rowCount() >= 1;

        _nameFilterAction.setEnabled(hasItems);
        _treeView.setHeaderHidden(!hasItems);

        updateExpandCollapseActionsReadOnly();
        updateOverlayWidget();
    };

    connect(&_nameFilterAction, &StringAction::stringChanged, this, [this](const QString& value) {
        if (_filterModel != nullptr) {
            const auto re = QRegularExpression(value);

            if (re.isValid())
                _filterModel->setFilterRegularExpression(re);
            else
                _filterModel->setFilterFixedString(value);
        }

        updateOverlayWidget();
    });

    const auto connectExpandCollapseActionsReadOnly = [this]() -> void {
        connect(&_treeView, &QTreeView::expanded, this, &HierarchyWidget::updateExpandCollapseActionsReadOnly);
        connect(&_treeView, &QTreeView::collapsed, this, &HierarchyWidget::updateExpandCollapseActionsReadOnly);
    };

    const auto disconnectExpandCollapseActionsReadOnly = [this]() -> void {
        disconnect(&_treeView, &QTreeView::expanded, this, &HierarchyWidget::updateExpandCollapseActionsReadOnly);
        disconnect(&_treeView, &QTreeView::collapsed, this, &HierarchyWidget::updateExpandCollapseActionsReadOnly);
    };

    connect(&_expandAllAction, &TriggerAction::triggered, this, [this, connectExpandCollapseActionsReadOnly, disconnectExpandCollapseActionsReadOnly]() -> void {
        disconnectExpandCollapseActionsReadOnly();
        {
            for (const auto& filterModelIndex : fetchFilterModelIndices())
                if (_filterModel->hasChildren(filterModelIndex))
                    _treeView.setExpanded(filterModelIndex, true);

            updateExpandCollapseActionsReadOnly();
        }
        connectExpandCollapseActionsReadOnly();
    });

    connect(&_collapseAllAction, &TriggerAction::triggered, this, [this, connectExpandCollapseActionsReadOnly, disconnectExpandCollapseActionsReadOnly]() -> void {
        disconnectExpandCollapseActionsReadOnly();
        {
            for (const auto& filterModelIndex : fetchFilterModelIndices())
                if (_filterModel->hasChildren(filterModelIndex))
                    _treeView.setExpanded(filterModelIndex, false);

            updateExpandCollapseActionsReadOnly();
        }
        connectExpandCollapseActionsReadOnly();
    });

    connect(&_model, &QAbstractItemModel::rowsInserted, this, numberOfRowsChanged);
    connect(&_model, &QAbstractItemModel::rowsRemoved, this, numberOfRowsChanged);

    if (_filterModel) {
        connect(_filterModel, &QAbstractItemModel::rowsInserted, this, numberOfRowsChanged);
        connect(_filterModel, &QAbstractItemModel::rowsRemoved, this, numberOfRowsChanged);
    }

    connect(&_treeView, &QTreeView::expanded, this, &HierarchyWidget::updateExpandCollapseActionsReadOnly);
    connect(&_treeView, &QTreeView::collapsed, this, &HierarchyWidget::updateExpandCollapseActionsReadOnly);

    const auto selectionChanged = [this]() -> void {
        _selectAllAction.setEnabled(maySelectAll());
        _selectNoneAction.setEnabled(maySelectNone());
    };

    connect(&_selectionModel, &QItemSelectionModel::selectionChanged, this, selectionChanged);

    connect(&_selectAllAction, &TriggerAction::triggered, this, &HierarchyWidget::selectAll);
    connect(&_selectNoneAction, &TriggerAction::triggered, this, &HierarchyWidget::selectNone);

    numberOfRowsChanged();
    selectionChanged();
}

QModelIndex HierarchyWidget::toSourceModelIndex(const QModelIndex& modelIndex) const
{
    if (_filterModel)
        return _filterModel->mapToSource(modelIndex);

    return modelIndex;
}

QModelIndexList HierarchyWidget::getSelectedRows() const
{
    QModelIndexList selectedRows;

    for (const auto& index : _selectionModel.selectedRows())
        selectedRows << toSourceModelIndex(index);

    return selectedRows;
}

QModelIndexList HierarchyWidget::fetchFilterModelIndices(QModelIndex filterModelIndex /*= QModelIndex()*/) const
{
    QModelIndexList modelIndexList;

    if (_filterModel == nullptr)
        return modelIndexList;

    if (filterModelIndex.isValid())
        modelIndexList << filterModelIndex;

    for (int rowIndex = 0; rowIndex < _filterModel->rowCount(filterModelIndex); ++rowIndex) {
        QModelIndex index = _filterModel->index(rowIndex, 0, filterModelIndex);

        modelIndexList << index;

        if (_filterModel->hasChildren(index))
            modelIndexList << fetchFilterModelIndices(index);
    }

    return modelIndexList;
}

bool HierarchyWidget::mayExpandAll() const
{
    if (_model.rowCount() == 0)
        return false;

    const auto allFilterModelIndices = fetchFilterModelIndices();

    for (const auto& filterModelIndex : allFilterModelIndices)
        if (_filterModel->hasChildren(filterModelIndex) && !_treeView.isExpanded(filterModelIndex))
            return true;

    return false;
}

void HierarchyWidget::expandAll()
{
    const auto allFilterModelIndices = fetchFilterModelIndices();

    for (const auto& filterModelIndex : allFilterModelIndices)
        _treeView.setExpanded(filterModelIndex, true);
}

bool HierarchyWidget::mayCollapseAll() const
{
    if (_model.rowCount() == 0)
        return false;

    const auto allFilterModelIndices = fetchFilterModelIndices();

    for (const auto& filterModelIndex : allFilterModelIndices)
        if (_filterModel->hasChildren(filterModelIndex) && _treeView.isExpanded(filterModelIndex))
            return true;

    return false;
}

void HierarchyWidget::collapseAll()
{
    const auto allFilterModelIndices = fetchFilterModelIndices();

    for (const auto& filterModelIndex : allFilterModelIndices)
        _treeView.setExpanded(filterModelIndex, false);
}

bool HierarchyWidget::maySelectAll() const
{
    return _selectionModel.selectedRows().count() < _filterModel->rowCount();
}

void HierarchyWidget::selectAll()
{
    for (const auto& filterModelIndex : fetchFilterModelIndices())
        _selectionModel.select(filterModelIndex, QItemSelectionModel::Select | QItemSelectionModel::Rows);
}

bool HierarchyWidget::maySelectNone() const
{
    if (_filterModel->rowCount() == 0)
        return false;

    return _selectionModel.selectedRows().count() >= 1;
}

void HierarchyWidget::selectNone()
{
    for (const auto& filterModelIndex : fetchFilterModelIndices())
        _selectionModel.select(filterModelIndex, QItemSelectionModel::Deselect | QItemSelectionModel::Rows);
}

void HierarchyWidget::updateOverlayWidget()
{
    if (_filterModel == nullptr) {
        if (_model.rowCount() == 0) {
            _overlayWidget.set(windowIcon(), QString("No %1s loaded").arg(_itemTypeName.toLower()), "");
            _overlayWidget.show();
        }
        else {
            _overlayWidget.hide();
        }
    }
    else {
        if (_model.rowCount() >= 1 && _filterModel->rowCount() == 0) {
            _overlayWidget.set(windowIcon(), QString("No %1s found for %2").arg(_itemTypeName.toLower(), _nameFilterAction.getString()), "Try changing the search parameters...");
            _overlayWidget.show();
        }
        else {
            _overlayWidget.hide();
        }
    }
}

void HierarchyWidget::updateExpandCollapseActionsReadOnly()
{
    const auto hasItems = _model.rowCount() >= 1;

    _expandAllAction.setEnabled(hasItems && mayExpandAll());
    _collapseAllAction.setEnabled(hasItems && mayCollapseAll());
}

}
}
