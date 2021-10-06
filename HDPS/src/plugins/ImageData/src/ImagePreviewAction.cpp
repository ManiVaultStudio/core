#include "ImagePreviewAction.h"

#include "util/Exception.h"

using namespace hdps;
using namespace hdps::gui;

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

ImagePreviewAction::ImagePreviewAction(QObject* parent, const QString& datasetName) :
    WidgetAction(parent),
    hdps::EventListener(),
    _images(datasetName),
    _previewImageAction(this, "Preview image"),
    _currentImageNameAction(this, "Current image name"),
    _windowLevelAction(this),
    _previewScalarData(),
    _previewImageSize(),
    _subsamplingFactor(1)
{
    setText("Image preview");
    setEventCore(Application::core());

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
                // Update action states
                updateActions();

                // Get source image size
                const auto imageSize = _images->getImageSize();

                // Compute subsampling factor
                _subsamplingFactor = std::max(1.0, std::min(imageSize.width(), imageSize.height()) / 64.0);

                // Establish effective (sub-sampled) image resolution
                _previewImageSize.setWidth(static_cast<int>(floorf(imageSize.width() / _subsamplingFactor)));
                _previewImageSize.setHeight(static_cast<int>(floorf(imageSize.height() / _subsamplingFactor)));

                // Establish number of preview image pixels
                const auto numberOfPreviewImagePixels = _previewImageSize.width() * _previewImageSize.height();

                // Establish number of required scalars
                const auto numberOfScalars = numberOfPreviewImagePixels * previewImageNoChannels;

                // Allocate preview image scalars
                if (numberOfScalars != _previewScalarData.count())
                    _previewScalarData.resize(numberOfPreviewImagePixels * previewImageNoChannels);

                break;
            }

            default:
                break;
        }
    });

    // Update preview image action with image
    const auto updatePreviewImageAction = [this]() {
        if (_previewScalarData.isEmpty())
            return;

        // Assign image to image action
        _previewImageAction.setImage(getPreviewImage(_currentImageNameAction.getCurrentIndex()));
    };

    // Update preview image action image when selected dimension changes or the window/level settings
    connect(&_currentImageNameAction, &OptionAction::currentIndexChanged, this, updatePreviewImageAction);
    connect(&_windowLevelAction.getWindowAction(), &DecimalAction::valueChanged, this, updatePreviewImageAction);
    connect(&_windowLevelAction.getLevelAction(), &DecimalAction::valueChanged, this, updatePreviewImageAction);
}

float ImagePreviewAction::getAspectRatio() const
{
    const auto imageSize = _images->getImageSize();

    return static_cast<float>(imageSize.height()) / static_cast<float>(imageSize.width());
}

QImage ImagePreviewAction::getPreviewImage(const std::uint32_t& dimensionIndex)
{
    try {

        // Floating point image scalar data
        QVector<float> scalarData32;
        
        scalarData32.resize(_images->getNumberOfPixels());
        
        // Scalar data range
        QPair<float, float> scalarDataRange;

        // Get float scalar data
        _images->getScalarData(dimensionIndex, scalarData32, scalarDataRange, _subsamplingFactor);
        
        std::pair<float, float> displayRange;

        const auto maxWindow            = scalarDataRange.second - scalarDataRange.first;
        const auto windowNormalized     = _windowLevelAction.getWindowAction().getValue();
        const auto levelNormalized      = _windowLevelAction.getLevelAction().getValue();
        const auto level                = std::clamp(scalarDataRange.first + (levelNormalized * maxWindow), scalarDataRange.first, scalarDataRange.second);
        const auto window               = std::clamp(windowNormalized * maxWindow, scalarDataRange.first, scalarDataRange.second);

        displayRange.first      = std::clamp(level - (window / 2.0f), scalarDataRange.first, scalarDataRange.second);
        displayRange.second     = std::clamp(level + (window / 2.0f), scalarDataRange.first, scalarDataRange.second);

        // Compute display range length
        const auto displayRangeLength = displayRange.second - displayRange.first;

        // Establish number of preview image pixels
        const auto numberOfPreviewImagePixels = _previewImageSize.width() * _previewImageSize.height();

        // Compute preview scalar data
        for (std::int32_t pixelIndex = 0; pixelIndex < numberOfPreviewImagePixels; pixelIndex++) {

            // Compute normalized data value and gray scale value
            const auto normalized   = displayRangeLength == 0.0f ? 0.5f : std::clamp((scalarData32[pixelIndex] - displayRange.first) / displayRangeLength, 0.0f, 1.0f);
            const auto grayscale    = static_cast<uchar>(normalized * 255.0f);
        
            // Assign preview data
            for (int channelIndex = 0; channelIndex < previewImageNoChannels; channelIndex++)
                _previewScalarData[pixelIndex * previewImageNoChannels + channelIndex] = grayscale;
        }

        // Create the preview image
        return QImage((uchar*)_previewScalarData.data(), _previewImageSize.width(), _previewImageSize.height(), QImage::Format_RGB32).mirrored(false, true);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to get preview image", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to get preview image");
    }

    return QImage();
}

ImagePreviewAction::Widget::Widget(QWidget* parent, ImagePreviewAction* imagePreviewAction, const std::int32_t& widgetFlags, const WidgetActionWidget::State& state) :
    WidgetActionWidget(parent, imagePreviewAction, state)
{
    //setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto mainLayout     = new QVBoxLayout();
    auto toolbarLayout  = new QHBoxLayout();

    mainLayout->setMargin(0);
    mainLayout->setSpacing(3);

    mainLayout->addWidget(imagePreviewAction->getPreviewImageAction().createWidget(this, ImageAction::Label));
    mainLayout->addLayout(toolbarLayout);

    setLayout(mainLayout);

    toolbarLayout->addWidget(imagePreviewAction->getCurrentImageNameAction().createWidget(this));
    toolbarLayout->addWidget(imagePreviewAction->getWindowLevelAction().createCollapsedWidget(this));
}
