#include "DimensionsPickerAction.h"
#include "PointData.h"

#include "Application.h"

#include <QLabel>
#include <QFileDialog>
#include <QTableView>
#include <QHeaderView>
#include <QTime>
#include <QAbstractEventDispatcher>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include <deque>
#include <set>

#ifndef __APPLE__
    #include <execution>
#endif

using namespace hdps;
using namespace hdps::gui;

DimensionsPickerAction::DimensionsPickerAction(QObject* parent, const QString& title /*= "Dimensions"*/) :
    GroupAction(parent),
    _points(nullptr),
    _holder(),
    _itemModel(new DimensionsPickerItemModel(_holder)),
    _proxyModel(new DimensionsPickerProxyModel(_holder)),
    _summaryAction(this, "Summary"),
    _filterAction(*this),
    _selectAction(*this),
    _miscellaneousAction(*this),
    _summaryUpdateAwakeConnection()
{
    setText(title);
    setIcon(Application::getIconFont("FontAwesome").getIcon("columns"));

    _summaryAction.setEnabled(false);

    _summaryAction.setSerializable(false);
    _filterAction.setSerializable(false);
    _selectAction.setSerializable(false);
    _miscellaneousAction.setSerializable(false);

    updateSummary();

    _summaryUpdateAwakeConnection = connect(QAbstractEventDispatcher::instance(), &QAbstractEventDispatcher::awake,[this] {
        updateSummary();
    });

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

    // Compute statistics when triggered
    connect(&_selectAction.getComputeStatisticsAction(), &TriggerAction::triggered, this, &DimensionsPickerAction::computeStatistics);
}

DimensionsPickerAction::~DimensionsPickerAction()
{
    disconnect(_summaryUpdateAwakeConnection);
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
}

std::vector<bool> DimensionsPickerAction::getEnabledDimensions() const
{
    return _holder.getEnabledDimensions();
}

void DimensionsPickerAction::setPointsDataset(const Dataset<Points>& points)
{
    _points = points;

    setDimensions(_points->getNumDimensions(), _points->getDimensionNames());
    setObjectName(QString("%1/Selection").arg(_points->getGuiName()));
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
    _proxyModel->setFilterRegExp(nameFilter);
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

void DimensionsPickerAction::setValueFromVariant(const QVariant& value)
{
    const auto enabledDimensions = value.toList();

    if (enabledDimensions.count() != _holder.getNumberOfDimensions())
        return;

    std::int32_t dimensionIndex = 0;

    for (const auto& enabledDimension : enabledDimensions) {
        _holder.setDimensionEnabled(dimensionIndex, enabledDimension.toBool());
        dimensionIndex++;
    }

    const ModelResetter modelResetter(_proxyModel.get());
}

QVariant DimensionsPickerAction::valueToVariant() const
{
    QVariantList enabledDimensions;

    for (const auto enabledDimension : getEnabledDimensions())
        enabledDimensions << QVariant(enabledDimension);

    return QVariant(enabledDimensions);
}

QVariant DimensionsPickerAction::defaultValueToVariant() const
{
    QVariantList enabledDimensions;

    for (const auto enabledDimension : getEnabledDimensions())
        enabledDimensions << true;

    return QVariant(enabledDimensions);
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
            QTime time;

            time.start();
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
                << " Duration: " << time.elapsed() << " microsecond(s)";

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

    _selectAction.getSelectionThresholdAction().setEnabled(!isReadOnly() && !_holder._statistics.empty());
}

void DimensionsPickerAction::updateSummary()
{
    const auto& holder = _holder;

    const auto numberOfDimensions           = holder.getNumberOfDimensions();
    const auto numberOfVisibleDimensions    = (_proxyModel == nullptr) ? 0 : _proxyModel->rowCount();

    _summaryAction.setString(tr("%1 available, %2 visible, %3 selected").arg(numberOfDimensions).arg(numberOfVisibleDimensions).arg(holder.getNumberOfSelectedDimensions()));
}

DimensionsPickerAction::Widget::Widget(QWidget* parent, DimensionsPickerAction* dimensionSelectionAction, const std::int32_t& widgetFlags) :
    WidgetActionWidget(parent, dimensionSelectionAction)
{
    setMinimumHeight(300);
    /*
    auto layout = new QVBoxLayout();

    auto tableView = new QTableView();

    tableView->setSortingEnabled(true);
    tableView->setModel(&dimensionSelectionAction->getProxyModel());
    tableView->setStyleSheet("QTableView::indicator:checked{ padding: 0px; margin: 0px;}");

    auto horizontalHeader = tableView->horizontalHeader();

    horizontalHeader->setStretchLastSection(false);
    horizontalHeader->setDefaultAlignment(Qt::AlignLeft);
    horizontalHeader->setSortIndicator(2, Qt::DescendingOrder);

    horizontalHeader->resizeSection(1, 85);
    horizontalHeader->resizeSection(2, 85);

    // Set column resize modes
    horizontalHeader->setSectionResizeMode(0, QHeaderView::Stretch);
    horizontalHeader->setSectionResizeMode(1, QHeaderView::Interactive);
    horizontalHeader->setSectionResizeMode(2, QHeaderView::Interactive);

    tableView->verticalHeader()->hide();
    tableView->verticalHeader()->setDefaultSectionSize(5);

    layout->addWidget(tableView);

    auto toolbarLayout = new QHBoxLayout();

    toolbarLayout->addWidget(dimensionSelectionAction->getFilterAction().createCollapsedWidget(this));
    toolbarLayout->addWidget(dimensionSelectionAction->getSelectAction().createCollapsedWidget(this));
    toolbarLayout->addWidget(dimensionSelectionAction->getMiscellaneousAction().createCollapsedWidget(this));
    toolbarLayout->addWidget(dimensionSelectionAction->getSummaryAction().createWidget(this), 1);

    layout->addLayout(toolbarLayout);

    if (widgetFlags & PopupLayout) {
        setPopupLayout(layout);
    }
    else {
        layout->setMargin(0);
        setLayout(layout);
    }
    */
}
