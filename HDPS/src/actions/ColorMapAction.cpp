#include "ColorMapAction.h"
#include "Application.h"

#include <QHBoxLayout>
#include <QPaintEvent>
#include <QPainter>
#include <QComboBox>
#include <QAbstractItemView>
#include <QItemSelectionModel>

#include <numeric>
#include <math.h>

using namespace hdps::util;

namespace hdps {

namespace gui {

ColorMapAction::ColorMapAction(QObject* parent, const QString& title /*= ""*/, const ColorMap::Type& colorMapType /*= ColorMap::Type::OneDimensional*/, const QString& colorMap /*= "RdYlBu"*/, const QString& defaultColorMap /*= "RdYlBu"*/) :
    WidgetAction(parent),
    _currentColorMapAction(this, "Current color map"),
    _colorMapFilterModel(this, colorMapType),
    _settingsAction(*this)
{
    setText(title);
    setIcon(Application::getIconFont("FontAwesome").getIcon("paint-roller"));
    setDefaultWidgetFlags(WidgetFlag::Default);

    initialize(colorMap, defaultColorMap);

    const auto notifyColorMapImageChanged = [this]() -> void {
        emit imageChanged(getColorMapImage());
        notifyResettable();
    };

    connect(&_currentColorMapAction, &OptionAction::currentIndexChanged, this, notifyColorMapImageChanged);
    connect(&_settingsAction.getHorizontalAxisAction().getMirrorAction(), &ToggleAction::toggled, this, notifyColorMapImageChanged);
    connect(&_settingsAction.getVerticalAxisAction().getMirrorAction(), &ToggleAction::toggled, this, notifyColorMapImageChanged);
    connect(&_settingsAction.getDiscreteAction(), &ToggleAction::toggled, this, notifyColorMapImageChanged);
    connect(&_settingsAction.getDiscreteAction().getNumberOfStepsAction(), &IntegralAction::valueChanged, this, notifyColorMapImageChanged);
    connect(&_currentColorMapAction, &OptionAction::resettableChanged, this, &ColorMapAction::notifyResettable);
    connect(&_settingsAction, &ColorMapSettingsAction::resettableChanged, this, &ColorMapAction::notifyResettable);
}

void ColorMapAction::initialize(const QString& colorMap /*= ""*/, const QString& defaultColorMap /*= ""*/)
{
    _colorMapFilterModel.setSourceModel(ColorMapModel::getGlobalInstance());
    _currentColorMapAction.initialize(_colorMapFilterModel, colorMap, defaultColorMap);

    notifyResettable();
}

hdps::util::ColorMap::Type ColorMapAction::getColorMapType() const
{
    return _colorMapFilterModel.getType();
}

void ColorMapAction::setColorMapType(const util::ColorMap::Type& colorMapType)
{
    _colorMapFilterModel.setType(colorMapType);
    _currentColorMapAction.reset();

    emit typeChanged(colorMapType);

    notifyResettable();
}

QString ColorMapAction::getColorMap() const
{
    return _currentColorMapAction.getCurrentText();
}

QImage ColorMapAction::getColorMapImage() const
{
    if (_currentColorMapAction.getModel() == nullptr)
        return QImage();

    // Get the model index of the selected color map image
    const auto filteredModelIndex = _currentColorMapAction.getModel()->index(_currentColorMapAction.getCurrentIndex(), 0);

    // Get the selected color map image
    auto colorMapImage = filteredModelIndex.siblingAtColumn(static_cast<std::int32_t>(ColorMapModel::Column::Image)).data(Qt::EditRole).value<QImage>();

    // Cast away the const-ness of the this pointer and get the settings action
    auto& settingsAction = const_cast<ColorMapAction*>(this)->getSettingsAction();

    // Establish whether the color map needs to be mirrored in the horizontally and vertical direction
    const auto mirrorHorizontally   = settingsAction.getHorizontalAxisAction().getMirrorAction().isChecked();
    const auto mirrorVertically     = settingsAction.getVerticalAxisAction().getMirrorAction().isChecked();

    // Perform the mirroring
    colorMapImage = colorMapImage.mirrored(mirrorHorizontally, mirrorVertically);

    // Convert to discrete representation
    if (settingsAction.getDiscreteAction().isChecked()) {

        // Get the number of discrete color map steps
        const auto numberOfDiscreteSteps = settingsAction.getDiscreteAction().getNumberOfStepsAction().getValue();

        switch (_colorMapFilterModel.getType())
        {
            case ColorMap::Type::OneDimensional:
            {
                // The discrete one-dimensional color map image
                QImage discreteColorMapImage(numberOfDiscreteSteps, 1, QImage::Format::Format_ARGB32);

                // Compute step size in source image coordinates 
                const auto sourceStepSizeX = static_cast<std::int32_t>(floorf(colorMapImage.width()) / static_cast<float>(numberOfDiscreteSteps));

                // Create average color for each discrete color section
                for (int stepX = 0; stepX < numberOfDiscreteSteps; stepX++) {

                    // Compute source image pixel range
                    const auto rangeStartX  = stepX * sourceStepSizeX;
                    const auto rangeEndR    = rangeStartX + sourceStepSizeX;

                    // Create channel for RGB color components
                    QVector<QVector<float>> channels;

                    channels.resize(3);

                    // Sum the pixel colors so that we can average them later
                    for (int p = rangeStartX; p < rangeEndR; p++) {
                        const auto pixel = colorMapImage.pixelColor(p, 0);

                        channels[0].push_back(pixel.redF());
                        channels[1].push_back(pixel.greenF());
                        channels[2].push_back(pixel.blueF());
                    }

                    // Compute number of pixels
                    const auto noPixels = static_cast<float>(sourceStepSizeX);

                    QColor discreteColor;

                    // Compute average color per channel
                    discreteColor.setRedF(std::accumulate(channels[0].begin(), channels[0].end(), 0.0f) / noPixels);
                    discreteColor.setGreenF(std::accumulate(channels[1].begin(), channels[1].end(), 0.0f) / noPixels);
                    discreteColor.setBlueF(std::accumulate(channels[2].begin(), channels[2].end(), 0.0f) / noPixels);

                    // Assign the pixel color
                    discreteColorMapImage.setPixelColor(stepX, 0, discreteColor);
                }

                return discreteColorMapImage;
            }

            case ColorMap::Type::TwoDimensional:
            {
                // The discrete one-dimensional color map image
                QImage discreteColorMapImage(numberOfDiscreteSteps, numberOfDiscreteSteps, QImage::Format::Format_ARGB32);

                // Compute step size in source image coordinates 
                const auto sourceStepSizeX = static_cast<std::int32_t>(floorf(colorMapImage.width()) / static_cast<float>(numberOfDiscreteSteps));
                const auto sourceStepSizeY = static_cast<std::int32_t>(floorf(colorMapImage.height()) / static_cast<float>(numberOfDiscreteSteps));

                // Create average color for each discrete color section
                for (int stepY = 0; stepY < numberOfDiscreteSteps; stepY++) {
                    for (int stepX = 0; stepX < numberOfDiscreteSteps; stepX++) {
                        
                        // Compute source image pixel ranges in x and y direction
                        const auto rangeStartX  = stepX * sourceStepSizeX;
                        const auto rangeEndX    = rangeStartX + sourceStepSizeX;
                        const auto rangeStartY  = stepY * sourceStepSizeY;
                        const auto rangeEndY    = rangeStartY + sourceStepSizeY;

                        // Create channel for RGB color components
                        QVector<QVector<float>> channels;

                        channels.resize(3);

                        // Sum the pixel colors so that we can average them later
                        for (int pixelY = rangeStartY; pixelY < rangeEndY; pixelY++) {
                            for (int pixelX = rangeStartX; pixelX < rangeEndX; pixelX++) {

                                // Get source image pixel color
                                const auto pixel = colorMapImage.pixelColor(pixelX, pixelY);

                                channels[0].push_back(pixel.redF());
                                channels[1].push_back(pixel.greenF());
                                channels[2].push_back(pixel.blueF());
                            }
                        }
                        
                        // Compute number of pixels
                        const auto noPixels = static_cast<float>(sourceStepSizeX * sourceStepSizeY);

                        QColor discreteColor;

                        // Compute average color per channel
                        discreteColor.setRedF(std::accumulate(channels[0].begin(), channels[0].end(), 0.0f) / noPixels);
                        discreteColor.setGreenF(std::accumulate(channels[1].begin(), channels[1].end(), 0.0f) / noPixels);
                        discreteColor.setBlueF(std::accumulate(channels[2].begin(), channels[2].end(), 0.0f) / noPixels);

                        // Assign the pixel color
                        discreteColorMapImage.setPixelColor(stepX, stepY, discreteColor);
                    }
                }

                return discreteColorMapImage;
            }

            default:
                break;
        }
    }

    // Ensure 24-bit depth image
    return colorMapImage.convertToFormat(QImage::Format_RGB32);
}

void ColorMapAction::setColorMap(const QString& colorMap)
{
    Q_ASSERT(!colorMap.isEmpty());

    _currentColorMapAction.setCurrentText(colorMap);

    notifyResettable();
}

QString ColorMapAction::getDefaultColorMap() const
{
    return _currentColorMapAction.getDefaultText();
}

void ColorMapAction::setDefaultColorMap(const QString& defaultColorMap)
{
    Q_ASSERT(!defaultColorMap.isEmpty());

    _currentColorMapAction.setDefaultText(defaultColorMap);

    notifyResettable();
}

void ColorMapAction::setValueFromVariant(const QVariant& value)
{
}

QVariant ColorMapAction::valueToVariant() const
{
    return QVariant();
}

QVariant ColorMapAction::defaultValueToVariant() const
{
    return QVariant();
}

ColorMapAction::ComboBoxWidget::ComboBoxWidget(QWidget* parent, OptionAction* optionAction, ColorMapAction* colorMapAction) :
    OptionAction::ComboBoxWidget(parent, optionAction),
    _colorMapAction(colorMapAction)
{
    setObjectName("ComboBox");

    // Make text color transparent
    QPalette palette = this->palette();
    palette.setColor(QPalette::Text, QColor(0, 0, 0, 0));
    
    // Assign palette
    setPalette(palette);

    // Paint the control when the color map image changes
    connect(colorMapAction, &ColorMapAction::imageChanged, this, [this](const QImage& colorMapImage) {
        update();
    });

    /*
    const auto updateIconSize = [this, colorMapAction]() -> void {
        setIconSize(colorMapAction->getColorMapType() == ColorMap::Type::OneDimensional ? QSize() : QSize(12, 12));
    };

    // Adjust the size of the icon when the color map type changes (hide the icon in one-dimensional mode)
    connect(colorMapAction, &ColorMapAction::typeChanged, this, [updateIconSize](const util::ColorMap::Type& colorMapType) {
        updateIconSize();
    });
    */

    setIconSize(QSize());
}

void ColorMapAction::ComboBoxWidget::paintEvent(QPaintEvent* paintEvent)
{
    OptionAction::ComboBoxWidget::paintEvent(paintEvent);

    // Draw at a higher resolution to get better anti-aliasing
    const auto pixmapSize = 2 * size();
    const auto pixmapRect = QRect(QPoint(), pixmapSize);

    // Create color pixmap
    QPixmap colorPixmap(pixmapSize);

    // Fill with a transparent background
    colorPixmap.fill(Qt::transparent);

    // Create a painter to draw in the color pixmap
    QPainter painter(&colorPixmap);

    // Enable anti-aliasing
    painter.setRenderHint(QPainter::Antialiasing);

    QStyleOption styleOption;

    styleOption.init(this);
    
    // Set inset margins
    const auto margin = 8;

    // Deflated fill rectangle for color map inset
    const auto colorMapRectangle = pixmapRect.marginsRemoved(QMargins(margin, margin, margin + 28, margin + 1));

    // Get color map image from the model
    auto colorMapImage = _colorMapAction->getColorMapImage();

    // Convert to gray scale if disabled
    if (!isEnabled())
        colorMapImage = colorMapImage.convertToFormat(QImage::Format_Grayscale8);

    // Establish pen color based on whether the color map is enabled or not
    const auto penColor = isEnabled() ? styleOption.palette.color(QPalette::Normal, QPalette::Shadow) : styleOption.palette.color(QPalette::Disabled, QPalette::ButtonText);

    // Get scaled copy of the color map image so that it fits correctly
    colorMapImage = colorMapImage.scaled(colorMapRectangle.size(), Qt::AspectRatioMode::IgnoreAspectRatio);

    // Create a textured brush
    QBrush colorMapPixMapBrush(colorMapImage);

    // And set the texture offset such that it aligns properly
    colorMapPixMapBrush.setTransform(QTransform::fromTranslate(margin, margin));

    // Do the painting
    painter.setPen(QPen(penColor, 1.5, Qt::SolidLine, Qt::SquareCap, Qt::SvgMiterJoin));
    painter.setBrush(colorMapPixMapBrush);
    painter.drawRoundedRect(colorMapRectangle, 4, 4);

    QPainter painterColorWidget(this);

    // Draw the color map over the control
    painterColorWidget.drawPixmap(rect(), colorPixmap, pixmapRect);
}

QWidget* ColorMapAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags)
{
    auto widget = new WidgetActionWidget(parent, this);
    auto layout = new QHBoxLayout();

    layout->setMargin(0);
    //layout->setSpacing(3);

    auto comboBoxWidget = new ComboBoxWidget(parent, &_currentColorMapAction, this);

    layout->addWidget(comboBoxWidget);

    if (widgetFlags & WidgetFlag::Settings)
        layout->addWidget(_settingsAction.createCollapsedWidget(widget));

    widget->setLayout(layout);

    connect(comboBoxWidget, qOverload<int>(&QComboBox::currentIndexChanged), this, [widget](int index) -> void {
        widget->update();
    });

    comboBoxWidget->setStyleSheet("QComboBox QAbstractItemView { min-width: 250px; }");

    return widget;
}

}
}
