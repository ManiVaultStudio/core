// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DataHierarchyWidget.h"
#include "DataHierarchyWidgetContextMenu.h"

#include <Application.h>
#include <Set.h>
#include <PluginFactory.h>
#include <actions/PluginTriggerAction.h>

#include <QDebug>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QStyledItemDelegate>
#include <QStyleOptionViewItem>

#include <stdexcept>

using namespace mv;
using namespace mv::util;
using namespace mv::plugin;
using namespace mv::gui;

/**
 * Progress item delegate editor widget class
 * 
 * Specialized delegate editor widget which selectively toggles the 
 * progress action widget.
 * 
 * @author Thomas Kroes
 */
class ProgressItemDelegateEditorWidget : public QWidget
{
public:

    /**
     * Construct with pointer to \p progressItem and pointer to \p parent widget
     * @param progressItem Pointer to progressItem
     * @param parent Pointer to parent widget
     */
    ProgressItemDelegateEditorWidget(DataHierarchyModel::ProgressItem* progressItem, QWidget* parent) :
        QWidget(parent),
        _progressItem(progressItem),
        _progressEditorWidget(nullptr)
    {
        Q_ASSERT(progressItem != nullptr);

        if (progressItem == nullptr)
            return;

        _progressEditorWidget = progressItem->getTaskAction().getProgressAction().createWidget(this);

        auto layout = new QVBoxLayout();

        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(_progressEditorWidget);

        setLayout(layout);

        updateEditorWidgetVisibility();
        updateEditorWidgetReadOnly();

        connect(_progressItem->getTaskAction().getTask(), &Task::statusChanged, this, &ProgressItemDelegateEditorWidget::updateEditorWidgetVisibility);
        connect(&_progressItem->getDataset()->getDataHierarchyItem(), &DataHierarchyItem::lockedChanged, this, &ProgressItemDelegateEditorWidget::updateEditorWidgetReadOnly);
    }

private:

    /** Updates the editor widget visibility based on the dataset task status */
    void updateEditorWidgetVisibility() {
        const auto datasetTaskStatus = _progressItem->getDatasetTask().getStatus();

        if (datasetTaskStatus == Task::Status::Running || datasetTaskStatus == Task::Status::RunningIndeterminate || datasetTaskStatus == Task::Status::Finished)
            _progressEditorWidget->setVisible(true);
        else
            _progressEditorWidget->setVisible(false);
    }

    /** Updates the editor widget read-only state based on the dataset task status */
    void updateEditorWidgetReadOnly() {
        qDebug() << __FUNCTION__ << _progressItem->getDataset()->getGuiName() << _progressItem->getDataset()->isLocked();
        _progressEditorWidget->setEnabled(!_progressItem->getDataset()->isLocked());
    }

private:
    DataHierarchyModel::ProgressItem*   _progressItem;              /** Reference to the progress item */
    QWidget*                            _progressEditorWidget;      /** Pointer to created editor widget */
};

/**
 * Tree view item delegate class
 * 
 * Qt natively does not support disabled items to be selected, this class solves that
 * When an item (dataset) is locked, merely the visual representation is changed and not the item flags (only appears disabled)
 * 
 * @author Thomas Kroes
 */
class ItemDelegate : public QStyledItemDelegate {
public:

    /**
     * Construct with owning parent \p dataHierarchyWidget
     * @param parent Pointer to owning parent data hierarchy widget
     */
    explicit ItemDelegate(DataHierarchyWidget* dataHierarchyWidget) :
        QStyledItemDelegate(dataHierarchyWidget),
        _dataHierarchyWidget(dataHierarchyWidget)
    {
        Q_ASSERT(dataHierarchyWidget != nullptr);
    }

    /**
     * Creates custom editor with \p parent widget style \p option and model \p index
     * 
     * This method creates a custom editor widget for the progress column
     * 
     * @param parent Pointer to parent widget
     * @param option Style option
     * @param index Model index to create the editor for
     * @return Pointer to widget if progress column, nullptr otherwise
     */
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const {
        if (static_cast<DataHierarchyModel::Column>(index.column()) != DataHierarchyModel::Column::Progress)
            return QStyledItemDelegate::createEditor(parent, option, index);

        const auto sourceModelIndex = _dataHierarchyWidget->getFilterModel().mapToSource(index);
        const auto progressItem     = static_cast<DataHierarchyModel::ProgressItem*>(_dataHierarchyWidget->getModel().itemFromIndex(sourceModelIndex));
        
        return new ProgressItemDelegateEditorWidget(progressItem, parent);
    }

    /**
     * Update \p editor widget geometry when the cell geometry changes
     * @param option Style option
     * @param index Model index of the cell for which the geometry changed
     */
    void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const {
        Q_UNUSED(index)

        if (editor == nullptr)
            return;

        editor->setGeometry(option.rect);
    }

protected:

    /**
     * Init the style option(s) for the item delegate (we override the options to paint disabled when locked)
     * @param option Style option
     * @param index Index of the cell for which to initialize the style
     */
    void initStyleOption(QStyleOptionViewItem* option, const QModelIndex& index) const override
    {
        QStyledItemDelegate::initStyleOption(option, index);

        auto item = static_cast<DataHierarchyModel::Item*>(_dataHierarchyWidget->getModel().itemFromIndex(_dataHierarchyWidget->getFilterModel().mapToSource(index)));

        if (item->getDataset()->isLocked() || index.column() >= static_cast<int>(DataHierarchyModel::Column::IsGroup))
            option->state &= ~QStyle::State_Enabled;
    }

private:
    DataHierarchyWidget* _dataHierarchyWidget;  /** Pointer to owning data hierarchy widget */
};

DataHierarchyWidget::DataHierarchyWidget(QWidget* parent) :
    QWidget(parent),
    _model(this),
    _filterModel(this),
    _hierarchyWidget(this, "Dataset", _model, &_filterModel),
    _groupingAction(this, "Selection grouping", Application::core()->isDatasetGroupingEnabled()),
    _resetAction(this, "Reset")
{
    auto layout = new QVBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(&_hierarchyWidget, 1);

    setLayout(layout);

    _hierarchyWidget.setWindowIcon(Application::getIconFont("FontAwesome").getIcon("database"));
    _hierarchyWidget.setNoItemsDescription(QString("Right-click > Import to load data into %1").arg(Application::getName()));
    
    auto& settingsGroupAction = _hierarchyWidget.getSettingsGroupAction();

    settingsGroupAction.setVisible(true);
    settingsGroupAction.setShowLabels(false);

    settingsGroupAction.addAction(&_groupingAction);
    settingsGroupAction.addAction(&_resetAction);

    auto& treeView = _hierarchyWidget.getTreeView();

    treeView.setItemDelegate(new ItemDelegate(this));

    treeView.setColumnHidden(static_cast<int>(DataHierarchyModel::Column::DatasetId), true);
    treeView.setColumnHidden(static_cast<int>(DataHierarchyModel::Column::SourceDatasetId), true);
    treeView.setColumnHidden(static_cast<int>(DataHierarchyModel::Column::GroupIndex), true);

    auto treeViewHeader = treeView.header();

    treeViewHeader->setStretchLastSection(false);
    treeViewHeader->setMinimumSectionSize(18);

    treeViewHeader->resizeSection(DataHierarchyModel::Column::Name, 180);
    treeViewHeader->resizeSection(DataHierarchyModel::Column::GroupIndex, 60);
    treeViewHeader->resizeSection(DataHierarchyModel::Column::IsGroup, treeViewHeader->minimumSectionSize());
    treeViewHeader->resizeSection(DataHierarchyModel::Column::IsLocked, treeViewHeader->minimumSectionSize());
    treeViewHeader->resizeSection(DataHierarchyModel::Column::IsDerived, treeViewHeader->minimumSectionSize());

    treeViewHeader->setSectionResizeMode(DataHierarchyModel::Column::Name, QHeaderView::Interactive);
    treeViewHeader->setSectionResizeMode(DataHierarchyModel::Column::DatasetId, QHeaderView::Fixed);
    treeViewHeader->setSectionResizeMode(DataHierarchyModel::Column::GroupIndex, QHeaderView::Fixed);
    treeViewHeader->setSectionResizeMode(DataHierarchyModel::Column::Progress, QHeaderView::Stretch);
    treeViewHeader->setSectionResizeMode(DataHierarchyModel::Column::IsGroup, QHeaderView::Fixed);
    treeViewHeader->setSectionResizeMode(DataHierarchyModel::Column::IsLocked, QHeaderView::Fixed);

    _groupingAction.setIconByName("object-group");
    _groupingAction.setToolTip("Enable/disable dataset grouping");

    connect(&_groupingAction, &ToggleAction::toggled, this, &DataHierarchyWidget::onGroupingActionToggled);

    connect(&_resetAction, &TriggerAction::triggered, &Application::core()->getDataManager(), &AbstractDataManager::reset);

    connect(&_filterModel, &QSortFilterProxyModel::rowsInserted, this, [this, &treeView](const QModelIndex& parent, int first, int last) -> void {
        for (int rowIndex = first; rowIndex <= last; rowIndex++) {
            const auto progressFilterModelIndex         = _filterModel.index(rowIndex, static_cast<int>(DataHierarchyModel::Column::Progress), parent);
            const auto nameFilterModelIndex             = _filterModel.index(rowIndex, static_cast<int>(DataHierarchyModel::Column::Name), parent);
            const auto nameModelIndex                   = _filterModel.mapToSource(nameFilterModelIndex);
            const auto persistentNameFilterModelIndex   = QPersistentModelIndex(nameFilterModelIndex);

            _hierarchyWidget.getTreeView().openPersistentEditor(progressFilterModelIndex);

            updateItemExpansion(nameFilterModelIndex);

            connect(&_model.getItem<DataHierarchyModel::Item>(persistentNameFilterModelIndex)->getDataset()->getDataHierarchyItem(), &DataHierarchyItem::expandedChanged, this, [this, persistentNameFilterModelIndex]() -> void {
                updateItemExpansion(persistentNameFilterModelIndex);
            });
        }

        QCoreApplication::processEvents();
    });

    connect(&treeView, &QTreeView::expanded, this, [this](const QModelIndex& filterModelIndex) -> void {
        if (!filterModelIndex.isValid())
            return;

        auto modelIndex = _filterModel.mapToSource(filterModelIndex);

        if (!modelIndex.isValid())
            _model.getItem(modelIndex)->getDataset()->getDataHierarchyItem().setExpanded(true);
    });

    connect(&treeView, &QTreeView::collapsed, this, [this](const QModelIndex& filterModelIndex) -> void {
        if (!filterModelIndex.isValid())
            return;

        auto modelIndex = _filterModel.mapToSource(filterModelIndex);

        if (!modelIndex.isValid())
            _model.getItem(modelIndex)->getDataset()->getDataHierarchyItem().setExpanded(false);
    });

    connect(&_filterModel, &QSortFilterProxyModel::rowsAboutToBeRemoved, this, [this](const QModelIndex& parent, int first, int last) -> void {
        for (int rowIndex = first; rowIndex <= last; rowIndex++)
            _hierarchyWidget.getTreeView().closePersistentEditor(_filterModel.index(rowIndex, static_cast<int>(DataHierarchyModel::Column::Progress), parent));

        QCoreApplication::processEvents();
    });

    connect(&_hierarchyWidget.getSelectionModel(), &QItemSelectionModel::selectionChanged, this, [this](const QItemSelection& selected, const QItemSelection& deselected) {
        DataHierarchyItems dataHierarchyItems;

        for (const auto& index : _hierarchyWidget.getSelectedRows())
            dataHierarchyItems << &dataHierarchy().getItem(index.siblingAtColumn(static_cast<int>(DataHierarchyModel::Column::DatasetId)).data(Qt::EditRole).toString());

        dataHierarchy().selectItems(dataHierarchyItems);
    });

    connect(&_hierarchyWidget.getTreeView(), &QTreeView::customContextMenuRequested, this, [this](const QPoint& position) {

        auto createContextMenu = [this, position]() -> void {
            Datasets datasets;

            for (const auto& selectedRow : _hierarchyWidget.getSelectedRows())
                datasets << _model.getItem(selectedRow)->getDataset();

            QScopedPointer<DataHierarchyWidgetContextMenu> datasetsContextMenu(new DataHierarchyWidgetContextMenu(this, datasets));

            datasetsContextMenu->exec(_hierarchyWidget.getTreeView().viewport()->mapToGlobal(position));

        };

        // Get around possible signal-execution order complications
        QTimer::singleShot(10, createContextMenu);
    });

    updateColumnsVisibility();
    initializeExpansion();
}

QModelIndex DataHierarchyWidget::getModelIndexByDataset(const Dataset<DatasetImpl>& dataset)
{
    const auto modelIndices = _model.match(_model.index(0, 1), Qt::DisplayRole, dataset->getId(), 1, Qt::MatchFlag::MatchRecursive);

    if (modelIndices.isEmpty())
        throw new std::runtime_error(QString("'%1' not found in the data hierarchy model").arg(dataset->text()).toLatin1());

    return modelIndices.first();
}

void DataHierarchyWidget::onGroupingActionToggled(const bool& toggled)
{
    Application::core()->setDatasetGroupingEnabled(toggled);

    updateColumnsVisibility();
}

void DataHierarchyWidget::updateColumnsVisibility()
{
    //auto& treeView = _hierarchyWidget.getTreeView();

    //treeView.setColumnHidden(DataHierarchyModelItem::Column::GUID, true);
    //treeView.setColumnHidden(DataHierarchyModelItem::Column::GroupIndex, !_groupingAction.isChecked());
}

void DataHierarchyWidget::updateItemExpansion(const QModelIndex& filterModelIndex /*= QModelIndex()*/)
{
    if (!filterModelIndex.isValid())
        return;

    const auto modelIndex   = _filterModel.mapToSource(filterModelIndex);
    const auto isExpanded   = _model.getItem(modelIndex)->getDataset()->getDataHierarchyItem().isExpanded();

    auto& treeView = _hierarchyWidget.getTreeView();

    if (treeView.isExpanded(filterModelIndex) != isExpanded)
        treeView.setExpanded(filterModelIndex, isExpanded);
}

void DataHierarchyWidget::initializeExpansion(QModelIndex parentFilterModelIndex /*= QModelIndex()*/)
{
    for (int rowIndex = 0; rowIndex < _filterModel.rowCount(parentFilterModelIndex); ++rowIndex) {
        const auto childFilterModelIndex    = _filterModel.index(rowIndex, 0, parentFilterModelIndex);
        const auto childModelIndex          = _filterModel.mapToSource(childFilterModelIndex);

        updateItemExpansion(childFilterModelIndex);

        if (_model.hasChildren(childModelIndex))
            initializeExpansion(childFilterModelIndex);
    }
}
