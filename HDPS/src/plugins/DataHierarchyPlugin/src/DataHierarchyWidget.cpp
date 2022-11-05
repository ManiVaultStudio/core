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

    /**
     * Init the style option(s) for the item delegate (we override the options to paint disabled when locked)
     */
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
    _treeView(this),
    _selectionModel(&_filterModel),
    _noDataOverlayWidget(new NoDataOverlayWidget(this)),
    _datasetNameFilterAction(this, "Dataset name filter"),
    _dataHierarchyFilterAction(this, _filterModel),
    _expandAllAction(this, "Expand all"),
    _collapseAllAction(this, "Collapse all"),
    _groupingAction(this, "Selection grouping", Application::core()->isDatasetGroupingEnabled(), Application::core()->isDatasetGroupingEnabled())
{
    _filterModel.setSourceModel(&_model);

    _treeView.setModel(&_filterModel);

    _treeView.setContextMenuPolicy(Qt::CustomContextMenu);
    _treeView.setSelectionModel(&_selectionModel);
    _treeView.setDragEnabled(true);
    _treeView.setDragDropMode(QAbstractItemView::DragOnly);
    _treeView.setSelectionBehavior(QAbstractItemView::SelectRows);
    _treeView.setSelectionMode(QAbstractItemView::ExtendedSelection);
    _treeView.setRootIsDecorated(true);
    _treeView.setItemsExpandable(true);
    _treeView.setIconSize(QSize(14, 14));
    _treeView.setItemDelegate(new ItemDelegate());

    _treeView.header()->setStretchLastSection(false);
    _treeView.header()->setMinimumSectionSize(18);

    _treeView.header()->resizeSection(DataHierarchyModelItem::Column::Name, 180);
    _treeView.header()->resizeSection(DataHierarchyModelItem::Column::GroupIndex, 60);
    _treeView.header()->resizeSection(DataHierarchyModelItem::Column::Progress, 45);
    _treeView.header()->resizeSection(DataHierarchyModelItem::Column::IsGroup, _treeView.header()->minimumSectionSize());
    _treeView.header()->resizeSection(DataHierarchyModelItem::Column::IsAnalyzing, _treeView.header()->minimumSectionSize());
    _treeView.header()->resizeSection(DataHierarchyModelItem::Column::IsLocked, _treeView.header()->minimumSectionSize());

    _treeView.header()->setSectionResizeMode(DataHierarchyModelItem::Column::Name, QHeaderView::Interactive);
    _treeView.header()->setSectionResizeMode(DataHierarchyModelItem::Column::GUID, QHeaderView::Fixed);
    _treeView.header()->setSectionResizeMode(DataHierarchyModelItem::Column::GroupIndex, QHeaderView::Fixed);
    _treeView.header()->setSectionResizeMode(DataHierarchyModelItem::Column::Info, QHeaderView::Stretch);
    _treeView.header()->setSectionResizeMode(DataHierarchyModelItem::Column::Progress, QHeaderView::Fixed);
    _treeView.header()->setSectionResizeMode(DataHierarchyModelItem::Column::IsGroup, QHeaderView::Fixed);
    _treeView.header()->setSectionResizeMode(DataHierarchyModelItem::Column::IsAnalyzing, QHeaderView::Fixed);
    _treeView.header()->setSectionResizeMode(DataHierarchyModelItem::Column::IsLocked, QHeaderView::Fixed);

    _datasetNameFilterAction.setPlaceHolderString("Filter by name or regular expression...");
    _datasetNameFilterAction.setSearchMode(true);
    _datasetNameFilterAction.setClearable(true);

    _expandAllAction.setIcon(Application::getIconFont("FontAwesome").getIcon("angle-double-down"));
    _expandAllAction.setToolTip("Expand all datasets in the hierarchy");

    connect(&_expandAllAction, &TriggerAction::triggered, this, &DataHierarchyWidget::expandAll);

    _collapseAllAction.setIcon(Application::getIconFont("FontAwesome").getIcon("angle-double-up"));
    _collapseAllAction.setToolTip("Collapse all datasets in the hierarchy");

    connect(&_collapseAllAction, &TriggerAction::triggered, this, &DataHierarchyWidget::collapseAll);

    _groupingAction.setIcon(Application::getIconFont("FontAwesome").getIcon("object-group"));
    _groupingAction.setToolTip("Enable/disable dataset grouping");

    connect(&_groupingAction, &ToggleAction::toggled, this, &DataHierarchyWidget::onGroupingActionToggled);

    auto layout = new QVBoxLayout();

    layout->setContentsMargins(6, 6, 6, 6);
    layout->setSpacing(3);

    auto toolbarLayout = new QHBoxLayout();

    toolbarLayout->setContentsMargins(0, 3, 0, 3);
    toolbarLayout->setSpacing(4);
    toolbarLayout->addWidget(_datasetNameFilterAction.createWidget(this), 1);
    toolbarLayout->addWidget(_dataHierarchyFilterAction.createCollapsedWidget(this));
    toolbarLayout->addWidget(_expandAllAction.createWidget(this, ToggleAction::PushButtonIcon));
    toolbarLayout->addWidget(_collapseAllAction.createWidget(this, ToggleAction::PushButtonIcon));
    toolbarLayout->addWidget(_groupingAction.createWidget(this, ToggleAction::CheckBox));

    layout->addLayout(toolbarLayout);
    layout->addWidget(&_treeView);

    setLayout(layout);

    connect(&_datasetNameFilterAction, &StringAction::stringChanged, this, [this](const QString& value) {
        auto re = QRegularExpression(value);

        if (re.isValid())
            _filterModel.setFilterRegularExpression(re);
        else
            _filterModel.setFilterFixedString(value);
    });

    connect(&_selectionModel, &QItemSelectionModel::selectionChanged, this, [this](const QItemSelection& selected, const QItemSelection& deselected) {
        DataHierarchyItems dataHierarchyItems;

        for (const auto& index : getSelectedRows())
            dataHierarchyItems << _model.getItem(index, Qt::DisplayRole)->getDataHierarchyItem();

        Application::core()->getDataHierarchyManager().selectItems(dataHierarchyItems);
    });

    connect(&_model, &QAbstractItemModel::rowsInserted, this, &DataHierarchyWidget::numberOfRowsChanged);
    connect(&_model, &QAbstractItemModel::rowsRemoved, this, &DataHierarchyWidget::numberOfRowsChanged);

    connect(&_filterModel, &QAbstractItemModel::rowsInserted, this, &DataHierarchyWidget::updateToolBar);
    connect(&_filterModel, &QAbstractItemModel::rowsRemoved, this, &DataHierarchyWidget::updateToolBar);

    connect(&_treeView, &QTreeView::expanded, this, &DataHierarchyWidget::expanded);
    connect(&_treeView, &QTreeView::collapsed, this, &DataHierarchyWidget::collapsed);

    // Add data hierarchy item to the widget when added in the data manager
    connect(&Application::core()->getDataHierarchyManager(), &DataHierarchyManager::itemAdded, this, &DataHierarchyWidget::addDataHierarchyItem);

    // Remove model item when a data hierarchy item is removed
    connect(&Application::core()->getDataHierarchyManager(), &DataHierarchyManager::itemAboutToBeRemoved, this, [this](const Dataset<DatasetImpl>& dataset) {

        // Clear the selection
        _selectionModel.clear();

        // Remove the item from the data hierarchy
        _model.removeDataHierarchyModelItem(getModelIndexByDataset(dataset));
    });

    // Invoked the custom context menu when requested by the tree view
    connect(&_treeView, &QTreeView::customContextMenuRequested, this, [this](const QPoint& position) {
        const auto selectedRows = getSelectedRows();

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

        contextMenu->exec(_treeView.viewport()->mapToGlobal(position));
    });

    // Update tool bar when items got expanded/collapsed
    connect(&_treeView, &QTreeView::expanded, this, &DataHierarchyWidget::updateToolBar);
    connect(&_treeView, &QTreeView::collapsed, this, &DataHierarchyWidget::updateToolBar);

    // Initial visibility of the overlay and header
    numberOfRowsChanged();

    // Initial update of tree view columns visibility
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

            if (getSelectedRows().contains(getModelIndexByDataset(dataset)))
                return;

            _selectionModel.select(_filterModel.mapFromSource(getModelIndexByDataset(dataset)), QItemSelectionModel::SelectionFlag::ClearAndSelect | QItemSelectionModel::SelectionFlag::Rows);
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
            if (!modelIndex.isValid() || expanded == _treeView.isExpanded(modelIndex))
                return;

            _treeView.setExpanded(modelIndex, expanded);
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
    // Search for the model index
    const auto modelIndices = _model.match(_model.index(0, 1), Qt::DisplayRole, dataset->getGuid(), 1, Qt::MatchFlag::MatchRecursive);

    // Except if not found
    if (modelIndices.isEmpty())
        throw new std::runtime_error(QString("'%1' not found in the data hierarchy model").arg(dataset->getGuiName()).toLatin1());

    // Return first match
    return modelIndices.first();
}

void DataHierarchyWidget::numberOfRowsChanged()
{
    const auto dataIsLoaded = _model.rowCount() >= 1;

    _noDataOverlayWidget->setVisible(!dataIsLoaded);

    _treeView.setHeaderHidden(!dataIsLoaded);

    updateToolBar();
}

void DataHierarchyWidget::onGroupingActionToggled(const bool& toggled)
{
    // Set in the core whether datasets may be grouped or not
    Application::core()->setDatasetGroupingEnabled(toggled);

    // Update the visibility of the tree view columns
    updateColumnsVisibility();
}

void DataHierarchyWidget::updateColumnsVisibility()
{
    _treeView.setColumnHidden(DataHierarchyModelItem::Column::GUID, true);
    _treeView.setColumnHidden(DataHierarchyModelItem::Column::GroupIndex, !_groupingAction.isChecked());
}

QModelIndexList DataHierarchyWidget::getSelectedRows() const
{
    QModelIndexList selectedRows;

    for (const auto& index : _selectionModel.selectedRows())
        selectedRows << _filterModel.mapToSource(index);

    return selectedRows;
}

QModelIndexList DataHierarchyWidget::gatherIndicesRecursively(QModelIndex filterModelParentIndex /*= QModelIndex()*/) const
{
    QModelIndexList modelIndexList;

    if (filterModelParentIndex.isValid())
        modelIndexList << filterModelParentIndex;

    for (int rowIndex = 0; rowIndex < _filterModel.rowCount(filterModelParentIndex); ++rowIndex) {
        QModelIndex index = _filterModel.index(rowIndex, 0, filterModelParentIndex);

        if (_filterModel.hasChildren(index))
            modelIndexList << gatherIndicesRecursively(index);
    }

    return modelIndexList;
}

bool DataHierarchyWidget::mayExpandAll() const
{
    const auto allModelIndexes = gatherIndicesRecursively();

    for (const auto& modelIndex : allModelIndexes)
        if (_model.rowCount(modelIndex) > 0 && !_treeView.isExpanded(modelIndex))
            return true;

    return false;
}

void DataHierarchyWidget::expandAll()
{
    const auto allFilterModelIndices = gatherIndicesRecursively();

    for (const auto& filterModelIndex : allFilterModelIndices)
        _treeView.setExpanded(filterModelIndex, true);
}

bool DataHierarchyWidget::mayCollapseAll() const
{
    const auto allFilterModelIndices = gatherIndicesRecursively();

    for (const auto& filterModelIndex : allFilterModelIndices)
        if (_model.rowCount(filterModelIndex) > 0 && _treeView.isExpanded(filterModelIndex))
            return true;

    return false;
}

void DataHierarchyWidget::collapseAll()
{
    const auto allFilterModelIndices = gatherIndicesRecursively();

    for (const auto& filterModelIndex : allFilterModelIndices)
        _treeView.setExpanded(filterModelIndex, false);
}

void DataHierarchyWidget::expanded(const QModelIndex& filterModelIndex)
{
    if (!filterModelIndex.isValid())
        return;

    _model.getItem(_filterModel.mapToSource(filterModelIndex), Qt::DisplayRole)->getDataHierarchyItem()->setExpanded(true);
}

void DataHierarchyWidget::collapsed(const QModelIndex& filterModelIndex)
{
    if (!filterModelIndex.isValid())
        return;

    _model.getItem(_filterModel.mapToSource(filterModelIndex), Qt::DisplayRole)->getDataHierarchyItem()->setExpanded(false);
}

void DataHierarchyWidget::updateToolBar()
{
    const auto dataHierarchyEmpty = _model.rowCount() <= 0;

    _expandAllAction.setEnabled(!dataHierarchyEmpty && mayExpandAll());
    _collapseAllAction.setEnabled(!dataHierarchyEmpty && mayCollapseAll());
    _groupingAction.setEnabled(!dataHierarchyEmpty);
}

DataHierarchyWidget::NoDataOverlayWidget::NoDataOverlayWidget(QWidget* parent) :
    QWidget(parent),
    _opacityEffect(new QGraphicsOpacityEffect(this))
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setGraphicsEffect(_opacityEffect);

    _opacityEffect->setOpacity(0.35);

    auto layout             = new QVBoxLayout();
    auto iconLabel          = new QLabel();
    auto titleLabel         = new QLabel("No data loaded");
    auto descriptionLabel   = new QLabel("Right-click to import data");

    const auto& fontAwesome = Application::getIconFont("FontAwesome");

    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->setFont(fontAwesome.getFont(14));
    iconLabel->setText(fontAwesome.getIconCharacter("database"));
    iconLabel->setStyleSheet("QLabel { padding: 10px; }");

    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-weight: bold");

    descriptionLabel->setAlignment(Qt::AlignCenter);

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->setAlignment(Qt::AlignCenter);

    layout->addStretch(1);
    layout->addWidget(iconLabel);
    layout->addWidget(titleLabel);
    layout->addWidget(descriptionLabel);
    layout->addStretch(1);

    setLayout(layout);

    // Install event filter for synchronizing widget size
    parent->installEventFilter(this);

    setObjectName("NoDataOverlayWidget");
    setStyleSheet("QWidget#NoDataOverlayWidget > QLabel { color: gray; }");
}

bool DataHierarchyWidget::NoDataOverlayWidget::eventFilter(QObject* target, QEvent* event)
{
    switch (event->type())
    {
        // Resize event
        case QEvent::Resize:
        {
            if (dynamic_cast<QWidget*>(target) != parent())
                break;

            // Set fixed overlay size
            setFixedSize(static_cast<QResizeEvent*>(event)->size());

            break;
        }

        default:
            break;
    }

    return QWidget::eventFilter(target, event);
}
