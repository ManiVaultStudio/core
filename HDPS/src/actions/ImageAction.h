#pragma once

#include "WidgetAction.h"

#include <QLabel>
#include <QImage>

namespace hdps {

namespace gui {

/**
 * Image widget action class
 *
 * For image display and storage
 *
 * @author Thomas Kroes
 */
class ImageAction : public WidgetAction
{
    Q_OBJECT

public:

    /** Describes the widget configurations */
    enum WidgetFlag {
        Label = 0x00001     /** Image preview with a label widget */
    };

public:

    /** Label widget class for image action */
    class LabelWidget : public QLabel
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param imageAction Pointer to image action
         */
        LabelWidget(QWidget* parent, ImageAction* imageAction);

        /**
         * Invoked when the label is resized
         * @param resizeEvent Resize event that occurred
         */
        //void resizeEvent(QResizeEvent* resizeEvent) override;

        //QSize minimumSizeHint() const override;
        QSize sizeHint() const override {
            return QSize(100, 100);
        }

        //int heightForWidth(int w) const override;

    protected:
        ImageAction*    _imageAction;       /** Pointer to image action */

        friend class ImageAction;
    };

protected:

    /**
     * Get widget representation of the image action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     * @param state State of the widget (for stateful widgets)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags, const WidgetActionWidget::State& state = WidgetActionWidget::State::Standard) override;;

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    ImageAction(QObject* parent, const QString& title = "");

    /** Get the current image */
    QImage getImage() const;

    /**
     * Set the current image
     * @param image Current image
     */
    void setImage(const QImage& image);

signals:

    /**
     * Signals that the current image changed
     * @param image Current image that changed
     */
    void imageChanged(const QImage& string);

protected:
    QImage     _image;      /** Current image */
};

}
}
