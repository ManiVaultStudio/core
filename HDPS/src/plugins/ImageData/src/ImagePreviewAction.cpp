#include "ImagePreviewAction.h"

#include "util/Exception.h"

using namespace hdps;
using namespace hdps::gui;

#include <QVBoxLayout>
#include <QHBoxLayout>

ImagePreviewAction::ImagePreviewAction(QObject* parent, const QString& datasetName) :
    WidgetAction(parent),
    hdps::EventListener(),
    _images(datasetName),
    _currentImageNameAction(this, "Current image name"),
    _currentImageIndexAction(this, "Current image index"),
    _previewScalarData()
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
                // Update action states
                updateActions();

                // Establish number of required scalars
                const auto numberOfScalars = _images->getNumberOfPixels() * previewImageNoChannels;

                // Allocate preview image scalars
                if (numberOfScalars != _previewScalarData.count())
                    _previewScalarData.resize(_images->getNumberOfPixels() * previewImageNoChannels);

                break;
            }

            default:
                break;
        }
    });
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
        _images->getScalarData(dimensionIndex, scalarData32, scalarDataRange);
        

        // Compute scalar data range length
        const auto scalarDataRangeLength = scalarDataRange.second - scalarDataRange.first;

        for (std::uint32_t pixelIndex = 0; pixelIndex < _images->getNumberOfPixels(); pixelIndex++) {

            // Compute normalized data value and gray scale value
            const auto normalized   = (scalarData32[pixelIndex] - scalarDataRange.first) / scalarDataRangeLength;
            const auto grayscale    = static_cast<unsigned char>(normalized * 255.0f);

            // Assign preview data
            for (int channelIndex = 0; channelIndex < previewImageNoChannels; channelIndex++)
                _previewScalarData[pixelIndex * previewImageNoChannels + channelIndex] = grayscale;
        }

        // Get the image size
        const auto imageSize = _images->getImageSize();

        // Create the preview image
        return QImage((uchar*)_previewScalarData.data(), imageSize.width(), imageSize.height(), QImage::Format_RGB32).mirrored(false, true);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to get preview image", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to get preview image");
    }
}

ImagePreviewAction::Widget::Widget(QWidget* parent, ImagePreviewAction* imagePreviewAction, const std::int32_t& widgetFlags, const WidgetActionWidget::State& state) :
    WidgetActionWidget(parent, imagePreviewAction, state)
{
    //setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);

    auto mainLayout         = new QVBoxLayout();
    auto currentImageLayout = new QHBoxLayout();
    auto previewLabel       = new QLabel();

    mainLayout->setMargin(0);

    previewLabel->setFrameShape(QFrame::Panel);
    previewLabel->setFrameShadow(QFrame::Sunken);
    //previewLabel->setScaledContents(true);
    //previewLabel->setFixedSize(200, 200);
    //previewLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    

    mainLayout->addWidget(previewLabel);
    mainLayout->addLayout(currentImageLayout);

    currentImageLayout->addWidget(imagePreviewAction->getCurrentImageNameAction().createWidget(this));
    currentImageLayout->addWidget(imagePreviewAction->getCurrentImageIndexAction().createWidget(this, IntegralAction::Slider));

    setLayout(mainLayout);

    // Update preview image label
    connect(&imagePreviewAction->getCurrentImageNameAction(), &OptionAction::currentIndexChanged, this, [previewLabel, imagePreviewAction](const std::int32_t& currentIndex) {

        // Get scaled copy to fit in the label
        const auto previewImage = imagePreviewAction->getPreviewImage(currentIndex).scaledToWidth(previewLabel->width(), Qt::SmoothTransformation);

        // Configure preview label
        previewLabel->setPixmap(QPixmap::fromImage(previewImage));
        previewLabel->setScaledContents(true);
        previewLabel->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
        //previewLabel->setFixedHeight(previewImage.height());
    });
}
