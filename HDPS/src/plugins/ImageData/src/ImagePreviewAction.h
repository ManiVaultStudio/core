#pragma once

#include "actions/WidgetAction.h"
#include "actions/ImageAction.h"
#include "actions/OptionAction.h"
#include "actions/WindowLevelAction.h"

#include "event/EventListener.h"
#include "util/DatasetRef.h"

#include "Images.h"

using namespace hdps;
using namespace hdps::gui;
using namespace hdps::util;

/**
 * Preview action class
 *
 * Action class for image channel(s) preview
 *
 * @author Thomas Kroes
 */
class ImagePreviewAction : public WidgetAction, public EventListener
{
    Q_OBJECT

public:

    /** Image preview widget for image preview action */
    class Widget : public WidgetActionWidget
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param imagePreviewAction Pointer to image preview action
         */
        Widget(QWidget* parent, ImagePreviewAction* imagePreviewAction, const std::int32_t& widgetFlags, const WidgetActionWidget::State& state);

    protected:

        friend class ImagePreviewAction;
    };

protected:

    /**
     * Get widget representation of the preview action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     * @param state State of the widget (for stateful widgets)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags, const WidgetActionWidget::State& state = WidgetActionWidget::State::Standard) override {
        return new Widget(parent, this, widgetFlags, state);
    };

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param datasetName Name of the images dataset
     */
    ImagePreviewAction(QObject* parent, const QString& datasetName);

    /** Get aspect ratio */
    float getAspectRatio() const;

    /**
     * Get preview image at dimension index
     * @param dimensionIndex Dimension index
     */
    QImage getPreviewImage(const std::uint32_t& dimensionIndex);

public: // Action getters

    ImageAction& getPreviewImageAction() { return _previewImageAction; }
    OptionAction& getCurrentImageNameAction() { return _currentImageNameAction; }
    WindowLevelAction& getWindowLevelAction() { return _windowLevelAction; }

protected:
    DatasetRef<Images>      _images;                        /** Images dataset reference */
    ImageAction             _previewImageAction;            /** Preview image action */
    OptionAction            _currentImageNameAction;        /** Current image name action */
    WindowLevelAction       _windowLevelAction;             /** Window level action */
    QVector<uchar>          _previewScalarData;             /** Preview scalar data */
    QSize                   _previewImageSize;              /** Effective preview image size */
    std::uint32_t           _subsamplingFactor;             /** Subsampling factor */

    static const std::int32_t previewImageNoChannels    = 4;
};
