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
    _expandAllAction(this, "Expand all"),
    _collapseAllAction(this, "Collapse all")
{
    _nameFilterAction.setPlaceHolderString("Filter by name or regular expression...");
    _nameFilterAction.setSearchMode(true);
    _nameFilterAction.setClearable(true);

    _expandAllAction.setIcon(Application::getIconFont("FontAwesome").getIcon("angle-double-down"));
    _expandAllAction.setToolTip("Expand all datasets in the hierarchy");
    _expandAllAction.setDefaultWidgetFlags(TriggerAction::Icon);

    _collapseAllAction.setIcon(Application::getIconFont("FontAwesome").getIcon("angle-double-up"));
    _collapseAllAction.setToolTip("Collapse all datasets in the hierarchy");
    _collapseAllAction.setDefaultWidgetFlags(TriggerAction::Icon);

    auto layout = new QVBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    if (showToolbar) {
        auto toolbarLayout = new QHBoxLayout();

        toolbarLayout->addWidget(_nameFilterAction.createWidget(this), 1);
        toolbarLayout->addWidget(_expandAllAction.createWidget(this));
        toolbarLayout->addWidget(_collapseAllAction.createWidget(this));

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
    _treeView.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    
    auto header = _treeView.header();

    header->setMinimumSectionSize(18);
    header->setIconSize(QSize(4, 10));

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

    connect(&_expandAllAction, &TriggerAction::triggered, this, [this]() -> void {
        //if (!filterModelIndex.isValid())
        //    return;

        //_model.getItem(toSourceModelIndex(filterModelIndex), Qt::DisplayRole)->getDataHierarchyItem()->setExpanded(true);
    });

    connect(&_collapseAllAction, &TriggerAction::triggered, this, [this]() -> void {
        //if (!filterModelIndex.isValid())
        //    return;

        //_model.getItem(_filterModel.mapToSource(filterModelIndex), Qt::DisplayRole)->getDataHierarchyItem()->setExpanded(false);
    });

    const auto numberOfRowsChanged = [this]() -> void {
        const auto noItems = _model.rowCount() <= 0;

        _nameFilterAction.setEnabled(!noItems && mayExpandAll());
        _expandAllAction.setEnabled(!noItems && mayExpandAll());
        _collapseAllAction.setEnabled(!noItems && mayCollapseAll());

        _treeView.setHeaderHidden(!noItems);

        updateOverlayWidget();
    };

    connect(&_model, &QAbstractItemModel::rowsInserted, this, numberOfRowsChanged);
    connect(&_model, &QAbstractItemModel::rowsRemoved, this, numberOfRowsChanged);

    numberOfRowsChanged();
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

        if (_filterModel->hasChildren(index))
            modelIndexList << fetchFilterModelIndices(index);
    }

    return modelIndexList;
}

bool HierarchyWidget::mayExpandAll() const
{
    const auto allFilterModelIndices = fetchFilterModelIndices();

    for (const auto& filterModelIndex : allFilterModelIndices)
        if (_model.rowCount(filterModelIndex) > 0 && !_treeView.isExpanded(filterModelIndex))
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
    const auto allFilterModelIndices = fetchFilterModelIndices();

    for (const auto& filterModelIndex : allFilterModelIndices)
        if (_model.rowCount(filterModelIndex) > 0 && _treeView.isExpanded(filterModelIndex))
            return true;

    return false;
}

void HierarchyWidget::collapseAll()
{
    const auto allFilterModelIndices = fetchFilterModelIndices();

    for (const auto& filterModelIndex : allFilterModelIndices)
        _treeView.setExpanded(filterModelIndex, false);
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
            _overlayWidget.set(windowIcon(), QString("No %1s found").arg(_itemTypeName.toLower()), "Try changing the search parameters...");
            _overlayWidget.show();
        }
        else {
            _overlayWidget.hide();
        }
    }
}

}
}
