#include "DataHierarchyWidget.h"
#include "DataHierarchyModel.h"
#include "DataHierarchyModelItem.h"

#include <Application.h>
#include <Dataset.h>
#include <PluginFactory.h>
#include <widgets/Divider.h>
#include <actions/PluginTriggerAction.h>

#include <QDebug>
#include <QHeaderView>
#include <QResizeEvent>
#include <QVBoxLayout>
#include <QMenu>
#include <QLabel>
#include <QStyleOptionViewItem>

#include <stdexcept>

using namespace hdps;
using namespace hdps::util;
using namespace hdps::plugin;

/**
 * Tree view item delegate class
 * Qt natively does not support disabled items to be selected, this class solves that
 * When an item (dataset) is locked, merely the visual representation is changed and not the item flags (only appears disabled)
 */
class ItemDelegate : public QStyledItemDelegate {
public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     */
    explicit ItemDelegate(QObject* parent = nullptr) :
        QStyledItemDelegate(parent)
    {
    }

protected:

    /** Init the style option(s) for the item delegate (we override the options to paint disabled when locked) */
    void initStyleOption(QStyleOptionViewItem* option, const QModelIndex& index) const override
    {
        QStyledItemDelegate::initStyleOption(option, index);

        if (index.siblingAtColumn(DataHierarchyModelItem::Column::IsLocked).data(Qt::EditRole).toBool())
            option->state &= ~QStyle::State_Enabled;
    }
};

DataHierarchyWidget::DataHierarchyWidget(QWidget* parent) :
    QWidget(parent),
    _model(this),
    _filterModel(this),
    _hierarchyWidget(this, "Dataset", _model, &_filterModel),
    _groupingAction(this, "Selection grouping", Application::core()->isDatasetGroupingEnabled(), Application::core()->isDatasetGroupingEnabled())
{
    auto layout = new QVBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(&_hierarchyWidget, 1);

    setLayout(layout);

    _hierarchyWidget.setWindowIcon(Application::getIconFont("FontAwesome").getIcon("database"));
    
    auto& treeView = _hierarchyWidget.getTreeView();

    auto treeViewHeader = treeView.header();

    treeViewHeader->setStretchLastSection(false);
    treeViewHeader->setMinimumSectionSize(18);

    treeViewHeader->resizeSection(DataHierarchyModelItem::Column::Name, 180);
    treeViewHeader->resizeSection(DataHierarchyModelItem::Column::GroupIndex, 60);
    treeViewHeader->resizeSection(DataHierarchyModelItem::Column::Progress, 45);
    treeViewHeader->resizeSection(DataHierarchyModelItem::Column::IsGroup, treeViewHeader->minimumSectionSize());
    treeViewHeader->resizeSection(DataHierarchyModelItem::Column::IsAnalyzing, treeViewHeader->minimumSectionSize());
    treeViewHeader->resizeSection(DataHierarchyModelItem::Column::IsLocked, treeViewHeader->minimumSectionSize());

    treeViewHeader->setSectionResizeMode(DataHierarchyModelItem::Column::Name, QHeaderView::Interactive);
    treeViewHeader->setSectionResizeMode(DataHierarchyModelItem::Column::GUID, QHeaderView::Fixed);
    treeViewHeader->setSectionResizeMode(DataHierarchyModelItem::Column::GroupIndex, QHeaderView::Fixed);
    treeViewHeader->setSectionResizeMode(DataHierarchyModelItem::Column::Info, QHeaderView::Stretch);
    treeViewHeader->setSectionResizeMode(DataHierarchyModelItem::Column::Progress, QHeaderView::Fixed);
    treeViewHeader->setSectionResizeMode(DataHierarchyModelItem::Column::IsGroup, QHeaderView::Fixed);
    treeViewHeader->setSectionResizeMode(DataHierarchyModelItem::Column::IsAnalyzing, QHeaderView::Fixed);
    treeViewHeader->setSectionResizeMode(DataHierarchyModelItem::Column::IsLocked, QHeaderView::Fixed);

    treeView.setItemDelegate(new ItemDelegate());

    _groupingAction.setIcon(Application::getIconFont("FontAwesome").getIcon("object-group"));
    _groupingAction.setToolTip("Enable/disable dataset grouping");

    connect(&_groupingAction, &ToggleAction::toggled, this, &DataHierarchyWidget::onGroupingActionToggled);

    connect(&Application::core()->getDataHierarchyManager(), &DataHierarchyManager::itemAdded, this, &DataHierarchyWidget::addDataHierarchyItem);

    connect(&Application::core()->getDataHierarchyManager(), &DataHierarchyManager::itemAboutToBeRemoved, this, [this](const Dataset<DatasetImpl>& dataset) {
        _hierarchyWidget.getSelectionModel().clear();

        _model.removeDataHierarchyModelItem(getModelIndexByDataset(dataset));
    });

    connect(&_hierarchyWidget.getSelectionModel(), &QItemSelectionModel::selectionChanged, this, [this](const QItemSelection& selected, const QItemSelection& deselected) {
        DataHierarchyItems dataHierarchyItems;

        //for (const auto& index : getSelectedRows())
        //    dataHierarchyItems << _model.getItem(index, Qt::DisplayRole)->getDataHierarchyItem();

        //Application::core()->getDataHierarchyManager().selectItems(dataHierarchyItems);
    });

    // Invoked the custom context menu when requested by the tree view
    connect(&_hierarchyWidget.getTreeView(), &QTreeView::customContextMenuRequested, this, [this](const QPoint& position) {
        const auto selectedRows = _hierarchyWidget.getSelectedRows();

        Datasets datasets;

        for (const auto& selectedRow : selectedRows)
            datasets << _model.getItem(selectedRow, Qt::DisplayRole)->getDataHierarchyItem()->getDataset();

        auto contextMenu = new QMenu();

        QMap<QString, QMenu*> menus;

        const auto addMenu = [contextMenu, &menus, datasets](const plugin::Type& pluginType) -> void {
            for (auto pluginTriggerAction : Application::core()->getPluginTriggerActions(pluginType, datasets)) {
                const auto titleSegments = pluginTriggerAction->getTitle().split("/");

                QString menuPath, previousMenuPath = titleSegments.first();

                for (auto titleSegment : titleSegments) {
                    if (titleSegment != titleSegments.first() && titleSegment != titleSegments.last())
                        menuPath += "/";

                    menuPath += titleSegment;

                    if (!menus.contains(menuPath)) {
                        menus[menuPath] = new QMenu(titleSegment);

                        if (titleSegment != titleSegments.first()) {
                            if (titleSegment == titleSegments.last())
                                menus[previousMenuPath]->addAction(pluginTriggerAction);
                            else {
                                if (titleSegment == "Group") {
                                    //menus[menuPath]->setIcon(Application::getIconFont("FontAwesome").getIcon("object-group"));

                                    if (menus[previousMenuPath]->actions().isEmpty())
                                        menus[previousMenuPath]->addMenu(menus[menuPath]);
                                    else
                                        menus[previousMenuPath]->insertMenu(menus[previousMenuPath]->actions().first(), menus[menuPath]);

                                    if (menus[previousMenuPath]->actions().count() >= 2)
                                        menus[previousMenuPath]->insertSeparator(menus[previousMenuPath]->actions()[1]);
                                } else
                                    menus[previousMenuPath]->addMenu(menus[menuPath]);
                            }
                                
                        } else
                            contextMenu->addMenu(menus[titleSegments.first()]);
                    }

                    previousMenuPath = menuPath;
                }
            }
            
            switch (pluginType)
            {
                case plugin::Type::ANALYSIS:
                {
                    if (!menus.contains("Analyze"))
                        break;

                    menus["Analyze"]->setTitle("Analyze");
                    menus["Analyze"]->setIcon(Application::getIconFont("FontAwesome").getIcon("square-root-alt"));
                    break;
                }

                case plugin::Type::LOADER:
                {
                    if (!menus.contains("Import"))
                        break;

                    menus["Import"]->setTitle("Import");
                    menus["Import"]->setIcon(Application::getIconFont("FontAwesome").getIcon("file-import"));
                    break;
                }

                case plugin::Type::WRITER:
                {
                    if (!menus.contains("Export"))
                        break;

                    menus["Export"]->setTitle("Export");
                    menus["Export"]->setIcon(Application::getIconFont("FontAwesome").getIcon("file-export"));
                    break;
                }

                case plugin::Type::TRANSFORMATION:
                {
                    if (!menus.contains("Transform"))
                        break;

                    menus["Transform"]->setTitle("Transform");
                    menus["Transform"]->setIcon(Application::getIconFont("FontAwesome").getIcon("random"));
                    break;
                }

                case plugin::Type::VIEW:
                {
                    if (!menus.contains("View"))
                        break;

                    menus["View"]->setTitle("View");
                    menus["View"]->setIcon(Application::getIconFont("FontAwesome").getIcon("eye"));
                    break;
                }

                default:
                    break;
            }
        };

        addMenu(plugin::Type::ANALYSIS);
        addMenu(plugin::Type::LOADER);
        addMenu(plugin::Type::WRITER);
        addMenu(plugin::Type::TRANSFORMATION);
        addMenu(plugin::Type::VIEW);

        QSet<DataType> dataTypes;

        for (const auto& dataset : datasets)
            dataTypes.insert(dataset->getDataType());

        if (datasets.count() >= 2 && dataTypes.count() == 1) {
            auto groupDataAction = new QAction("Group...");

            groupDataAction->setToolTip("Group datasets into one");
            groupDataAction->setIcon(Application::getIconFont("FontAwesome").getIcon("object-group"));

            connect(groupDataAction, &QAction::triggered, [this, datasets]() -> void {
                Application::core()->groupDatasets(datasets);
            });

            contextMenu->addSeparator();
            contextMenu->addAction(groupDataAction);
        }

        if (!Application::core()->requestAllDataSets().isEmpty()) {
            auto lockMenu   = new QMenu("Lock");
            auto unlockMenu = new QMenu("Unlock");
        
            lockMenu->setIcon(Application::getIconFont("FontAwesome").getIcon("lock"));
            unlockMenu->setIcon(Application::getIconFont("FontAwesome").getIcon("unlock"));

            auto lockAllAction = new QAction("All");

            connect(lockAllAction, &QAction::triggered, this, [this, datasets]() -> void {
                for (auto dataset : Application::core()->requestAllDataSets())
                    dataset->lock();
            });

            lockMenu->setEnabled(false);
            unlockMenu->setEnabled(false);

            QVector<bool> locked;

            for (auto dataset : Application::core()->requestAllDataSets())
                locked << dataset->isLocked();

            const auto numberOfLockedDatasets = std::accumulate(locked.begin(), locked.end(), 0);

            unlockMenu->setEnabled(numberOfLockedDatasets >= 1);
            lockMenu->setEnabled(numberOfLockedDatasets < Application::core()->requestAllDataSets().size());

            auto unlockAllAction = new QAction("All");

            connect(unlockAllAction, &QAction::triggered, this, [this, datasets]() -> void {
                for (auto dataset : Application::core()->requestAllDataSets())
                    dataset->unlock();
            });

            auto lockSelectedAction = new QAction("Selected");
        
            lockSelectedAction->setIcon(Application::getIconFont("FontAwesome").getIcon("mouse-pointer"));
            lockSelectedAction->setEnabled(!datasets.isEmpty());

            connect(lockSelectedAction, &QAction::triggered, this, [this, datasets]() -> void {
                for (auto dataset : datasets)
                    dataset->lock();
            });

            auto unlockSelectedAction   = new QAction("Selected");

            unlockSelectedAction->setIcon(Application::getIconFont("FontAwesome").getIcon("mouse-pointer"));
            unlockSelectedAction->setEnabled(!datasets.isEmpty());

            connect(unlockSelectedAction, &QAction::triggered, this, [this, datasets]() -> void {
                for (auto dataset : datasets)
                    dataset->unlock();
            });
        
            lockMenu->addAction(lockSelectedAction);
            lockMenu->addAction(lockAllAction);
            unlockMenu->addAction(unlockSelectedAction);
            unlockMenu->addAction(unlockAllAction);

            contextMenu->addSeparator();

            contextMenu->addMenu(lockMenu);
            contextMenu->addMenu(unlockMenu);
        }

        contextMenu->exec(_hierarchyWidget.getTreeView().viewport()->mapToGlobal(position));
    });

    updateColumnsVisibility();
}

void DataHierarchyWidget::addDataHierarchyItem(DataHierarchyItem& dataHierarchyItem)
{
    auto dataset = dataHierarchyItem.getDataset();

    try {
        QModelIndex parentModelIndex;

        if (!dataHierarchyItem.hasParent())
            parentModelIndex = QModelIndex();
        else
            parentModelIndex = getModelIndexByDataset(dataHierarchyItem.getParent().getDataset());;

        _model.addDataHierarchyModelItem(parentModelIndex, dataHierarchyItem);

        connect(&dataHierarchyItem, &DataHierarchyItem::taskDescriptionChanged, this, [this, dataset](const QString& description) {
            _model.setData(getModelIndexByDataset(dataset).siblingAtColumn(DataHierarchyModelItem::Column::Info), description);
        });

        connect(&dataHierarchyItem, &DataHierarchyItem::taskProgressChanged, this, [this, dataset](const float& progress) {
            _model.setData(getModelIndexByDataset(dataset).siblingAtColumn(DataHierarchyModelItem::Column::Progress), progress);
            _model.setData(getModelIndexByDataset(dataset).siblingAtColumn(DataHierarchyModelItem::Column::IsAnalyzing), progress > 0.0f);
        });

        connect(&dataHierarchyItem, &DataHierarchyItem::selectionChanged, this, [this, dataset](const bool& selection) {
            if (!selection)
                return;

            if (_hierarchyWidget.getSelectedRows().contains(getModelIndexByDataset(dataset)))
                return;

            _hierarchyWidget.getSelectionModel().select(_filterModel.mapFromSource(getModelIndexByDataset(dataset)), QItemSelectionModel::SelectionFlag::ClearAndSelect | QItemSelectionModel::SelectionFlag::Rows);
        });

        connect(&dataHierarchyItem, &DataHierarchyItem::lockedChanged, this, [this, dataset](const bool& locked) {
            emit _model.dataChanged(getModelIndexByDataset(dataset).siblingAtColumn(DataHierarchyModelItem::Column::Name), getModelIndexByDataset(dataset).siblingAtColumn(DataHierarchyModelItem::Column::IsLocked));
        });

        connect(&dataHierarchyItem, &DataHierarchyItem::visibilityChanged, this, [this](const bool& visible) {
            _filterModel.invalidate();
        });
        
        _filterModel.invalidate();

        connect(&dataHierarchyItem, &DataHierarchyItem::iconChanged, this, [this, dataset]() {
            emit _model.dataChanged(getModelIndexByDataset(dataset).siblingAtColumn(DataHierarchyModelItem::Column::Name), getModelIndexByDataset(dataset).siblingAtColumn(DataHierarchyModelItem::Column::Name));
        });

        const auto setModelItemExpansion = [this](const QModelIndex& modelIndex, const bool& expanded) -> void {
            if (!modelIndex.isValid() || expanded == _hierarchyWidget.getTreeView().isExpanded(modelIndex))
                return;

            _hierarchyWidget.getTreeView().setExpanded(modelIndex, expanded);
        };

        connect(&dataHierarchyItem, &DataHierarchyItem::expandedChanged, this, [this, dataset, setModelItemExpansion](const bool& expanded) {
            setModelItemExpansion(getModelIndexByDataset(dataset), expanded);
        });

        QCoreApplication::processEvents();

        setModelItemExpansion(getModelIndexByDataset(dataset), dataHierarchyItem.isExpanded());
    }
    catch (std::exception& e)
    {
        exceptionMessageBox(QString("Unable to add %1 to the data hierarchy tree widget").arg(dataset->getGuiName()), e);
    }
    catch (...) {
        exceptionMessageBox(QString("Unable to add %1 to the data hierarchy tree widget").arg(dataset->getGuiName()));
    }
}

QModelIndex DataHierarchyWidget::getModelIndexByDataset(const Dataset<DatasetImpl>& dataset)
{
    const auto modelIndices = _model.match(_model.index(0, 1), Qt::DisplayRole, dataset->getGuid(), 1, Qt::MatchFlag::MatchRecursive);

    if (modelIndices.isEmpty())
        throw new std::runtime_error(QString("'%1' not found in the data hierarchy model").arg(dataset->getGuiName()).toLatin1());

    return modelIndices.first();
}

void DataHierarchyWidget::onGroupingActionToggled(const bool& toggled)
{
    Application::core()->setDatasetGroupingEnabled(toggled);

    updateColumnsVisibility();
}

void DataHierarchyWidget::updateColumnsVisibility()
{
    auto& treeView = _hierarchyWidget.getTreeView();

    treeView.setColumnHidden(DataHierarchyModelItem::Column::GUID, true);
    treeView.setColumnHidden(DataHierarchyModelItem::Column::GroupIndex, !_groupingAction.isChecked());
}
