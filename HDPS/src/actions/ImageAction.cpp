#include "ImageAction.h"

#include <QHBoxLayout>
#include <QResizeEvent>
#include <QCoreApplication>

namespace hdps {

namespace gui {

ImageAction::ImageAction(QObject* parent, const QString& title /*= ""*/) :
    WidgetAction(parent, title)
{
    setText(title);
}

QImage ImageAction::getImage() const
{
    return _image;
}

void ImageAction::setImage(const QImage& image)
{
    _image = image;

    emit imageChanged(_image);
}

ImageAction::LabelWidget::LabelWidget(QWidget* parent, ImageAction* imageAction) :
    QLabel("No image loaded...", parent),
    _imageAction(imageAction)
{
    //setStyleSheet("QWidget { background-color: red;}");
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    setAlignment(Qt::AlignCenter);
    setFrameShape(QFrame::Panel);
    setFrameShadow(QFrame::Sunken);
    setScaledContents(true);

    const auto update = [this, imageAction]() {

        // Get image from action
        const auto image = _imageAction->getImage();

        // Only process valid image
        if (image.isNull())
            return;

        // Assign bitmap to label
        setPixmap(QPixmap::fromImage(imageAction->getImage()));

        // Compute fixed size
        const auto aspectRatio  = static_cast<float>(image.height()) / static_cast<float>(image.width());
        const auto fixedSize    = QSize(width(), width() * aspectRatio);

        // Set fixed size
        setFixedSize(fixedSize);

        QCoreApplication::processEvents();

        adjustSize();
        updateGeometry();

        //parentWidget()->layout()->update();
        //parentWidget()->adjustSize();
        //parentWidget()->parentWidget()->adjustSize();
        //parentWidget()->parentWidget()->updateGeometry();
    };

    connect(imageAction, &ImageAction::imageChanged, this, update);

    update();
}

//QSize ImageAction::LabelWidget::minimumSizeHint() const
//{
//    qDebug() << "sizeHint";
//
//    // Get image from action
//    const auto image = _imageAction->getImage();
//
//    if (image.isNull())
//        return QSize(10, 10);
//
//    return QSize(500, 500);
//    return image.size();
//}

//void ImageAction::LabelWidget::resizeEvent(QResizeEvent* resizeEvent)
//{
//    // Get image from action
//    const auto image = _imageAction->getImage();
//
//    if (image.isNull())
//        return;
//
//    // Compute fixed size
//    const auto aspectRatio  = static_cast<float>(image.height()) / static_cast<float>(image.width());
//    const auto fixedSize    = QSize(resizeEvent->size().width(), resizeEvent->size().width() * aspectRatio);
//
//    //setFixedWidth(resizeEvent->size().width());
//    //if (fixedSize != size())
//    setFixedHeight(500);
//
//    //setS
//    adjustSize();
//    //qDebug() << aspectRatio << fixedSize << resizeEvent;
//}

//QSize ImageAction::LabelWidget::sizeHint() const
//{
//    qDebug() << "sizeHint";
//
//    // Get image from action
//    const auto image = _imageAction->getImage();
//
//    if (image.isNull())
//        return QSize(10, 10);
//
//    return QSize(500, 500);
//    return image.size();
//
//    //// Compute fixed size
//    //const auto aspectRatio  = static_cast<float>(image.height()) / static_cast<float>(image.width());
//    //const auto fixedHeight  = size().width() * aspectRatio;
//
//    //return QSize(size().width(), 500);
//}

//int ImageAction::LabelWidget::heightForWidth(int w) const
//{
//    qDebug() << "heightForWidth";
//
//    // Get image from action
//    const auto image = _imageAction->getImage();
//
//    if (image.isNull())
//        return w;
//
//    // Compute fixed size
//    const auto aspectRatio = static_cast<float>(image.height()) / static_cast<float>(image.width());
//    return size().width() * aspectRatio;
//}

QWidget* ImageAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags)
{
    return new ImageAction::LabelWidget(parent, this);

    auto widget = new WidgetActionWidget(parent, this);
    auto layout = new QHBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(3);

    if (widgetFlags & WidgetFlag::Label)
        layout->addWidget(new ImageAction::LabelWidget(parent, this));

    widget->setLayout(layout);

    return widget;
}

}
}
