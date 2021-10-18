#include "InfoAction.h"
#include "Application.h"

using namespace hdps;
using namespace hdps::gui;

InfoAction::InfoAction(QObject* parent, const QString& datasetName) :
    GroupAction(parent, true),
    EventListener(),
    _images(datasetName),
    _typeAction(this, "Image collection type"),
    _numberOfImagesAction(this, "Number of images"),
    _imageResolutionAction(this, "Image resolution"),
    _sourceRectangleAction(this, "Source rectangle"),
    _targetRectangleAction(this, "Target rectangle"),
    _numberOfPixelsAction(this, "Number of pixels"),
    _numberComponentsPerPixelAction(this, "Number of components per pixel")
{
    setText("Info");
    setEventCore(Application::core());

    _typeAction.setEnabled(false);
    _numberOfImagesAction.setEnabled(false);
    _imageResolutionAction.setEnabled(false);
    _sourceRectangleAction.setEnabled(false);
    _targetRectangleAction.setEnabled(false);
    _numberOfPixelsAction.setEnabled(false);
    _numberComponentsPerPixelAction.setEnabled(false);

    _typeAction.setMayReset(false);
    _numberOfImagesAction.setMayReset(false);
    _imageResolutionAction.setMayReset(false);
    _sourceRectangleAction.setMayReset(false);
    _targetRectangleAction.setMayReset(false);
    _numberOfPixelsAction.setMayReset(false);
    _numberComponentsPerPixelAction.setMayReset(false);

    const auto sizeToString = [](const QSize& size) -> QString {
        return QString("[%1, %2]").arg(QString::number(size.width()), QString::number(size.height()));

    };
    const auto rectangleToString = [](const QRect& rectangle) -> QString {
        return QString("[%1, %2, %3, %4]").arg(QString::number(rectangle.left()), QString::number(rectangle.top()), QString::number(rectangle.width()), QString::number(rectangle.height()));
    };

    const auto updateActions = [this, sizeToString, rectangleToString]() -> void {
        if (!_images.isValid())
            return;

        _typeAction.setString(ImageData::getTypeName(_images->getType()));
        _numberOfImagesAction.setString(QString::number(_images->getNumberOfImages()));

        const auto imageSize = _images->getImageSize();

        _imageResolutionAction.setString(sizeToString(imageSize));
        _sourceRectangleAction.setString(rectangleToString(_images->getSourceRectangle()));
        _targetRectangleAction.setString(rectangleToString(_images->getTargetRectangle()));
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
