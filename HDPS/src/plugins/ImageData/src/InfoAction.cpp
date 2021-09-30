#include "InfoAction.h"

using namespace hdps;
using namespace hdps::gui;

InfoAction::InfoAction(QObject* parent, CoreInterface* core, const QString& datasetName) :
    GroupAction(parent, false),
    EventListener(),
    _core(core),
    _images(datasetName),
    _typeAction(this, "Image collection type"),
    _numberOfImagesAction(this, "Number of images"),
    _imageWidthAction(this, "Image width"),
    _imageHeightAction(this, "Image height"),
    _numberOfPixelsAction(this, "Number of pixels"),
    _numberComponentsPerPixelAction(this, "Number of components per pixel")
{
    setText("Info");
    setEventCore(_core);

    _typeAction.setEnabled(false);
    _numberOfImagesAction.setEnabled(false);
    _imageWidthAction.setEnabled(false);
    _imageHeightAction.setEnabled(false);
    _numberOfPixelsAction.setEnabled(false);
    _numberComponentsPerPixelAction.setEnabled(false);

    _typeAction.setMayReset(false);
    _numberOfImagesAction.setMayReset(false);
    _imageWidthAction.setMayReset(false);
    _imageHeightAction.setMayReset(false);
    _numberOfPixelsAction.setMayReset(false);
    _numberComponentsPerPixelAction.setMayReset(false);

    const auto updateActions = [this]() -> void {
        if (!_images.isValid())
            return;

        _typeAction.setString(ImageData::getTypeName(_images->getType()));
        _numberOfImagesAction.setString(QString::number(_images->getNumberOfImages()));

        const auto imageSize = _images->getImageSize();

        _imageWidthAction.setString(QString::number(imageSize.width()));
        _imageHeightAction.setString(QString::number(imageSize.height()));
        _numberOfPixelsAction.setString(QString::number(_images->getNumberOfPixels()));
        _numberComponentsPerPixelAction.setString(QString::number(_images->getNumberOfComponentsPerPixel()));
    };

    registerDataEventByType(ImageType, [this, updateActions](hdps::DataEvent* dataEvent) {
        if (!_images.isValid())
            return;

        if (dataEvent->dataSetName != _images->getName())
            return;

        switch (dataEvent->getType()) {
            case EventType::DataAdded:
            case EventType::DataChanged:
            case EventType::SelectionChanged:
            {
                updateActions();
                break;
            }

            default:
                break;
        }
    });
}
