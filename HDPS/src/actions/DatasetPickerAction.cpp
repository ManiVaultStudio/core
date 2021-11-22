#include "DatasetPickerAction.h"
#include "Application.h"

#include "event/Event.h"

#include <QHBoxLayout>

using namespace hdps;

DatasetPickerAction::DatasetPickerAction(QObject* parent) :
    WidgetAction(parent),
    EventListener(),
    _datasets(),
    _currentDatasetAction(this, "Pick dimension")
{
    setText("Dataset picker");
    setIcon(Application::getIconFont("FontAwesome").getIcon("database"));
    setEventCore(Application::core());

    _currentDatasetAction.setToolTip("Pick a dataset");

    connect(&_currentDatasetAction, &OptionAction::currentIndexChanged, this, [this](const std::int32_t& currentIndex) {
        emit datasetPicked(_datasets[currentIndex].get());
    });

    registerDataEvent([this](DataEvent* dataEvent) {

        // Get dataset for which the event occurred
        const auto dataset = dataEvent->getDataset();

        // Only proceed if the dataset resides in the dataset picker
        if (!_datasets.contains(dataset))
            return;

        switch (dataEvent->getType())
        {
            case EventType::DataAboutToBeRemoved:
            {
                _datasets.removeOne(dataset);
                updateCurrentDatasetAction();

                break;
            }

            case EventType::DataGuiNameChanged:
            {
                updateCurrentDatasetAction();

                break;
            }

            default:
                break;
        }
    });
}

void DatasetPickerAction::setDatasets(const QVector<Dataset<DatasetImpl>>& datasets)
{
    // Assign datasets
    _datasets = datasets;

    // And update the options action
    updateCurrentDatasetAction();
}

Dataset<DatasetImpl> DatasetPickerAction::getCurrentDataset()
{
    // Get the current dataset index
    const auto currentIndex = _currentDatasetAction.getCurrentIndex();

    if (currentIndex >= 0)
        return _datasets[currentIndex];

    return Dataset<DatasetImpl>();
}

void DatasetPickerAction::setCurrentDataset(const Dataset<DatasetImpl>& currentDataset)
{
    // Get index of current dataset
    const auto currentIndex = _datasets.indexOf(currentDataset);

    // Only proceed if dataset was found
    if (currentIndex < 0)
        return;

    // Change current index
    _currentDatasetAction.setCurrentIndex(currentIndex);
}

void DatasetPickerAction::updateCurrentDatasetAction()
{
    // Dataset options
    QStringList datasets;

    // Add option for each dataset
    for (auto dataset : _datasets)
        datasets << dataset->getGuiName();

    // Set options in picker action
    _currentDatasetAction.setOptions(datasets);
}

DatasetPickerAction::Widget::Widget(QWidget* parent, DatasetPickerAction* datasetPickerAction) :
    WidgetActionWidget(parent, datasetPickerAction)
{
    auto layout = new QHBoxLayout();

    layout->setMargin(0);
    layout->setSpacing(3);

    auto currentDatasetWidget = datasetPickerAction->getCurrentDatasetAction().createWidget(this);

    currentDatasetWidget->findChild<QComboBox*>("ComboBox")->setSizeAdjustPolicy(QComboBox::AdjustToContents);

    layout->addWidget(currentDatasetWidget);

    setLayout(layout);
}
