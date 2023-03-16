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

namespace hdps::gui {

ColorMapAction::ColorMapAction(QObject* parent, const QString& title /*= ""*/, const ColorMap::Type& colorMapType /*= ColorMap::Type::OneDimensional*/, const QString& colorMap /*= "RdYlBu"*/, const QString& defaultColorMap /*= "RdYlBu"*/) :
    WidgetAction(parent),
    _colorMapFilterModel(this, colorMapType),
    _currentColorMapAction(this, "Current color map"),
    _rangeAction{ DecimalRangeAction(this, "Range (x)"), DecimalRangeAction(this, "Range (y)") },
    _dataRangeAction{ DecimalRangeAction(this, "Data range (x)"), DecimalRangeAction(this, "Data range (y)") },
    _sharedDataRangeAction{ DecimalRangeAction(this, "Shared data range (x)"), DecimalRangeAction(this, "Shared data range (y)") },
    _synchronizeWithLocalDataRange(this, "Local data range"),
    _synchronizeWithSharedDataRange(this, "Synchronize with shared data range"),
    _mirrorAction{ ToggleAction(this, "Mirror horizontally"), ToggleAction(this, "Mirror vertically") },
    _discretizeAction(this, "Discrete"),
    _numberOfDiscreteStepsAction(this, "Number of steps", 2, 10, 5, 5),
    _discretizeAlphaAction(this, "Discretize alpha", false, false),
    _settingsOneDimensionalAction(*this),
    _settingsTwoDimensionalAction(*this),
    _editorOneDimensionalAction(*this),
    _settingsAction(*this)
{
    setText(title);
    setIcon(Application::getIconFont("FontAwesome").getIcon("paint-roller"));
    setDefaultWidgetFlags(WidgetFlag::Default);
    setSerializationName("ColorMap");

    _currentColorMapAction.setSerializationName("Current");

    initialize(colorMap, defaultColorMap);

    const auto notifyColorMapImageChanged = [this]() -> void {
        emit imageChanged(getColorMapImage());
    };

    connect(&_currentColorMapAction, &OptionAction::currentIndexChanged, this, notifyColorMapImageChanged);
    connect(&getMirrorAction(Qt::Horizontal), &ToggleAction::toggled, this, notifyColorMapImageChanged);
    connect(&getMirrorAction(Qt::Vertical), &ToggleAction::toggled, this, notifyColorMapImageChanged);
    connect(&_discretizeAction, &ToggleAction::toggled, this, notifyColorMapImageChanged);
    connect(&_numberOfDiscreteStepsAction, &IntegralAction::valueChanged, this, notifyColorMapImageChanged);
    connect(&_discretizeAlphaAction, &ToggleAction::toggled, this, notifyColorMapImageChanged);


    _numberOfDiscreteStepsAction.setSerializationName("NumberOfDiscreteSteps");
    _discretizeAlphaAction.setSerializationName("DiscretizeAlpha");

    _numberOfDiscreteStepsAction.setToolTip("Number of discrete steps");
    _discretizeAlphaAction.setToolTip("Whether to discrete the alpha channel");
}

QString ColorMapAction::getTypeString() const
{
    return "ColorMap";
}

void ColorMapAction::initialize(const QString& colorMap /*= ""*/, const QString& defaultColorMap /*= ""*/)
{
    _colorMapFilterModel.setSourceModel(ColorMapModel::getGlobalInstance());
    _currentColorMapAction.initialize(_colorMapFilterModel, colorMap, defaultColorMap);
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
    emit imageChanged(getColorMapImage());
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

    if (settingsAction.getEditor1DAction().isChecked())
        colorMapImage = settingsAction.getEditor1DAction().getColorMapImage();

    // Establish whether the color map needs to be mirrored in the horizontal and vertical direction
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
                const auto sourceStepSizeX = static_cast<std::int32_t>(ceilf(colorMapImage.width()) / static_cast<float>(numberOfDiscreteSteps));

                // Create average color for each discrete color section
                for (int discreteStepIndex = 0; discreteStepIndex < numberOfDiscreteSteps; discreteStepIndex++) {

                    // Compute source image pixel range
                    const auto rangeStartX  = discreteStepIndex * sourceStepSizeX;
                    const auto rangeEndR    = rangeStartX + sourceStepSizeX;

                    // Create channel for RGB color components
                    QVector<QVector<float>> channels(4);

                    // Sum the pixel colors so that we can average them later
                    for (int p = rangeStartX; p < rangeEndR; p++) {
                        const auto pixel = colorMapImage.pixelColor(p, 0);

                        channels[0].push_back(pixel.redF());
                        channels[1].push_back(pixel.greenF());
                        channels[2].push_back(pixel.blueF());
                        channels[3].push_back(pixel.alphaF());
                    }

                    // Compute number of pixels
                    const auto noPixels = static_cast<float>(sourceStepSizeX);

                    QColor discreteColor;

                    // Compute average color per channel
                    discreteColor.setRedF(std::accumulate(channels[0].begin(), channels[0].end(), 0.0f) / noPixels);
                    discreteColor.setGreenF(std::accumulate(channels[1].begin(), channels[1].end(), 0.0f) / noPixels);
                    discreteColor.setBlueF(std::accumulate(channels[2].begin(), channels[2].end(), 0.0f) / noPixels);
                    discreteColor.setAlphaF(std::accumulate(channels[3].begin(), channels[3].end(), 0.0f) / noPixels);

                    // Assign the pixel color
                    discreteColorMapImage.setPixelColor(discreteStepIndex, 0, discreteColor);
                }

                for (int pixelX = 0; pixelX < colorMapImage.width(); pixelX++) {
                    auto pixelColor = discreteColorMapImage.pixelColor(QPoint(std::min(numberOfDiscreteSteps - 1, static_cast<std::int32_t>(floorf(static_cast<float>(pixelX) / sourceStepSizeX))), 0));

                    if (!settingsAction.getDiscreteAction().getDiscretizeAlphaAction().isChecked())
                        pixelColor.setAlphaF(colorMapImage.pixelColor(QPoint(pixelX, 0)).alphaF());

                    colorMapImage.setPixelColor(QPoint(pixelX, 0), pixelColor);
                }

                return colorMapImage;
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
                        QVector<QVector<float>> channels(4);

                        // Sum the pixel colors so that we can average them later
                        for (int pixelY = rangeStartY; pixelY < rangeEndY; pixelY++) {
                            for (int pixelX = rangeStartX; pixelX < rangeEndX; pixelX++) {

                                // Get source image pixel color
                                const auto pixel = colorMapImage.pixelColor(pixelX, pixelY);

                                channels[0].push_back(pixel.redF());
                                channels[1].push_back(pixel.greenF());
                                channels[2].push_back(pixel.blueF());
                                channels[3].push_back(pixel.alphaF());
                            }
                        }
                        
                        // Compute number of pixels
                        const auto noPixels = static_cast<float>(sourceStepSizeX * sourceStepSizeY);

                        QColor discreteColor;

                        // Compute average color per channel
                        discreteColor.setRedF(std::accumulate(channels[0].begin(), channels[0].end(), 0.0f) / noPixels);
                        discreteColor.setGreenF(std::accumulate(channels[1].begin(), channels[1].end(), 0.0f) / noPixels);
                        discreteColor.setBlueF(std::accumulate(channels[2].begin(), channels[2].end(), 0.0f) / noPixels);
                        discreteColor.setAlphaF(std::accumulate(channels[3].begin(), channels[3].end(), 0.0f) / noPixels);

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

    return colorMapImage.convertToFormat(QImage::Format_ARGB32);
}

void ColorMapAction::setColorMap(const QString& colorMap)
{
    Q_ASSERT(!colorMap.isEmpty());

    _currentColorMapAction.setCurrentText(colorMap);
}

QString ColorMapAction::getDefaultColorMap() const
{
    return _currentColorMapAction.getDefaultText();
}

void ColorMapAction::setDefaultColorMap(const QString& defaultColorMap)
{
    Q_ASSERT(!defaultColorMap.isEmpty());

    _currentColorMapAction.setDefaultText(defaultColorMap);
}

void ColorMapAction::connectToPublicAction(WidgetAction* publicAction)
{
    auto publicColorMapAction = dynamic_cast<ColorMapAction*>(publicAction);

    Q_ASSERT(publicColorMapAction != nullptr);

    if (publicColorMapAction == nullptr)
        return;

    _currentColorMapAction.connectToPublicAction(&publicColorMapAction->getCurrentColorMapAction());

    _settingsAction.connectToPublicAction(&publicColorMapAction->getSettingsAction());

    WidgetAction::connectToPublicAction(publicAction);

    /*
    auto publicColorMapDiscreteAction = dynamic_cast<ColorMapDiscreteAction*>(publicAction);

    Q_ASSERT(publicColorMapDiscreteAction != nullptr);

    _numberOfStepsAction.connectToPublicAction(&publicColorMapDiscreteAction->getNumberOfStepsAction());
    _discretizeAlphaAction.connectToPublicAction(&publicColorMapDiscreteAction->getDiscretizeAlphaAction());

    connect(this, &WidgetAction::toggled, publicColorMapDiscreteAction, &WidgetAction::setChecked);
    connect(publicColorMapDiscreteAction, &WidgetAction::toggled, this, &WidgetAction::setChecked);

    setChecked(publicColorMapDiscreteAction->isChecked());

    WidgetAction::connectToPublicAction(publicAction);
    */
}

void ColorMapAction::disconnectFromPublicAction()
{
    _currentColorMapAction.disconnectFromPublicAction();

    _settingsAction.disconnectFromPublicAction();

    WidgetAction::disconnectFromPublicAction();

    /*
    _numberOfStepsAction.disconnectFromPublicAction();
    _discretizeAlphaAction.disconnectFromPublicAction();

    WidgetAction::disconnectFromPublicAction();
    */
}

WidgetAction* ColorMapAction::getPublicCopy() const
{
    return new ColorMapAction(parent(), text(), _colorMapFilterModel.getType(), _currentColorMapAction.getCurrentText(), _currentColorMapAction.getDefaultText());
}

void ColorMapAction::fromVariantMap(const QVariantMap& variantMap)
{
    WidgetAction::fromVariantMap(variantMap);

    _currentColorMapAction.fromParentVariantMap(variantMap);
    _settingsAction.fromParentVariantMap(variantMap);

    _numberOfDiscreteStepsAction.fromParentVariantMap(variantMap);
    _discretizeAlphaAction.fromParentVariantMap(variantMap);
}

QVariantMap ColorMapAction::toVariantMap() const
{
    QVariantMap variantMap = WidgetAction::toVariantMap();

    _currentColorMapAction.insertIntoVariantMap(variantMap);
    _settingsAction.insertIntoVariantMap(variantMap);

    _numberOfDiscreteStepsAction.insertIntoVariantMap(variantMap);
    _discretizeAlphaAction.insertIntoVariantMap(variantMap);

    return variantMap;
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

    const auto updateStyle = [this, colorMapAction]() -> void {
        if (colorMapAction->getSettingsAction().getEditor1DAction().isChecked()) {
            style()->unpolish(this);
            setEnabled(false);
        }
        else {
            style()->polish(this);
            setEnabled(true);
        }
    };

    connect(&colorMapAction->getSettingsAction().getEditor1DAction(), &ColorMapEditor1DAction::toggled, this, updateStyle);

    updateStyle();
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

    styleOption.initFrom(this);

    // Set inset margins
    const auto margin = 8;

    // Deflated fill rectangle for color map inset
    const auto colorMapRectangle = pixmapRect.marginsRemoved(QMargins(margin, margin, margin + (_colorMapAction->getSettingsAction().getEditor1DAction().isChecked() ? 2 : 31), margin + 1));

    // Get color map image from the model
    auto colorMapImage = _colorMapAction->getColorMapImage();

    // Convert to gray scale if disabled
    if (!_colorMapAction->getSettingsAction().getEditor1DAction().isChecked() && !isEnabled())
        colorMapImage = colorMapImage.convertToFormat(QImage::Format_Grayscale8);

    // Establish pen color based on whether the color map is enabled or not
    const auto penColor = isEnabled() ? styleOption.palette.color(QPalette::Normal, QPalette::Shadow) : styleOption.palette.color(QPalette::Disabled, QPalette::ButtonText);

    // Get scaled copy of the color map image so that it fits correctly
    colorMapImage = colorMapImage.scaled(colorMapRectangle.size(), Qt::AspectRatioMode::IgnoreAspectRatio).mirrored(false, true);

    // Create a textured brush
    QBrush colorMapPixMapBrush(colorMapImage);

    // And set the texture offset such that it aligns properly
    colorMapPixMapBrush.setTransform(QTransform::fromTranslate(margin, margin));

    QSize checkerBoardSize(22, 22);

    QImage checkerBoardImage(checkerBoardSize, QImage::Format::Format_ARGB32);

    QPainter checkerBoardPainter(&checkerBoardImage);

    checkerBoardPainter.setPen(Qt::NoPen);
    checkerBoardPainter.setBrush(QColor(100, 100, 100));
    checkerBoardPainter.drawRect(QRect(QPoint(0, 0), QSize(22, 22)));
    checkerBoardPainter.setBrush(QColor(200, 200, 200));
    checkerBoardPainter.drawRect(QRect(QPoint(0, 0), QSize(11, 11)));
    checkerBoardPainter.drawRect(QRect(QPoint(11, 11), QSize(11, 11)));

    // Create a checkerboard brush
    QBrush checkerBoardBrush(Qt::black);

    checkerBoardBrush.setTransform(QTransform::fromTranslate(margin, margin));
    checkerBoardBrush.setTexture(QPixmap::fromImage(checkerBoardImage));

    painter.setBrush(checkerBoardBrush);
    painter.drawRoundedRect(colorMapRectangle, 4, 4);
    
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

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(3);

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
