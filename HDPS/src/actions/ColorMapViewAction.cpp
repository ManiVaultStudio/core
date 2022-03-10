#include "ColorMapViewAction.h"
#include "ColorMapAction.h"
#include "Application.h"

#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>

using namespace hdps::util;

namespace hdps {

namespace gui {

ColorMapViewAction::ColorMapViewAction(ColorMapAction& colorMapAction) :
    WidgetAction(&colorMapAction),
    _colorMapAction(colorMapAction)
{
    setText("Preview");
}

ColorMapViewAction::Widget::Widget(QWidget* parent, ColorMapViewAction* colorMapViewAction) :
    WidgetActionWidget(parent, colorMapViewAction)
{
    auto layout = new QVBoxLayout();

    auto colorMapPreviewLabel = new QLabel();

    colorMapPreviewLabel->setFixedWidth(100);
    colorMapPreviewLabel->setFrameShape(QFrame::Panel);
    colorMapPreviewLabel->setFrameShadow(QFrame::Sunken);
    colorMapPreviewLabel->setScaledContents(true);
    colorMapPreviewLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    layout->addWidget(colorMapPreviewLabel);
    layout->invalidate();

    colorMapPreviewLabel->adjustSize();

    // Update color map image preview
    const auto updateColorMapImagePreview = [colorMapPreviewLabel, colorMapViewAction]() -> void {
        const auto colorMapImage        = colorMapViewAction->getColorMapAction().getColorMapImage();
        const auto scaledColorMapImage  = colorMapImage.scaled(colorMapPreviewLabel->size(), Qt::IgnoreAspectRatio);

        colorMapPreviewLabel->setPixmap(QPixmap::fromImage(scaledColorMapImage.mirrored(false, true)));
    };

    connect(&colorMapViewAction->getColorMapAction(), &ColorMapAction::imageChanged, this, updateColorMapImagePreview);

    updateColorMapImagePreview();

    setPopupLayout(layout);
}

}
}
