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
    _overlayWidget(&_treeView),
    _noItemsDescription(""),
    _filterNameAction(this, "Name filter"),
    _filterGroupAction(this),
    _filterCaseSensitiveAction(this, "Case-sensitive", false, false),
    _filterRegularExpressionAction(this, "Regular expression", false, false),
    _expandAllAction(this, "Expand all"),
    _collapseAllAction(this, "Collapse all"),
    _selectAllAction(this, "Select all"),
    _selectNoneAction(this, "Select none"),
    _selectionGroupAction(this),
    _settingsGroupAction(this)
{
    _filterNameAction.setSearchMode(true);
    _filterNameAction.setClearable(true);

    _filterGroupAction.setText(QString("%1 filtering").arg(_itemTypeName));
    _filterGroupAction.setIcon(Application::getIconFont("FontAwesome").getIcon("filter"));
    _filterGroupAction.setToolTip("Adjust filtering parameters");

    _filterCaseSensitiveAction.setToolTip("Enable/disable search filter case-sensitive");
    _filterCaseSensitiveAction.setConnectionPermissionsToNone();

    _filterRegularExpressionAction.setToolTip("Enable/disable search filter with regular expression");
    _filterRegularExpressionAction.setConnectionPermissionsToNone();

    _filterGroupAction << _filterCaseSensitiveAction;
    _filterGroupAction << _filterRegularExpressionAction;

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

    _settingsGroupAction.setText("Settings");
    _settingsGroupAction.setToolTip(QString("Edit %1s hierarchy settings").arg(_itemTypeName.toLower()));
    _settingsGroupAction.setIcon(Application::getIconFont("FontAwesome").getIcon("cog"));
    _settingsGroupAction.setVisible(false);

    auto layout = new QVBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    if (showToolbar) {
        auto toolbarLayout = new QHBoxLayout();

        toolbarLayout->setSpacing(3);

        toolbarLayout->addWidget(_filterNameAction.createWidget(this), 1);
        toolbarLayout->addWidget(_filterGroupAction.createCollapsedWidget(this));
        toolbarLayout->addWidget(_expandAllAction.createWidget(this));
        toolbarLayout->addWidget(_collapseAllAction.createWidget(this));
        toolbarLayout->addWidget(_selectionGroupAction.createCollapsedWidget(this));
        toolbarLayout->addWidget(_settingsGroupAction.createCollapsedWidget(this));

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

    _treeView.setAutoFillBackground(true);
    //_treeView.setAutoExpandDelay(300);
    _treeView.setContextMenuPolicy(Qt::CustomContextMenu);
    _treeView.setSelectionModel(&_selectionModel);
    _treeView.setDragEnabled(true);
    _treeView.setDragDropMode(QAbstractItemView::DragOnly);
    _treeView.setSelectionBehavior(QAbstractItemView::SelectRows);
    _treeView.setSelectionMode(QAbstractItemView::ExtendedSelection);
    _treeView.setRootIsDecorated(true);
    _treeView.setItemsExpandable(true);
    _treeView.setIconSize(QSize(14, 14));
    _treeView.setAnimated(true);
    
    auto header = _treeView.header();

    header->setMinimumSectionSize(18);
    header->setIconSize(QSize(4, 10));

    const auto numberOfRowsChanged = [this]() -> void {
        const auto hasItems = _filterModel != nullptr ? _filterModel->rowCount() >= 1 : _model.rowCount() >= 1;

        _filterNameAction.setEnabled(_model.rowCount() >= 1);
        _filterGroupAction.setEnabled(_model.rowCount() >= 1);
        _selectionGroupAction.setEnabled(hasItems);

        //_treeView.setEnabled(hasItems);
        _treeView.setHeaderHidden(!hasItems);
        
        updateExpandCollapseActionsReadOnly();
        updateOverlayWidget();
    };

    const auto updateFilterModel = [this]() -> void {
        if (_filterModel == nullptr)
            return;

        const auto caseSensitivity = _filterCaseSensitiveAction.isChecked() ? "case-sensitive" : "case-insensitive";

        if (_filterRegularExpressionAction.isChecked()) {
            _filterNameAction.setPlaceHolderString(QString("Search for %1 by regular expression (%2)").arg(_itemTypeName.toLower(), caseSensitivity));

            auto regularExpression = QRegularExpression(_filterNameAction.getString());

            if (!_filterCaseSensitiveAction.isChecked())
                regularExpression.setPatternOptions(QRegularExpression::CaseInsensitiveOption);

            if (regularExpression.isValid())
                _filterModel->setFilterRegularExpression(regularExpression);
        }
        else {
            _filterNameAction.setPlaceHolderString(QString("Search for %1 by name (%2)").arg(_itemTypeName.toLower(), caseSensitivity));
            _filterModel->setFilterFixedString(_filterNameAction.getString());
        }
            
        _filterModel->setFilterCaseSensitivity(_filterCaseSensitiveAction.isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive);
        _filterModel->invalidate();

        updateOverlayWidget();
    };

    connect(&_filterNameAction, &StringAction::stringChanged, this, updateFilterModel);
    connect(&_filterCaseSensitiveAction, &ToggleAction::toggled, this, updateFilterModel);
    connect(&_filterRegularExpressionAction, &ToggleAction::toggled, this, updateFilterModel);

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
        connect(_filterModel, &QAbstractItemModel::rowsInserted, this, numberOfRowsChanged);
        connect(_filterModel, &QAbstractItemModel::rowsRemoved, this, numberOfRowsChanged);
    }
    else {
        connect(&_model, &QAbstractItemModel::rowsInserted, this, numberOfRowsChanged);
        connect(&_model, &QAbstractItemModel::rowsRemoved, this, numberOfRowsChanged);
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
    for (const auto& filterModelIndex : fetchFilterModelIndices())
        _selectionModel.select(filterModelIndex, QItemSelectionModel::Deselect | QItemSelectionModel::Rows);
}

void HierarchyWidget::updateOverlayWidget()
{
    if (_filterModel == nullptr) {
        if (_model.rowCount() == 0) {
            _overlayWidget.set(windowIcon(), QString("No %1s to display").arg(_itemTypeName.toLower()), _noItemsDescription);
            _overlayWidget.show();
        }
        else {
            _overlayWidget.hide();
        }
    }
    else {
        if (_model.rowCount() >= 1) {
            if (_filterModel->rowCount() == 0) {
                _overlayWidget.set(windowIcon(), QString("No %1s found").arg(_itemTypeName.toLower()), "Try changing the filter parameters...");
                _overlayWidget.show();
            }
            else {
                _overlayWidget.hide();
            }
        }
        else {
            _overlayWidget.set(windowIcon(), QString("No %1s to display").arg(_itemTypeName.toLower()), _noItemsDescription);
            _overlayWidget.show();
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
