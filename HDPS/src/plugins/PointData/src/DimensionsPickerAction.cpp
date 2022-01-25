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
    _selectionHolder(),
    _selectionItemModel(new DimensionsPickerItemModel(_selectionHolder)),
    _selectionProxyModel(new DimensionsPickerProxyModel(_selectionHolder)),
    _summaryAction(this, "Summary"),
    _filterAction(*this),
    _selectAction(*this),
    _miscellaneousAction(*this),
    _summaryUpdateAwakeConnection()
{
    setText(title);
    setIcon(Application::getIconFont("FontAwesome").getIcon("columns"));

    _summaryAction.setEnabled(false);

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
    computeStatistics();
}

DimensionsPickerAction::~DimensionsPickerAction()
{
    disconnect(_summaryUpdateAwakeConnection);
}

void DimensionsPickerAction::setDimensions(const std::uint32_t numDimensions, const std::vector<QString>& names)
{
    if (names.size() == numDimensions)
    {
        _selectionHolder = DimensionsPickerHolder(names.data(), numDimensions);
    }
    else
    {
        assert(names.empty());
        _selectionHolder = DimensionsPickerHolder(numDimensions);
    }

    auto dimensionSelectionItemModel = std::make_unique<DimensionsPickerItemModel>(_selectionHolder);
    auto proxyModel = std::make_unique<DimensionsPickerProxyModel>(_selectionHolder);

    proxyModel->setSourceModel(dimensionSelectionItemModel.get());
    
    _selectionProxyModel    = std::move(proxyModel);
    _selectionItemModel     = std::move(dimensionSelectionItemModel);

    updateSlider();
}

std::vector<bool> DimensionsPickerAction::getEnabledDimensions() const
{
    return _selectionHolder.getEnabledDimensions();
}

void DimensionsPickerAction::setPointsDataset(const Dataset<Points>& points)
{
    _points = points;

    setDimensions(_points->getNumDimensions(), _points->getDimensionNames());
    computeStatistics();
}

DimensionsPickerHolder& DimensionsPickerAction::getHolder()
{
    return _selectionHolder;
}

hdps::DimensionsPickerProxyModel& DimensionsPickerAction::getProxyModel()
{
    return *_selectionProxyModel;
}

void DimensionsPickerAction::setNameFilter(const QString& nameFilter)
{
    if (_selectionProxyModel == nullptr)
        return;

    const ModelResetter modelResetter(_selectionProxyModel.get());
    _selectionProxyModel->setFilterRegExp(nameFilter);
}

void DimensionsPickerAction::setShowOnlySelectedDimensions(const bool& showOnlySelectedDimensions)
{
    if (_selectionProxyModel == nullptr)
        return;

    const ModelResetter modelResetter(_selectionProxyModel.get());
    _selectionProxyModel->SetFilterShouldAcceptOnlySelected(showOnlySelectedDimensions);
}

void DimensionsPickerAction::setApplyExclusionList(const bool& applyExclusionList)
{
    if (_selectionProxyModel == nullptr)
        return;

    const ModelResetter modelResetter(_selectionProxyModel.get());
    _selectionProxyModel->SetFilterShouldAppyExclusion(applyExclusionList);
}

void DimensionsPickerAction::setIgnoreZeroValues(const bool& ignoreZeroValues)
{
    const ModelResetter modelResetter(_selectionProxyModel.get());
    _selectionHolder._ignoreZeroValues = ignoreZeroValues;
    
    updateSlider();
}

void DimensionsPickerAction::loadSelectionFromFile(const QString& fileName)
{
    QFile file(fileName);

    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        const ModelResetter modelResetter(_selectionItemModel.get());

        _selectionHolder.disableAllDimensions();

        while (!file.atEnd())
        {
            const auto trimmedLine = file.readLine().trimmed();

            if (!trimmedLine.isEmpty())
            {
                const auto name = QString::fromUtf8(trimmedLine);

                if (!_selectionHolder.tryToEnableDimensionByName(name))
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
        const ModelResetter modelResetter(_selectionProxyModel.get());

        std::deque<QString> strings;

        while (!file.atEnd())
        {
            const auto trimmedLine = file.readLine().trimmed();

            if (!trimmedLine.isEmpty())
                strings.push_back(QString::fromUtf8(trimmedLine));
        }

        _selectionProxyModel->SetExclusion(std::vector<QString>(strings.cbegin(), strings.cend()));
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
        const auto numberOfDimensions = _selectionHolder.getNumberOfDimensions();

        for (std::size_t i{}; i < numberOfDimensions; ++i)
        {
            if (_selectionHolder.isDimensionEnabled(i))
            {
                file.write(_selectionHolder.getName(i).toUtf8());
                file.write("\n");
            }
        }
    }
    else
    {
        qCritical() << "Save failed to open file: " << fileName;
    }
}

void DimensionsPickerAction::computeStatistics()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    {
        const ModelResetter modelResetter(_selectionProxyModel.get());

        auto& statistics = _selectionHolder._statistics;
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

                for (const auto& statisticsPerDimension : _selectionHolder._statistics)
                {
                    if (!std::isnan(statisticsPerDimension.standardDeviation[i]))
                    {
                        distinctStandardDeviations.insert(statisticsPerDimension.standardDeviation[i]);
                    }
                }

                _selectionHolder.distinctStandardDeviationsWithAndWithoutZero[i].assign(distinctStandardDeviations.cbegin(), distinctStandardDeviations.cend());
            }

            assert(_selectAction.getSelectionThresholdAction().getMinimum() == 0);
            updateSlider();
        }
    }
    QApplication::restoreOverrideCursor();
}

void DimensionsPickerAction::updateSlider()
{
    const auto numberOfDistinctStandardDeviations   = _selectionHolder.distinctStandardDeviationsWithAndWithoutZero[_selectionHolder._ignoreZeroValues ? 1 : 0].size();
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

    _selectAction.getSelectionThresholdAction().setEnabled(!isReadOnly() && !_selectionHolder._statistics.empty());
}

void DimensionsPickerAction::updateSummary()
{
    const auto& holder = _selectionHolder;

    const auto numberOfDimensions           = holder.getNumberOfDimensions();
    const auto numberOfVisibleDimensions    = (_selectionProxyModel == nullptr) ? 0 : _selectionProxyModel->rowCount();

    _summaryAction.setString(tr("%1 available, %2 visible, %3 selected").arg(numberOfDimensions).arg(numberOfVisibleDimensions).arg(holder.getNumberOfSelectedDimensions()));
}

DimensionsPickerAction::Widget::Widget(QWidget* parent, DimensionsPickerAction* dimensionSelectionAction, const std::int32_t& widgetFlags) :
    WidgetActionWidget(parent, dimensionSelectionAction)
{
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
}
