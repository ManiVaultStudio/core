// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DimensionsPickerAction.h"

#include "Application.h"

#include <QTableView>
#include <QHeaderView>
#include <QTime>
#include <QAbstractEventDispatcher>
#include <QVBoxLayout>

#include <deque>
#include <set>

#if !defined(Q_OS_MAC)
#ifndef Q_MOC_RUN
#if defined(emit) // tbb defines emit which clashes with Qt's emit
    #undef emit
    #include <execution>
    #define emit
#endif
#endif
#endif

using namespace mv;
using namespace mv::gui;

DimensionsPickerAction::DimensionsPickerAction(QObject* parent, const QString& title) :
    WidgetAction(parent, title),
    _points(nullptr),
    _itemModel(new DimensionsPickerItemModel(_holder)),
    _proxyModel(new DimensionsPickerProxyModel(_holder)),
    _summaryAction(this, "Summary"),
    _filterAction(*this),
    _selectAction(*this),
    _miscellaneousAction(*this)
{
    setText("Dimensions");
    setIconByName("table-columns");

    _summaryAction.setEnabled(false);

    updateSummary();

    _summaryUpdateAwakeConnection = connect(QAbstractEventDispatcher::instance(), &QAbstractEventDispatcher::awake,[this] {
        updateSummary();
    });

    /*
    const auto updateReadOnly = [this]() -> void {
        const auto enable = !isReadOnly();

        _filterAction.setEnabled(enable);
        _miscellaneousAction.setEnabled(enable);
        //_selectionThresholdAction.setEnabled(enable && !_selectionHolder._statistics.empty());
        _summaryAction.setEnabled(false);
        _selectAction.setEnabled(enable);
    };

    connect(this, &GroupAction::readOnlyChanged, this, [this, updateReadOnly](const bool& readOnly) {
        updateReadOnly();
    });

    updateReadOnly();
    */

    // Compute statistics when triggered
    connect(&_selectAction.getComputeStatisticsAction(), &TriggerAction::triggered, this, &DimensionsPickerAction::computeStatistics);
}

DimensionsPickerAction::~DimensionsPickerAction()
{
    disconnect(_summaryUpdateAwakeConnection);
}

void DimensionsPickerAction::fromVariantMap(const QVariantMap& variantMap)
{
    WidgetAction::fromVariantMap(variantMap);

    if (variantMap.contains("DatasetID")) {
        const auto datasetID = variantMap["DatasetID"].toString();

        if (!datasetID.isEmpty()) {
            auto dataset = mv::data().getDataset(datasetID);

            if (dataset.isValid())
                setPointsDataset(Dataset<Points>(dataset));
        }
    }

    if (variantMap.contains("EnabledDimensions")) {

        const auto enabledDimensions = variantMap["EnabledDimensions"].toList();

        if (enabledDimensions.count() != _holder.getNumberOfDimensions())
            return;

        std::int32_t dimensionIndex = 0;

        for (const auto& enabledDimension : enabledDimensions) {
            _holder.setDimensionEnabled(dimensionIndex, enabledDimension.toBool());
            dimensionIndex++;
        }

        const ModelResetter modelResetter(_proxyModel.get());
    }
}

QVariantMap DimensionsPickerAction::toVariantMap() const
{
    auto variantMap = WidgetAction::toVariantMap();

    QVariantList enabledDimensions;

    for (const auto enabledDimension : getEnabledDimensions())
        enabledDimensions << QVariant(enabledDimension);

    const auto datasetId = _points.isValid() ? _points->getId() : "";
    
    variantMap.insert({
        { "EnabledDimensions", enabledDimensions },
        { "DatasetID", datasetId }
    });

    return variantMap;
}

void DimensionsPickerAction::setDimensions(const std::uint32_t numDimensions, const std::vector<QString>& names)
{
    if (names.size() == numDimensions)
    {
        _holder = DimensionsPickerHolder(names.data(), numDimensions);
    }
    else
    {
        assert(names.empty());
        _holder = DimensionsPickerHolder(numDimensions);
    }

    auto dimensionSelectionItemModel = std::make_unique<DimensionsPickerItemModel>(_holder);
    auto proxyModel = std::make_unique<DimensionsPickerProxyModel>(_holder);

    proxyModel->setSourceModel(dimensionSelectionItemModel.get());

    _proxyModel    = std::move(proxyModel);
    _itemModel     = std::move(dimensionSelectionItemModel);

    updateSlider();

    connect(_itemModel.get(), &QAbstractItemModel::dataChanged, this, [this](const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles) -> void {
        emit selectedDimensionsChanged(getSelectedDimensions());
    });

    emit proxyModelChanged(_proxyModel.get());
}

const QVector<std::int32_t> DimensionsPickerAction::getSelectedDimensions() const
{
    const auto enabledDimensions = getEnabledDimensions();

    QVector<std::int32_t> selectedDimensions;

    selectedDimensions.reserve(enabledDimensions.size());

    for (int dimensionIndex = 0; dimensionIndex < enabledDimensions.size(); ++dimensionIndex)
        if (enabledDimensions[dimensionIndex])
            selectedDimensions << dimensionIndex;

    return selectedDimensions;
}

std::vector<bool> DimensionsPickerAction::getEnabledDimensions() const
{
    return _holder.getEnabledDimensions();
}

Dataset<Points> DimensionsPickerAction::getPointsDataset() const
{
    return _points;
}

void DimensionsPickerAction::setPointsDataset(const Dataset<Points>& points)
{
    disconnect(&_points, &Dataset<Points>::dataDimensionsChanged, this, nullptr);   // disconnect possible previous connection

    _points = points;

    if (_points.isValid()) {
        setDimensions(_points->getNumDimensions(), _points->getDimensionNames());
        setObjectName(QString("%1/Selection").arg(_points->text()));

        connect(&_points, &Dataset<Points>::dataDimensionsChanged, this, [this]() {
            setDimensions(_points->getNumDimensions(), _points->getDimensionNames());
            });

    } else
        setDimensions(0, {});
}

DimensionsPickerHolder& DimensionsPickerAction::getHolder()
{
    return _holder;
}

DimensionsPickerItemModel& DimensionsPickerAction::getItemModel()
{
    return *_itemModel;
}

DimensionsPickerProxyModel& DimensionsPickerAction::getProxyModel()
{
    return *_proxyModel;
}

void DimensionsPickerAction::setNameFilter(const QString& nameFilter)
{
    if (_proxyModel == nullptr)
        return;

    const ModelResetter modelResetter(_proxyModel.get());
    _proxyModel->setFilterRegularExpression(nameFilter);
}

void DimensionsPickerAction::setShowOnlySelectedDimensions(const bool& showOnlySelectedDimensions)
{
    if (_proxyModel == nullptr)
        return;

    const ModelResetter modelResetter(_proxyModel.get());
    _proxyModel->SetFilterShouldAcceptOnlySelected(showOnlySelectedDimensions);
}

void DimensionsPickerAction::setApplyExclusionList(const bool& applyExclusionList)
{
    if (_proxyModel == nullptr)
        return;

    const ModelResetter modelResetter(_proxyModel.get());
    _proxyModel->SetFilterShouldAppyExclusion(applyExclusionList);
}

void DimensionsPickerAction::setIgnoreZeroValues(const bool& ignoreZeroValues)
{
    const ModelResetter modelResetter(_proxyModel.get());
    _holder._ignoreZeroValues = ignoreZeroValues;
    
    updateSlider();
}

void DimensionsPickerAction::loadSelectionFromFile(const QString& fileName)
{
    QFile file(fileName);

    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        const ModelResetter modelResetter(_itemModel.get());

        _holder.disableAllDimensions();

        while (!file.atEnd())
        {
            const auto trimmedLine = file.readLine().trimmed();

            if (!trimmedLine.isEmpty())
            {
                const auto name = QString::fromUtf8(trimmedLine);

                if (!_holder.tryToEnableDimensionByName(name))
                    qWarning() << "Failed to select dimension (name not found): " << name;
            }
        }
    }
    else
    {
        qCritical() << "Load failed to open file: " << fileName;
    }
}

void DimensionsPickerAction::loadExclusionFromFile(const QString& fileName)
{
    if (fileName.isEmpty())
        return;

    QFile file(fileName);

    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        const ModelResetter modelResetter(_proxyModel.get());

        std::deque<QString> strings;

        while (!file.atEnd())
        {
            const auto trimmedLine = file.readLine().trimmed();

            if (!trimmedLine.isEmpty())
                strings.push_back(QString::fromUtf8(trimmedLine));
        }

        _proxyModel->SetExclusion(std::vector<QString>(strings.cbegin(), strings.cend()));
    }
    else
    {
        qCritical() << "Load exclusion failed to open file: " << fileName;
    }
}

void DimensionsPickerAction::saveSelectionToFile(const QString& fileName)
{
    if (fileName.isEmpty())
        return;

    QFile file(fileName);

    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        const auto numberOfDimensions = _holder.getNumberOfDimensions();

        for (std::size_t i{}; i < numberOfDimensions; ++i)
        {
            if (_holder.isDimensionEnabled(i))
            {
                file.write(_holder.getName(i).toUtf8());
                file.write("\n");
            }
        }
    }
    else
    {
        qCritical() << "Save failed to open file: " << fileName;
    }
}

void DimensionsPickerAction::selectDimension(const QString& dimensionName, bool clearExisiting /*= false*/)
{
    if (clearExisiting)
        _holder.disableAllDimensions();

    _holder.tryToEnableDimensionByName(dimensionName);
}

void DimensionsPickerAction::selectDimension(const std::int32_t& dimensionIndex, bool clearExisiting /*= false*/)
{
    if (clearExisiting)
        _holder.disableAllDimensions();

    _holder.setDimensionEnabled(dimensionIndex, true);
}

void DimensionsPickerAction::selectDimensions(const QVector<std::int32_t>& dimensionIndices, bool clearExisiting /*= true*/)
{
    if (dimensionIndices == getSelectedDimensions())
        return;

    const auto numberOfDimensions = static_cast<std::int32_t>(_holder.getNumberOfDimensions());

    if (clearExisiting)
        for (int dimensionIndex = 0; dimensionIndex < numberOfDimensions; ++dimensionIndex)
            _holder.setDimensionEnabled(dimensionIndex, false);

    for (const auto& dimensionIndex : dimensionIndices)
        _holder.setDimensionEnabled(dimensionIndex, true);

    const auto rowCount = _itemModel->rowCount(QModelIndex());

    emit _itemModel->dataChanged(_itemModel->index(0, 0, QModelIndex()), _itemModel->index(_itemModel->rowCount(QModelIndex()) - 1, 0, QModelIndex()));
}

void DimensionsPickerAction::computeStatistics()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    {
        const ModelResetter modelResetter(_proxyModel.get());

        auto& statistics = _holder._statistics;
        statistics.clear();

        if (_points.isValid())
        {
            QTime time = time.currentTime();
            
            const auto& pointData = *_points;

            pointData.visitFromBeginToEnd([&statistics, &pointData](auto beginOfData, auto endOfData)
            {
                const auto numberOfDimensions = pointData.getNumDimensions();
                const auto numberOfPoints = pointData.getNumPoints();

                constexpr static auto quiet_NaN = std::numeric_limits<double>::quiet_NaN();

                if (numberOfPoints == 0)
                {
                    statistics.resize(numberOfDimensions, { quiet_NaN, quiet_NaN, quiet_NaN, quiet_NaN });
                }
                else
                {
                    statistics.resize(numberOfDimensions);
                    const auto* const statisticsData = statistics.data();

                    if (numberOfPoints == 1)
                    {
#ifndef __APPLE__
                        (void)std::for_each_n(std::execution::par_unseq, statistics.begin(), numberOfDimensions,
#else
                        (void)std::for_each_n(statistics.begin(), numberOfDimensions,
#endif
                            [statisticsData, beginOfData](auto& statisticsPerDimension)
                        {
                            const auto i = &statisticsPerDimension - statisticsData;

                            const double dataValue = beginOfData[i];
                            statisticsPerDimension = { {dataValue, dataValue}, {quiet_NaN, quiet_NaN} };
                        });
                    }
                    else
                    {
#ifndef __APPLE__
                        (void)std::for_each_n(std::execution::par_unseq, statistics.begin(), numberOfDimensions,
#else
                        (void)std::for_each_n(statistics.begin(), numberOfDimensions,
#endif
                            [statisticsData, numberOfDimensions, numberOfPoints, beginOfData](auto& statisticsPerDimension)
                        {
                            const std::unique_ptr<double[]> data(new double[numberOfPoints]);
                            {
                                const auto i = &statisticsPerDimension - statisticsData;

                                for (unsigned j{}; j < numberOfPoints; ++j)
                                {
                                    data[j] = beginOfData[j * numberOfDimensions + i];
                                }
                            }

                            double sum{};
                            unsigned numberOfNonZeroValues{};

                            for (unsigned j{}; j < numberOfPoints; ++j)
                            {
                                const auto value = data[j];

                                if (value != 0.0)
                                {
                                    sum += value;
                                    ++numberOfNonZeroValues;
                                }
                            }
                            const auto mean = sum / numberOfPoints;

                            double sumOfSquares{};

                            for (unsigned j{}; j < numberOfPoints; ++j)
                            {
                                const auto value = data[j] - mean;
                                sumOfSquares += value * value;
                            }

                            static_assert(quiet_NaN != quiet_NaN);

                            statisticsPerDimension = StatisticsPerDimension
                            {
                                {
                                    mean,
                                    (numberOfNonZeroValues == 0) ? quiet_NaN : (sum / numberOfNonZeroValues)
                                },
                                {
                                    std::sqrt(sumOfSquares / (numberOfPoints - 1)),
                                    (numberOfNonZeroValues == 0) ? quiet_NaN : std::sqrt(sumOfSquares / numberOfNonZeroValues)
                                }
                            };
                        });
                    }
                }
            });
            qDebug()
                << " Duration: " << QTime::currentTime().msecsTo(time) << " microsecond(s)";

            for (unsigned i{}; i <= 1; ++i)
            {
                std::set<double> distinctStandardDeviations;

                for (const auto& statisticsPerDimension : _holder._statistics)
                {
                    if (!std::isnan(statisticsPerDimension.standardDeviation[i]))
                    {
                        distinctStandardDeviations.insert(statisticsPerDimension.standardDeviation[i]);
                    }
                }

                _holder.distinctStandardDeviationsWithAndWithoutZero[i].assign(distinctStandardDeviations.cbegin(), distinctStandardDeviations.cend());
            }

            assert(_selectAction.getSelectionThresholdAction().getMinimum() == 0);
            updateSlider();
        }
    }
    QApplication::restoreOverrideCursor();
}

void DimensionsPickerAction::updateSlider()
{
    const auto numberOfDistinctStandardDeviations   = _holder.distinctStandardDeviationsWithAndWithoutZero[_holder._ignoreZeroValues ? 1 : 0].size();
    const auto isSliderValueAtMaximum               = (_selectAction.getSelectionThresholdAction().isAtMaximum());

    if (numberOfDistinctStandardDeviations > 0 && numberOfDistinctStandardDeviations <= std::numeric_limits<int>::max())
    {
        const auto newMaximum = static_cast<int>(numberOfDistinctStandardDeviations) - 1;

        if (newMaximum != _selectAction.getSelectionThresholdAction().getMaximum())
            _selectAction.getSelectionThresholdAction().setMaximum(newMaximum);
        
        _selectAction.getSelectionThresholdAction().setValue(isSliderValueAtMaximum ? newMaximum : 0);
    }
    else
    {
        _selectAction.getSelectionThresholdAction().setValue(isSliderValueAtMaximum ? 1 : 0);
        _selectAction.getSelectionThresholdAction().setMaximum(1);
    }

    //_selectAction.getSelectionThresholdAction().setEnabled(!isReadOnly() && !_holder._statistics.empty());
}

void DimensionsPickerAction::updateSummary()
{
    const auto& holder = _holder;

    const auto numberOfDimensions           = holder.getNumberOfDimensions();
    const auto numberOfVisibleDimensions    = (_proxyModel == nullptr) ? 0 : _proxyModel->rowCount();

    _summaryAction.setString(tr("%1 available, %2 visible, %3 selected").arg(numberOfDimensions).arg(numberOfVisibleDimensions).arg(holder.getNumberOfSelectedDimensions()));
}

void DimensionsPickerAction::connectToPublicAction(WidgetAction* publicAction, bool recursive)
{
    auto publicDimensionsPickerAction = dynamic_cast<DimensionsPickerAction*>(publicAction);

    Q_ASSERT(publicDimensionsPickerAction != nullptr);

    if (publicDimensionsPickerAction == nullptr)
        return;

    connect(this, &DimensionsPickerAction::selectedDimensionsChanged, publicDimensionsPickerAction, [publicDimensionsPickerAction](const QVector<std::int32_t>& dimensionIndices) -> void {
        publicDimensionsPickerAction->selectDimensions(dimensionIndices);
    });

    connect(publicDimensionsPickerAction, &DimensionsPickerAction::selectedDimensionsChanged, this, [this](const QVector<std::int32_t>& dimensionIndices) -> void {
        selectDimensions(dimensionIndices);
    });

    selectDimensions(publicDimensionsPickerAction->getSelectedDimensions());

    WidgetAction::connectToPublicAction(publicAction, recursive);
}

void DimensionsPickerAction::disconnectFromPublicAction(bool recursive)
{
    if (!isConnected())
        return;

    auto publicDimensionsPickerAction = dynamic_cast<DimensionsPickerAction*>(getPublicAction());

    Q_ASSERT(publicDimensionsPickerAction != nullptr);

    if (publicDimensionsPickerAction == nullptr)
        return;

    disconnect(this, &DimensionsPickerAction::selectedDimensionsChanged, publicDimensionsPickerAction, nullptr);
    disconnect(publicDimensionsPickerAction, &DimensionsPickerAction::selectedDimensionsChanged, this, nullptr);

    WidgetAction::disconnectFromPublicAction(recursive);
}

DimensionsPickerAction::Widget::Widget(QWidget* parent, DimensionsPickerAction* dimensionsPickerAction, const std::int32_t& widgetFlags) :
    WidgetActionWidget(parent, dimensionsPickerAction),
    _tableView(this)
{
    setMinimumHeight(300);

    connect(dimensionsPickerAction, &DimensionsPickerAction::proxyModelChanged, this, &DimensionsPickerAction::Widget::updateTableViewModel);
    connect(dimensionsPickerAction, &DimensionsPickerAction::proxyModelChanged, this, [&](DimensionsPickerProxyModel* dimensionsPickerProxyModel) {
        Widget::updateTableViewModel(dimensionsPickerProxyModel); // implicit conversion from DimensionsPickerProxyModel to QAbstractItemModel 
        });

    auto layout = new QVBoxLayout();

    _tableView.setSortingEnabled(true);
    _tableView.setStyleSheet("QTableView::indicator:checked{ padding: 0px; margin: 0px;}");

    auto horizontalHeader = _tableView.horizontalHeader();

    horizontalHeader->setStretchLastSection(false);
    horizontalHeader->setDefaultAlignment(Qt::AlignLeft);
    horizontalHeader->setSortIndicator(2, Qt::DescendingOrder);

    //horizontalHeader->resizeSection(1, 85);
    //horizontalHeader->resizeSection(2, 85);

    _tableView.verticalHeader()->hide();
    _tableView.verticalHeader()->setDefaultSectionSize(5);

    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(&_tableView);

    auto toolbarLayout = new QHBoxLayout();

    toolbarLayout->addWidget(dimensionsPickerAction->getFilterAction().createCollapsedWidget(this));
    toolbarLayout->addWidget(dimensionsPickerAction->getSelectAction().createCollapsedWidget(this));
    toolbarLayout->addWidget(dimensionsPickerAction->getMiscellaneousAction().createCollapsedWidget(this));
    toolbarLayout->addWidget(dimensionsPickerAction->getSummaryAction().createWidget(this), 1);

    layout->addLayout(toolbarLayout);

    setLayout(layout);

    updateTableViewModel(&dimensionsPickerAction->getProxyModel());

    connect(dimensionsPickerAction, &DimensionsPickerAction::selectedDimensionsChanged, this, [this]() -> void {
        _tableView.horizontalHeader()->resizeSections(QHeaderView::ResizeMode::ResizeToContents);
        });
}

void DimensionsPickerAction::Widget::updateTableViewModel(QAbstractItemModel* model)
{
    _tableView.setModel(model);

    if (model->rowCount() == 0 || model->columnCount() == 0)
        return;

    _tableView.horizontalHeader()->resizeSections(QHeaderView::ResizeMode::ResizeToContents);
    _tableView.horizontalHeader()->resizeSection(1, 60);
    _tableView.horizontalHeader()->resizeSection(2, 60);
}
