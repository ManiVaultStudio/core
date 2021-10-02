#include "ImagePreviewAction.h"

using namespace hdps;
using namespace hdps::gui;

#include <QVBoxLayout>
#include <QHBoxLayout>

ImagePreviewAction::ImagePreviewAction(QObject* parent, const QString& datasetName) :
    WidgetAction(parent),
    hdps::EventListener(),
    _images(datasetName),
    _currentImageNameAction(this, "Current image name"),
    _currentImageIndexAction(this, "Current image index")
{
    setText("Image preview");
    setEventCore(Application::core());

    // Update current image index when the current image name changes
    connect(&_currentImageNameAction, &OptionAction::currentIndexChanged, this, [this](const std::int32_t& currentIndex) {
        _currentImageIndexAction.setValue(currentIndex);
    });

    // Update current image name when the current image index changes
    connect(&_currentImageIndexAction, &IntegralAction::valueChanged, this, [this](const std::int32_t& value) {
        _currentImageNameAction.setCurrentIndex(value);
    });

    // Update actions when images data changes
    const auto updateActions = [this]() -> void {
        if (!_images.isValid())
            return;

        const auto dimensionNames       = _images->getDimensionNames();
        const auto numberOfDimensions   = dimensionNames.count();

        setEnabled(numberOfDimensions >= 1);

        if (numberOfDimensions <= 0)
            return;

        // Initialize current name and index actions
        _currentImageNameAction.initialize(dimensionNames, dimensionNames.first(), dimensionNames.first());
        _currentImageIndexAction.initialize(0, numberOfDimensions - 1, 0, 0);
    };

    registerDataEventByType(ImageType, [this, updateActions](hdps::DataEvent* dataEvent) {
        if (!_images.isValid())
            return;

        if (dataEvent->dataSetName != _images->getName())
            return;

        switch (dataEvent->getType()) {
            case EventType::DataAdded:
            case EventType::DataChanged:
            {
                updateActions();
                break;
            }

            default:
                break;
        }
    });
}

ImagePreviewAction::Widget::Widget(QWidget* parent, ImagePreviewAction* imagePreviewAction, const std::int32_t& widgetFlags, const WidgetActionWidget::State& state) :
    WidgetActionWidget(parent, imagePreviewAction, state)
{
    auto mainLayout         = new QVBoxLayout();
    auto currentImageLayout = new QHBoxLayout();
    auto previewLabel       = new QLabel();

    mainLayout->addWidget(previewLabel);
    mainLayout->addLayout(currentImageLayout);

    currentImageLayout->addWidget(imagePreviewAction->getCurrentImageNameAction().createWidget(this));
    currentImageLayout->addWidget(imagePreviewAction->getCurrentImageIndexAction().createWidget(this, IntegralAction::Slider));

    setLayout(mainLayout);
}
