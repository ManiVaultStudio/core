#include "InfoAction.h"
#include "Application.h"
#include "event/Event.h"

using namespace hdps;
using namespace hdps::gui;

InfoAction::InfoAction(QObject* parent, Images& images) :
    GroupAction(parent, true),
    _images(&images),
    _typeAction(this, "Image collection type"),
    _numberOfImagesAction(this, "Number of images"),
    _imageResolutionAction(this, "Image resolution"),
    _numberOfPixelsAction(this, "Number of pixels"),
    _numberComponentsPerPixelAction(this, "Number of components per pixel")
{
    setText("Info");

    _typeAction.setEnabled(false);
    _numberOfImagesAction.setEnabled(false);
    _imageResolutionAction.setEnabled(false);
    _numberOfPixelsAction.setEnabled(false);
    _numberComponentsPerPixelAction.setEnabled(false);

    const auto sizeToString = [](const QSize& size) -> QString {
        return QString("[%1, %2]").arg(QString::number(size.width()), QString::number(size.height()));

    };

    const auto updateActions = [this, sizeToString]() -> void {
        if (!_images.isValid())
            return;

        _typeAction.setString(ImageData::getTypeName(_images->getType()));
        _numberOfImagesAction.setString(QString::number(_images->getNumberOfImages()));

        const auto imageSize = _images->getImageSize();

        _imageResolutionAction.setString(sizeToString(imageSize));
        _numberOfPixelsAction.setString(QString::number(_images->getNumberOfPixels()));
        _numberComponentsPerPixelAction.setString(QString::number(_images->getNumberOfComponentsPerPixel()));
    };

    _eventListener.addSupportedEventType(static_cast<std::uint32_t>(EventType::DataAdded));
    _eventListener.addSupportedEventType(static_cast<std::uint32_t>(EventType::DataChanged));
    _eventListener.addSupportedEventType(static_cast<std::uint32_t>(EventType::DataSelectionChanged));
    _eventListener.registerDataEventByType(ImageType, [this, updateActions](hdps::DataEvent* dataEvent) {
        if (!_images.isValid())
            return;

        if (dataEvent->getDataset() != _images)
            return;

        switch (dataEvent->getType()) {
            case EventType::DataAdded:
            case EventType::DataChanged:
            case EventType::DataSelectionChanged:
            {
                updateActions();
                break;
            }

            default:
                break;
        }
    });
}
