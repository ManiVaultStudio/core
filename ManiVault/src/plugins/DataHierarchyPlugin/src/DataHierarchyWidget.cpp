// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DataHierarchyWidget.h"
#include "DataHierarchyWidgetContextMenu.h"

#include <Application.h>
#include <Set.h>
#include <Dataset.h>

#include <QDebug>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QStyledItemDelegate>
#include <QStyleOptionViewItem>
#include <QClipboard>

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
    ProgressItemDelegateEditorWidget(AbstractDataHierarchyModel::ProgressItem* progressItem, QWidget* parent) :
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
    void updateEditorWidgetVisibility() const {
        const auto datasetTaskStatus = _progressItem->getDatasetTask().getStatus();

        if (datasetTaskStatus == Task::Status::Running || datasetTaskStatus == Task::Status::RunningIndeterminate || datasetTaskStatus == Task::Status::Finished)
            _progressEditorWidget->setVisible(true);
        else
            _progressEditorWidget->setVisible(false);
    }

    /** Updates the editor widget read-only state based on the dataset task status */
    void updateEditorWidgetReadOnly() const {
        _progressEditorWidget->setEnabled(!_progressItem->getDataset()->isLocked());
    }

private:
    AbstractDataHierarchyModel::ProgressItem*   _progressItem;              /** Reference to the progress item */
    QWidget*                                    _progressEditorWidget;      /** Pointer to created editor widget */
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
     * @param dataHierarchyWidget Pointer to owning parent data hierarchy widget
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
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override {
        if (static_cast<AbstractDataHierarchyModel::Column>(index.column()) != AbstractDataHierarchyModel::Column::Progress)
            return QStyledItemDelegate::createEditor(parent, option, index);

        const auto sourceModelIndex = _dataHierarchyWidget->getFilterModel().mapToSource(index);
        const auto progressItem     = dynamic_cast<AbstractDataHierarchyModel::ProgressItem*>(_dataHierarchyWidget->getTreeModel().itemFromIndex(sourceModelIndex));
        
        return new ProgressItemDelegateEditorWidget(progressItem, parent);
    }

    /**
     * Update \p editor widget geometry when the cell geometry changes
     * @param option Style option
     * @param index Model index of the cell for which the geometry changed
     */
    void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const override {
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

        auto item = dynamic_cast<AbstractDataHierarchyModel::Item*>(_dataHierarchyWidget->getTreeModel().itemFromIndex(_dataHierarchyWidget->getFilterModel().mapToSource(index)));

        if (item->getDataset()->isLocked())// || index.column() >= static_cast<int>(AbstractDataHierarchyModel::Column::IsGroup))
            option->state &= ~QStyle::State_Enabled;
    }

private:
    DataHierarchyWidget* _dataHierarchyWidget;  /** Pointer to owning data hierarchy widget */
};

DataHierarchyWidget::DataHierarchyWidget(QWidget* parent) :
    QWidget(parent),
    _treeModel(this),
    _filterModel(this),
    _hierarchyWidget(this, "Dataset", _treeModel, &_filterModel),
    _resetAction(this, "Reset"),
    _unhideAction(this, "Unhide"),
    _statisticsAction(this, "Statistics")
{
    setObjectName("DataHierarchyWidget");

    auto layout = new QVBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(&_hierarchyWidget, 1);

    setLayout(layout);

    _hierarchyWidget.setWindowIcon(Application::getIconFont("FontAwesome").getIcon("database"));
    _hierarchyWidget.setNoItemsDescription(mv::plugins().getPluginFactoriesByType(plugin::Type::LOADER).empty() ? "No loader plugins available"  : "Right-click > Import to load data");

    auto& filterGroupAction = _hierarchyWidget.getFilterGroupAction();

    filterGroupAction.setPopupSizeHint(QSize(300, 0));

    filterGroupAction.addAction(&_filterModel.getVisibilityFilterAction());
    filterGroupAction.addAction(&_filterModel.getGroupFilterAction());
    filterGroupAction.addAction(&_filterModel.getLockedFilterAction());
    filterGroupAction.addAction(&_filterModel.getDerivedFilterAction());

    auto& settingsGroupAction = _hierarchyWidget.getSettingsGroupAction();

    settingsGroupAction.setVisible(true);
    settingsGroupAction.setShowLabels(false);

    auto selectionGroupingAction = mv::data().getSelectionGroupingAction();

    settingsGroupAction.addAction(selectionGroupingAction);
    settingsGroupAction.addAction(&_resetAction);

    auto& toolbarAction = _hierarchyWidget.getToolbarAction();

    toolbarAction.addAction(&_unhideAction);
    toolbarAction.addAction(&_statisticsAction);

    auto collapseAllAction = new QAction(this);

    collapseAllAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Minus));

    this->addAction(collapseAllAction);

    connect(collapseAllAction, &TriggerAction::triggered, this, [this]() -> void {
        _hierarchyWidget.getCollapseAllAction().trigger();
    });

    auto expandAllAction = new QAction(this);

    expandAllAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Plus));

    this->addAction(expandAllAction);

    connect(expandAllAction, &TriggerAction::triggered, this, [this]() -> void {
        _hierarchyWidget.getExpandAllAction().trigger();
	});

    auto& treeView = _hierarchyWidget.getTreeView();

    treeView.setItemDelegate(new ItemDelegate(this));

    treeView.setColumnHidden(static_cast<int>(AbstractDataHierarchyModel::Column::Location), true);
    treeView.setColumnHidden(static_cast<int>(AbstractDataHierarchyModel::Column::DatasetId), true);
    treeView.setColumnHidden(static_cast<int>(AbstractDataHierarchyModel::Column::RawDataName), true);
    treeView.setColumnHidden(static_cast<int>(AbstractDataHierarchyModel::Column::RawDataSize), true);
    treeView.setColumnHidden(static_cast<int>(AbstractDataHierarchyModel::Column::SourceDatasetId), true);
    treeView.setColumnHidden(static_cast<int>(AbstractDataHierarchyModel::Column::SelectionGroupIndex), true);
    treeView.setColumnHidden(static_cast<int>(AbstractDataHierarchyModel::Column::IsVisible), true);
    treeView.setColumnHidden(static_cast<int>(AbstractDataHierarchyModel::Column::IsGroup), true);

    auto treeViewHeader = treeView.header();

    treeViewHeader->setStretchLastSection(false);
    treeViewHeader->setMinimumSectionSize(18);

    treeViewHeader->resizeSection(AbstractDataHierarchyModel::Column::Name, 150);
    treeViewHeader->resizeSection(AbstractDataHierarchyModel::Column::SelectionGroupIndex, treeViewHeader->minimumSectionSize());
    treeViewHeader->resizeSection(AbstractDataHierarchyModel::Column::IsVisible, treeViewHeader->minimumSectionSize());
    treeViewHeader->resizeSection(AbstractDataHierarchyModel::Column::IsGroup, treeViewHeader->minimumSectionSize());
    treeViewHeader->resizeSection(AbstractDataHierarchyModel::Column::IsDerived, treeViewHeader->minimumSectionSize());
    treeViewHeader->resizeSection(AbstractDataHierarchyModel::Column::IsSubset, treeViewHeader->minimumSectionSize());
    treeViewHeader->resizeSection(AbstractDataHierarchyModel::Column::IsLocked, treeViewHeader->minimumSectionSize());

    treeViewHeader->setSectionResizeMode(AbstractDataHierarchyModel::Column::Name, QHeaderView::Interactive);
    treeViewHeader->setSectionResizeMode(AbstractDataHierarchyModel::Column::DatasetId, QHeaderView::Stretch);
    treeViewHeader->setSectionResizeMode(AbstractDataHierarchyModel::Column::RawDataName, QHeaderView::Stretch);
    treeViewHeader->setSectionResizeMode(AbstractDataHierarchyModel::Column::RawDataSize, QHeaderView::Stretch);
    treeViewHeader->setSectionResizeMode(AbstractDataHierarchyModel::Column::SelectionGroupIndex, QHeaderView::Fixed);
    treeViewHeader->setSectionResizeMode(AbstractDataHierarchyModel::Column::Progress, QHeaderView::Stretch);
    treeViewHeader->setSectionResizeMode(AbstractDataHierarchyModel::Column::IsVisible, QHeaderView::Fixed);
    treeViewHeader->setSectionResizeMode(AbstractDataHierarchyModel::Column::IsGroup, QHeaderView::Fixed);
    treeViewHeader->setSectionResizeMode(AbstractDataHierarchyModel::Column::IsDerived, QHeaderView::Fixed);
    treeViewHeader->setSectionResizeMode(AbstractDataHierarchyModel::Column::IsSubset, QHeaderView::Fixed);
    treeViewHeader->setSectionResizeMode(AbstractDataHierarchyModel::Column::IsLocked, QHeaderView::Fixed);

    const auto updateSelectionGroupIndexColumnVisibility = [this]() -> void {
        if (!mv::projects().hasProject())
            return;

        _hierarchyWidget.getTreeView().setColumnHidden(AbstractDataHierarchyModel::Column::SelectionGroupIndex, !mv::projects().getCurrentProject()->getSelectionGroupingAction().isChecked());
    };

    const auto linkSelectionGroupingAction = [this, updateSelectionGroupIndexColumnVisibility]() -> void {
        if (!mv::projects().hasProject())
            return;

        updateSelectionGroupIndexColumnVisibility();

        connect(&mv::projects().getCurrentProject()->getSelectionGroupingAction(), &ToggleAction::toggled, this, updateSelectionGroupIndexColumnVisibility);
    };

    linkSelectionGroupingAction();

    connect(&mv::projects(), &AbstractProjectManager::projectOpened, this, linkSelectionGroupingAction);

    connect(&_resetAction, &TriggerAction::triggered, &Application::core()->getDataManager(), &AbstractDataManager::reset);

    connect(&_filterModel, &QSortFilterProxyModel::rowsInserted, this, [this, &treeView](const QModelIndex& parent, int first, int last) -> void {
        for (int rowIndex = first; rowIndex <= last; rowIndex++) {
            const auto progressFilterModelIndex = _filterModel.index(rowIndex, static_cast<int>(AbstractDataHierarchyModel::Column::Progress), parent);
            const auto nameFilterModelIndex     = _filterModel.index(rowIndex, static_cast<int>(AbstractDataHierarchyModel::Column::Name), parent);
            const auto nameModelIndex           = _filterModel.mapToSource(nameFilterModelIndex);
            const auto persistentNameModelIndex = QPersistentModelIndex(nameModelIndex);

            _hierarchyWidget.getTreeView().openPersistentEditor(progressFilterModelIndex);

            updateDataHierarchyItemExpansion(nameModelIndex);

            auto item = _treeModel.getItem<AbstractDataHierarchyModel::Item>(persistentNameModelIndex);

            const auto datasetId = item->getDataset()->getId();

            connect(&item->getDataset()->getDataHierarchyItem(), &DataHierarchyItem::expandedChanged, this, [this, datasetId]() -> void {
                updateDataHierarchyItemExpansion(_treeModel.getModelIndex(datasetId));
            });

            connect(&item->getDataset()->getDataHierarchyItem(), &DataHierarchyItem::selectedChanged, this, [this, persistentNameModelIndex](bool selected) -> void {
                _hierarchyWidget.getSelectionModel().select(_filterModel.mapFromSource(persistentNameModelIndex), selected ? QItemSelectionModel::Rows | QItemSelectionModel::Select : QItemSelectionModel::Rows | QItemSelectionModel::Deselect);
            });
        }

        QCoreApplication::processEvents();
    });

    connect(&treeView, &QTreeView::expanded, this, [this](const QModelIndex& filterModelIndex) -> void {
        if (!filterModelIndex.isValid())
            return;

        auto modelIndex = _filterModel.mapToSource(filterModelIndex);

        if (modelIndex.isValid())
            _treeModel.getItem(modelIndex)->getDataset()->getDataHierarchyItem().setExpanded(true);
    });

    connect(&treeView, &QTreeView::collapsed, this, [this](const QModelIndex& filterModelIndex) -> void {
        if (!filterModelIndex.isValid())
            return;

        auto modelIndex = _filterModel.mapToSource(filterModelIndex);

        if (modelIndex.isValid())
            _treeModel.getItem(modelIndex)->getDataset()->getDataHierarchyItem().setExpanded(false);
    });

    connect(&_filterModel, &QSortFilterProxyModel::rowsAboutToBeRemoved, this, [this](const QModelIndex& parent, int first, int last) -> void {
        for (int rowIndex = first; rowIndex <= last; rowIndex++)
            _hierarchyWidget.getTreeView().closePersistentEditor(_filterModel.index(rowIndex, static_cast<int>(AbstractDataHierarchyModel::Column::Progress), parent));

        QCoreApplication::processEvents();
    });

    connect(&_hierarchyWidget.getSelectionModel(), &QItemSelectionModel::selectionChanged, this, [this](const QItemSelection& selected, const QItemSelection& deselected) {
        try {
            for (const auto& deselectedRange : deselected) {
                for (int rowIndex = deselectedRange.top(); rowIndex <= deselectedRange.bottom(); rowIndex++) {
                    const auto deselectedSourceModelIndex = _filterModel.mapToSource(_filterModel.index(rowIndex, 0, deselectedRange.parent()));

                    if (!deselectedSourceModelIndex.isValid())
                        throw std::runtime_error("Deselected source model index is not valid");

                    auto item = _treeModel.getItem(deselectedSourceModelIndex);

                    if (!item)
                        throw std::runtime_error("Item not found in the data hierarchy model");

                    item->getDataset()->getDataHierarchyItem().deselect();
                }
            }
        }
        catch (std::exception& e)
        {
            exceptionMessageBox("Unable to deselect data hierarchy item(s)", e);
        }
        catch (...) {
            exceptionMessageBox("Unable to deselect data hierarchy items(s)");
        }
        
        try {
            for (const auto& selectedRange : selected) {
                for (int rowIndex = selectedRange.top(); rowIndex <= selectedRange.bottom(); rowIndex++) {
                    const auto selectedSourceModelIndex = _filterModel.mapToSource(_filterModel.index(rowIndex, 0, selectedRange.parent()));

                    if (!selectedSourceModelIndex.isValid())
                        throw std::runtime_error("Selected source model index is not valid");

                    auto item = _treeModel.getItem(selectedSourceModelIndex);

                    if (!item)
                        throw std::runtime_error("Item not found in the data hierarchy model");

                    item->getDataset()->getDataHierarchyItem().select(false);
                }
            }
        }
        catch (std::exception& e)
        {
            exceptionMessageBox("Unable to select data hierarchy item(s)", e);
        }
        catch (...) {
            exceptionMessageBox("Unable to select data hierarchy item(s)");
        }
    });

    connect(&_hierarchyWidget.getTreeView(), &QTreeView::customContextMenuRequested, this, [this](const QPoint& position) {

        auto createContextMenu = [this, position]() -> void {
            Datasets datasets;

            for (const auto& selectedRow : _hierarchyWidget.getSelectedRows())
                datasets << _treeModel.getItem(selectedRow)->getDataset();

            QScopedPointer<DataHierarchyWidgetContextMenu> datasetsContextMenu(new DataHierarchyWidgetContextMenu(this, datasets));

            datasetsContextMenu->exec(_hierarchyWidget.getTreeView().viewport()->mapToGlobal(position));

        };

        QTimer::singleShot(10, createContextMenu);
    });

    initializeChildModelItemsExpansion();
    initializeSelection();

    for (auto dataHierarchyItem : mv::dataHierarchy().getItems()) {
        connect(dataHierarchyItem, &DataHierarchyItem::selectedChanged, this, [this, dataHierarchyItem](bool selected) -> void {
            const auto nameModelIndex = getModelIndexByDataset(dataHierarchyItem->getDataset());

            if (nameModelIndex.isValid())
                _hierarchyWidget.getSelectionModel().select(_filterModel.mapFromSource(nameModelIndex), selected ? QItemSelectionModel::Rows | QItemSelectionModel::Select : QItemSelectionModel::Rows | QItemSelectionModel::Deselect);
        });
    }

    connect(&_hierarchyWidget.getTreeView(), &QTreeView::clicked, this, [this](const QModelIndex& index) -> void {
        if (index.column() != static_cast<int>(AbstractDataHierarchyModel::Column::DatasetId))
            return;

        const auto datasetId    = _treeModel.getItem(_filterModel.mapToSource(index))->getDataset()->getId();
        const auto datasetIdLog = _treeModel.getItem(_filterModel.mapToSource(index))->getDataset()->getId(mv::settings().getMiscellaneousSettings().getShowSimplifiedGuidsAction().isChecked());

        QGuiApplication::clipboard()->setText(datasetId);

        qDebug() << "Dataset identifier" << datasetIdLog << "copied to clipboard";
	});
}

QModelIndex DataHierarchyWidget::getModelIndexByDataset(const Dataset<DatasetImpl>& dataset) const
{
    const auto modelIndices = _treeModel.match(_treeModel.index(0, static_cast<int>(AbstractDataHierarchyModel::Column::DatasetId), QModelIndex()), Qt::EditRole, dataset->getId(), 1, Qt::MatchFlag::MatchRecursive);

    if (modelIndices.isEmpty())
        throw std::runtime_error(QString("'%1' not found in the data hierarchy model").arg(dataset->text()).toLatin1());

    return modelIndices.first();
}

void DataHierarchyWidget::updateDataHierarchyItemExpansion(const QModelIndex& modelIndex /*= QModelIndex()*/)
{
    try
    {
        if (!modelIndex.isValid())
            return;

        auto modelItem = _treeModel.getItem(modelIndex);

        if (!modelItem)
            throw std::runtime_error("Model item not found");

        if (modelItem != nullptr) {
            const auto isExpanded       = modelItem->getDataset()->getDataHierarchyItem().isExpanded();
            const auto filterModelIndex = _filterModel.mapFromSource(modelIndex);

            auto& treeView = _hierarchyWidget.getTreeView();

            if (treeView.isExpanded(filterModelIndex) != isExpanded)
                treeView.setExpanded(filterModelIndex, isExpanded);
        }
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to update data hierarchy item expansion", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to update data hierarchy item expansion");
    }
}

void DataHierarchyWidget::initializeChildModelItemsExpansion(QModelIndex parentFilterModelIndex /*= QModelIndex()*/)
{
    try
    {
        for (int rowIndex = 0; rowIndex < _filterModel.rowCount(parentFilterModelIndex); ++rowIndex) {
            const auto childFilterModelIndex = _filterModel.index(rowIndex, 0, parentFilterModelIndex);

            if (!childFilterModelIndex.isValid())
                throw std::runtime_error("Supplied child filter model index is invalid");

            const auto childModelIndex = _filterModel.mapToSource(childFilterModelIndex);

            if (!childModelIndex.isValid())
                throw std::runtime_error("Supplied child model index is invalid");

            const auto persistentChildModelIndex = QPersistentModelIndex(childModelIndex);

            updateDataHierarchyItemExpansion(childModelIndex);

            auto childItem = _treeModel.getItem<AbstractDataHierarchyModel::Item>(persistentChildModelIndex);

            if (childItem == nullptr)
                throw std::runtime_error("Unable to get child model item for child model index");

            auto& dataset = childItem->getDataset();

            if (!dataset.isValid())
                throw std::runtime_error("Dataset is invalid");

            const auto& datasetId = dataset->getId();

            connect(&childItem->getDataset()->getDataHierarchyItem(), &DataHierarchyItem::expandedChanged, this, [this, datasetId]() -> void {
                const auto matches = _treeModel.match(_treeModel.index(0, static_cast<int>(AbstractDataHierarchyModel::Column::DatasetId)), Qt::EditRole, datasetId, -1);

                if (matches.isEmpty())
                    return;

                updateDataHierarchyItemExpansion(matches.first());
            });

            if (_treeModel.hasChildren(childModelIndex))
                initializeChildModelItemsExpansion(childFilterModelIndex);
        }
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to initialize model item expansion", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to initialize model item expansion");
    }
}

void DataHierarchyWidget::initializeSelection()
{
    try {
        QItemSelection itemSelection;

        for (auto selectedItem : dataHierarchy().getSelectedItems()) {
            const auto matches = _treeModel.match(_treeModel.index(0, static_cast<int>(AbstractDataHierarchyModel::Column::DatasetId)), Qt::EditRole, selectedItem->getDataset()->getId(), -1, Qt::MatchRecursive | Qt::MatchExactly);

            if (matches.isEmpty())
                return;

            itemSelection << QItemSelectionRange(_filterModel.mapFromSource(matches.first()));
        }

        auto& treeView = _hierarchyWidget.getTreeView();

        treeView.setFocus();
        treeView.selectionModel()->select(itemSelection, QItemSelectionModel::Rows | QItemSelectionModel::ClearAndSelect);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to initialize model selection", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to initialize model selection");
    }
}
