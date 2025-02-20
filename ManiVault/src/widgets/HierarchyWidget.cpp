// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "HierarchyWidget.h"
#include "Application.h"

#include <QDebug>
#include <QVBoxLayout>
#include <QSortFilterProxyModel>
#include <QHeaderView>

#include <stdexcept>

#ifdef _DEBUG
    #define HIERARCHY_WIDGET_VERBOSE
#endif

using namespace mv::util;

namespace mv::gui
{

HierarchyWidget::HierarchyWidget(QWidget* parent, const QString& itemTypeName, const QAbstractItemModel& model, QSortFilterProxyModel* filterModel /*= nullptr*/, bool showToolbar /*= true*/, bool showOverlay /*= true*/) :
    QWidget(parent),
    _itemTypeName(itemTypeName),
    _headerHidden(false),
    _model(model),
    _filterModel(filterModel),
    _selectionModel(_filterModel != nullptr ? _filterModel : const_cast<QAbstractItemModel*>(&_model)),
    _treeView(this),
    _infoOverlayWidget(showOverlay ? new InfoOverlayWidget(&_treeView) : nullptr),
    _noItemsDescription(""),
    _filterNameAction(this, "Name"),
    _filterColumnAction(this, "Column"),
    _filterGroupAction(this, "Filter"),
    _filterCaseSensitiveAction(this, "Case-sensitive", false),
    _filterRegularExpressionAction(this, "Regular expression", false),
    _expandAllAction(this, "Expand all"),
    _collapseAllAction(this, "Collapse all"),
    _selectAllAction(this, "Select all"),
    _selectNoneAction(this, "Select none"),
    _selectionGroupAction(this, "Selection"),
    _columnsGroupAction(this, "Columns"),
    _settingsGroupAction(this, "Settings"),
    _toolbarAction(this, "Toolbar")
{
    setObjectName("HierarchyWidget");

    if (_filterModel) {
        if (_filterModel->sourceModel() != &_model)
            _filterModel->setSourceModel(const_cast<QAbstractItemModel*>(&_model));

        _treeView.setModel(_filterModel);
    }
    else {
        _treeView.setModel(const_cast<QAbstractItemModel*>(&_model));
    }

    if (_infoOverlayWidget) {
        auto& widgetFader = _infoOverlayWidget->getWidgetFader();

        widgetFader.setOpacity(0.0f);
        widgetFader.setMaximumOpacity(0.5f);
        widgetFader.setFadeInDuration(100);
        widgetFader.setFadeOutDuration(300);
    }
        
    _filterNameAction.setSearchMode(true);
    _filterNameAction.setClearable(true);
    _filterNameAction.setConnectionPermissionsToForceNone();
    _filterNameAction.setStretch(2);

    _filterGroupAction.setText("Filtering");
    _filterGroupAction.setIconByName("filter");
    _filterGroupAction.setToolTip("Adjust filtering parameters");
    _filterGroupAction.setConnectionPermissionsToForceNone();

    _filterCaseSensitiveAction.setToolTip("Enable/disable search filter case-sensitive");
    _filterCaseSensitiveAction.setConnectionPermissionsToForceNone();

    _filterRegularExpressionAction.setToolTip("Enable/disable search filter with regular expression");
    _filterRegularExpressionAction.setConnectionPermissionsToForceNone();

    _filterGroupAction.addAction(&_filterColumnAction);
    _filterGroupAction.addAction(&_filterCaseSensitiveAction);
    _filterGroupAction.addAction(&_filterRegularExpressionAction);

    _expandAllAction.setIconByName("angle-double-down");
    _expandAllAction.setToolTip(QString("Expand all %1s in the hierarchy").arg(_itemTypeName.toLower()));
    _expandAllAction.setDefaultWidgetFlags(TriggerAction::Icon);
    _expandAllAction.setConnectionPermissionsToForceNone();

    _collapseAllAction.setIconByName("angle-double-up");
    _collapseAllAction.setToolTip(QString("Collapse all %1s in the hierarchy").arg(_itemTypeName.toLower()));
    _collapseAllAction.setDefaultWidgetFlags(TriggerAction::Icon);
    _collapseAllAction.setConnectionPermissionsToForceNone();

    _selectAllAction.setToolTip(QString("Select all %1s").arg(_itemTypeName.toLower()));
    _selectAllAction.setConnectionPermissionsToForceNone();

    _selectNoneAction.setToolTip(QString("De-select all %1s").arg(_itemTypeName.toLower()));
    _selectNoneAction.setConnectionPermissionsToForceNone();

    _selectionGroupAction.setText("Selection");
    _selectionGroupAction.setIconByName("mouse-pointer");
    _selectionGroupAction.setConnectionPermissionsToForceNone();

    _selectionGroupAction.addAction(&_selectAllAction);
    _selectionGroupAction.addAction(&_selectNoneAction);

    _columnsGroupAction.setText("Columns");
    _columnsGroupAction.setToolTip(QString("Edit which %1 columns should be visible").arg(_itemTypeName.toLower()));
    _columnsGroupAction.setIconByName("columns");
    _columnsGroupAction.setShowLabels(false);
    _columnsGroupAction.setConnectionPermissionsToForceNone();

    auto selectAllCollumns = new TriggerAction(this, "Select all");

    const auto updateSelectAllCollumnsReadOnly = [this, selectAllCollumns]() -> void {
        auto readOnly = true;

        for (std::int32_t columnIndex = 0; columnIndex < _model.columnCount(); columnIndex++) {
            if (_treeView.isColumnHidden(columnIndex)) {
                readOnly = false;
                break;
            }
        }

        selectAllCollumns->setEnabled(!readOnly);
    };

    connect(selectAllCollumns, &TriggerAction::triggered, this, [this]() -> void {
        for (std::int32_t columnIndex = 0; columnIndex < _model.columnCount(); columnIndex++)
            _columnsGroupAction.getActions()[columnIndex]->setChecked(true);
    });

    QStringList columnNames;

    for (std::int32_t columnIndex = 0; columnIndex < _model.columnCount(); columnIndex++) {
        const auto columnVisible    = !_treeView.isColumnHidden(columnIndex);
        const auto columnHeader     = _model.headerData(columnIndex, Qt::Horizontal, Qt::EditRole).toString();

        columnNames << columnHeader;

        auto columnVisibilityAction = new ToggleAction(this, columnHeader.isEmpty() ? QString("Column %1").arg(QString::number(columnIndex)) : columnHeader, columnVisible);

        columnVisibilityAction->setConnectionPermissionsToForceNone();

        connect(columnVisibilityAction, &ToggleAction::toggled, this, [this, columnIndex, updateSelectAllCollumnsReadOnly](bool toggled) -> void {
            _treeView.setColumnHidden(columnIndex, !toggled);
            updateSelectAllCollumnsReadOnly();
        });

        _columnsGroupAction.addAction(columnVisibilityAction);
    }

    _filterColumnAction.setOptions(columnNames);
    _filterColumnAction.setCurrentIndex(_filterModel ? _filterModel->filterKeyColumn() : 0);

    _columnsGroupAction.addAction(selectAllCollumns);

    connect(&_treeView, &HierarchyWidgetTreeView::columnHiddenChanged, this, [this](int column, bool hide) -> void {
        auto columnAction = _columnsGroupAction.getActions()[column];

        if (columnAction->isChecked() == hide)
            columnAction->setChecked(!hide);
    });

    updateSelectAllCollumnsReadOnly();

    _settingsGroupAction.setText("Settings");
    _settingsGroupAction.setToolTip(QString("Edit %1s hierarchy settings").arg(_itemTypeName.toLower()));
    _settingsGroupAction.setIconByName("gear");
    _settingsGroupAction.setVisible(false);

    auto layout = new QVBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    _filterGroupAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);
    _selectionGroupAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);
    _columnsGroupAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);
    _settingsGroupAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);

    _toolbarAction.setShowLabels(false);

    if (showToolbar) {
        if (_filterModel) {
            _toolbarAction.addAction(&_filterNameAction);
            _toolbarAction.addAction(&_filterGroupAction);
        }
        
        _toolbarAction.addAction(&_expandAllAction);
        _toolbarAction.addAction(&_collapseAllAction);
        //_toolbarAction.addAction(&_selectionGroupAction);
        _toolbarAction.addAction(&_columnsGroupAction);
        _toolbarAction.addAction(&_settingsGroupAction);

        layout->addWidget(_toolbarAction.createWidget(this));
    }
    
    layout->addWidget(&_treeView);

    setLayout(layout);

    _treeView.setAutoFillBackground(true);
    _treeView.setContextMenuPolicy(Qt::CustomContextMenu);
    _treeView.setSelectionModel(&_selectionModel);
    _treeView.setDragEnabled(true);
    _treeView.setDragDropMode(QAbstractItemView::DragOnly);
    _treeView.setSelectionBehavior(QAbstractItemView::SelectRows);
    _treeView.setSelectionMode(QAbstractItemView::ExtendedSelection);
    _treeView.setRootIsDecorated(true);
    _treeView.setItemsExpandable(true);
    _treeView.setIconSize(QSize(14, 14));
    _treeView.setObjectName("TreeView");
    
    auto header = _treeView.header();

    header->setMinimumSectionSize(18);
    header->setIconSize(QSize(4, 10));

    connect(header, &QHeaderView::sectionResized, this, [this](int logicalIndex, int oldSize, int newSize) -> void {
        for (int columnIndex = 0; columnIndex < _model.columnCount(); ++columnIndex)
            _columnsGroupAction.getActions()[columnIndex]->setChecked(!_treeView.header()->isSectionHidden(columnIndex));
    });

    const auto filterModelRowsChanged = [this]() -> void {
        const auto hasItems = _filterModel != nullptr ? _filterModel->rowCount() >= 1 : _model.rowCount() >= 1;

        for (auto action : _toolbarAction.getActions())
            const_cast<WidgetAction*>(action)->setEnabled(hasItems);

        _filterNameAction.setEnabled(_model.rowCount() >= 1);
        _filterColumnAction.setEnabled(_model.rowCount() >= 1);
        _filterGroupAction.setEnabled(_model.rowCount() >= 1);
        _selectionGroupAction.setEnabled(hasItems);
        _columnsGroupAction.setEnabled(hasItems);
        _settingsGroupAction.setEnabled(hasItems);
        
        updateExpandCollapseActionsReadOnly();
        updateOverlayWidget();
    };

    connect(&_filterNameAction, &StringAction::stringChanged, this, &HierarchyWidget::updateFilterModel);
    connect(&_filterColumnAction, &OptionAction::currentIndexChanged, this, &HierarchyWidget::updateFilterModel);
    connect(&_filterCaseSensitiveAction, &ToggleAction::toggled, this, &HierarchyWidget::updateFilterModel);
    connect(&_filterRegularExpressionAction, &ToggleAction::toggled, this, &HierarchyWidget::updateFilterModel);
    connect(&_model, &QAbstractItemModel::layoutChanged, this, &HierarchyWidget::updateFilterModel);

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

    if (_filterModel) {
        //connect(_filterModel, &QSortFilterProxyModel::layoutChanged, this, filterModelRowsChanged);
        //connect(_filterModel, &QSortFilterProxyModel::layoutChanged, this, &HierarchyWidget::updateHeaderVisibility);

        connect(_filterModel, &QSortFilterProxyModel::rowsInserted, this, filterModelRowsChanged);
        connect(_filterModel, &QSortFilterProxyModel::rowsRemoved, this, filterModelRowsChanged);

        connect(_filterModel, &QSortFilterProxyModel::rowsInserted, this, &HierarchyWidget::updateHeaderVisibility);
        connect(_filterModel, &QSortFilterProxyModel::rowsRemoved, this, &HierarchyWidget::updateHeaderVisibility);
    }
    else {
        connect(&_model, &QAbstractItemModel::rowsInserted, this, &HierarchyWidget::updateFilterModel);
        connect(&_model, &QAbstractItemModel::rowsRemoved, this, &HierarchyWidget::updateFilterModel);
        connect(&_model, &QAbstractItemModel::rowsInserted, this, &HierarchyWidget::updateHeaderVisibility);
        connect(&_model, &QAbstractItemModel::rowsRemoved, this, &HierarchyWidget::updateHeaderVisibility);
    }

    connect(&_model, &QAbstractItemModel::rowsInserted, this, &HierarchyWidget::updateOverlayWidget);
    connect(&_model, &QAbstractItemModel::rowsRemoved, this, &HierarchyWidget::updateOverlayWidget);
    connect(&_model, &QAbstractItemModel::layoutChanged, this, &HierarchyWidget::updateOverlayWidget);

    connect(&_treeView, &QTreeView::expanded, this, &HierarchyWidget::updateExpandCollapseActionsReadOnly);
    connect(&_treeView, &QTreeView::collapsed, this, &HierarchyWidget::updateExpandCollapseActionsReadOnly);

    const auto selectionChanged = [this]() -> void {
        _selectAllAction.setEnabled(maySelectAll());
        _selectNoneAction.setEnabled(maySelectNone());
    };

    connect(&_selectionModel, &QItemSelectionModel::selectionChanged, this, selectionChanged);

    connect(&_selectAllAction, &TriggerAction::triggered, this, &HierarchyWidget::selectAll);
    connect(&_selectNoneAction, &TriggerAction::triggered, this, &HierarchyWidget::selectNone);

    filterModelRowsChanged();
    selectionChanged();
    updateFilterModel();
    updateOverlayWidget();
    updateHeaderVisibility();
}

QString HierarchyWidget::getItemTypeName() const
{
    return _itemTypeName;
}

void HierarchyWidget::setItemTypeName(const QString& itemTypeName)
{
    if (itemTypeName == _itemTypeName)
        return;

    _itemTypeName = itemTypeName;

    updateFilterModel();
}

void HierarchyWidget::setWindowIcon(const QIcon& icon)
{
    QWidget::setWindowIcon(icon);

    updateOverlayWidget();
}

void HierarchyWidget::setNoItemsDescription(const QString& noItemsDescription)
{
    if (noItemsDescription == _noItemsDescription)
        return;

    _noItemsDescription = noItemsDescription;

    updateOverlayWidget();
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
#ifdef HIERARCHY_WIDGET_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    _treeView.expandAll();
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
#ifdef HIERARCHY_WIDGET_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    _treeView.collapseAll();
}

bool HierarchyWidget::maySelectAll() const
{
    if (_filterModel == nullptr)
        return _selectionModel.selectedRows().count() < _model.rowCount();

    return _selectionModel.selectedRows().count() < _filterModel->rowCount();
}

void HierarchyWidget::selectAll()
{
#ifdef HIERARCHY_WIDGET_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    _treeView.setFocus(Qt::NoFocusReason);

    for (const auto& filterModelIndex : fetchFilterModelIndices())
        _selectionModel.select(filterModelIndex, QItemSelectionModel::Select | QItemSelectionModel::Rows);
}

bool HierarchyWidget::maySelectNone() const
{
    if (_filterModel != nullptr && _filterModel->rowCount() == 0)
        return false;

    if (_model.rowCount() == 0)
        return false;

    return _selectionModel.selectedRows().count() >= 1;
}

void HierarchyWidget::selectNone()
{
#ifdef HIERARCHY_WIDGET_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    for (const auto& filterModelIndex : fetchFilterModelIndices())
        _selectionModel.select(filterModelIndex, QItemSelectionModel::Deselect | QItemSelectionModel::Rows);
}

void HierarchyWidget::updateOverlayWidget()
{
    if (_infoOverlayWidget.isNull())
        return;

    auto& widgetFader = _infoOverlayWidget->getWidgetFader();

    if (_filterModel == nullptr) {
        if (_model.rowCount() == 0) {
            _infoOverlayWidget->set(windowIcon(), QString("No %1s to display").arg(_itemTypeName.toLower()), _noItemsDescription);
            widgetFader.fadeIn();
        }
        else {
            widgetFader.fadeOut();
        }
    }
    else {
        if (_model.rowCount() >= 1) {
            if (_filterModel->rowCount() == 0) {
                _infoOverlayWidget->set(windowIcon(), QString("No %1s found").arg(_itemTypeName.toLower()), (_toolbarAction.isVisible() ? "Try changing the filter parameters..." : ""));
                widgetFader.fadeIn();
            }
            else {
                _infoOverlayWidget->set(windowIcon(), QString("No %1s found").arg(_itemTypeName.toLower()), "");
                widgetFader.fadeOut();
            }
        }
        else {
            _infoOverlayWidget->set(windowIcon(), QString("No %1s to display").arg(_itemTypeName.toLower()), _noItemsDescription);
            widgetFader.fadeIn();
        }
    }
}

void HierarchyWidget::updateExpandCollapseActionsReadOnly()
{
    const auto hasItems = _model.rowCount() >= 1;

    _expandAllAction.setEnabled(hasItems && mayExpandAll());
    _collapseAllAction.setEnabled(hasItems && mayCollapseAll());

    QModelIndex index;

    const auto showExpandCollapse = mayExpandAll() || mayCollapseAll();

    _expandAllAction.setVisible(showExpandCollapse);
    _collapseAllAction.setVisible(showExpandCollapse);
}

bool HierarchyWidget::getHeaderHidden() const
{
    return _headerHidden;
}

void HierarchyWidget::setHeaderHidden(bool headerHidden)
{
    if (headerHidden == _headerHidden)
        return;

    _headerHidden = headerHidden;

    updateHeaderVisibility();
}

void HierarchyWidget::updateFilterModel()
{
    if (_filterModel == nullptr)
        return;

    const auto itemTypeNameLowered = _itemTypeName.toLower();

    _filterModel->setFilterKeyColumn(_filterColumnAction.getCurrentIndex());

    auto shouldInvalidateFilterModel = false;

    //_filterModel->setRecursiveFilteringEnabled(!_filterNameAction.getString().isEmpty());

    if (_filterRegularExpressionAction.isChecked()) {
        _filterNameAction.setPlaceHolderString(QString("Search for %1 by regular expression").arg(itemTypeNameLowered));

        auto regularExpression = QRegularExpression(_filterNameAction.getString());

        if (!_filterCaseSensitiveAction.isChecked())
            regularExpression.setPatternOptions(QRegularExpression::CaseInsensitiveOption);

        if (regularExpression.isValid() && (regularExpression != _filterModel->filterRegularExpression())) {
            _filterModel->setFilterRegularExpression(regularExpression);

            shouldInvalidateFilterModel = true;
        }
    }
    else {
        const auto filterColumn = _model.headerData(_filterModel->filterKeyColumn(), Qt::Horizontal).toString().toLower();

        _filterNameAction.setPlaceHolderString(QString("Search for %1 by %2").arg(itemTypeNameLowered, filterColumn));

        if (QRegularExpression(_filterNameAction.getString()) != _filterModel->filterRegularExpression()) {
            _filterModel->setFilterFixedString(_filterNameAction.getString());

            shouldInvalidateFilterModel = true;
        }
    }

    _filterModel->setFilterCaseSensitivity(_filterCaseSensitiveAction.isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive);

    if (shouldInvalidateFilterModel)
        _filterModel->invalidate();

    updateOverlayWidget();
}

void HierarchyWidget::updateHeaderVisibility()
{
    const auto hasItems = _filterModel != nullptr ? _filterModel->rowCount() >= 1 : _model.rowCount() >= 1;

    _treeView.setHeaderHidden(_headerHidden || !hasItems);
}

}
