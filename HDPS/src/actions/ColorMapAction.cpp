#include "ColorMapAction.h"
#include "Application.h"
#include "CoreInterface.h"

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

ColorMapAction::ColorMapAction(QObject* parent, const QString& title, const ColorMap::Type& colorMapType /*= ColorMap::Type::OneDimensional*/, const QString& colorMap /*= "RdYlBu"*/) :
    WidgetAction(parent, title),
    _colorMapFilterModel(this, colorMapType),
    _currentColorMapAction(this, "Current color map"),
    _rangeAction{ DecimalRangeAction(this, "Range"), DecimalRangeAction(this, "Range (y)") },
    _dataRangeAction{ DecimalRangeAction(this, "Data range"), DecimalRangeAction(this, "Data range (y)") },
    _sharedDataRangeAction{ DecimalRangeAction(this, "Shared data range"), DecimalRangeAction(this, "Shared data range (y)") },
    _lockToSharedDataRangeAction(this, "Lock to shared data range"),
    _mirrorAction{ ToggleAction(this, "Mirror horizontally"), ToggleAction(this, "Mirror vertically") },
    _mirrorGroupAction(this, "Mirror"),
    _discretizeAction(this, "Discrete"),
    _numberOfDiscreteStepsAction(this, "Number of steps", 2, 10, 5, 5),
    _discretizeAlphaAction(this, "Discretize alpha", false, false),
    _settings1DAction(this, "Settings 1D"),
    _settings2DAction(this, "Settings 2D"),
    _customColorMapAction(this, "Custom color map"),
    _editor1DAction(this, "Editor 1D"),
    _customColorMapGroupAction(this, "Custom color map"),
    _settingsAction(*this, "Settings")
{
    setIcon(Application::getIconFont("FontAwesome").getIcon("paint-roller"));
    setDefaultWidgetFlags(WidgetFlag::Default);

    initialize(colorMap);

    const auto notifyColorMapImageChanged = [this]() -> void {
        emit imageChanged(getColorMapImage());
    };

    connect(&getCurrentColorMapAction(), &OptionAction::currentIndexChanged, this, notifyColorMapImageChanged);
    connect(&getMirrorAction(Axis::X), &ToggleAction::toggled, this, notifyColorMapImageChanged);
    connect(&getMirrorAction(Axis::Y), &ToggleAction::toggled, this, notifyColorMapImageChanged);
    connect(&getDiscretizeAction(), &ToggleAction::toggled, this, notifyColorMapImageChanged);
    connect(&getNumberOfDiscreteStepsAction(), &IntegralAction::valueChanged, this, notifyColorMapImageChanged);
    connect(&getDiscretizeAlphaAction(), &ToggleAction::toggled, this, notifyColorMapImageChanged);
    connect(&getCustomColorMapAction(), &ToggleAction::toggled, this, notifyColorMapImageChanged);

    const auto updateDiscretizationActions = [this]() -> void {
        getNumberOfDiscreteStepsAction().setEnabled(_discretizeAction.isChecked());
        getDiscretizeAlphaAction().setEnabled(_discretizeAction.isChecked());
    };

    connect(&getDiscretizeAction(), &ToggleAction::toggled, this, updateDiscretizationActions);

    const auto updateEditorActionReadOnly = [this]() -> void {
        getEditor1DAction().setEnabled(getCustomColorMapAction().isChecked());
    };

    connect(&getCustomColorMapAction(), &ToggleAction::toggled, this, updateEditorActionReadOnly);

    getEditor1DAction().setConfigurationFlag(WidgetAction::ConfigurationFlag::NoLabelInGroup);

    getCustomColorMapGroupAction().setConfigurationFlag(WidgetAction::ConfigurationFlag::NoLabelInGroup);
    getCustomColorMapGroupAction().addAction(&getCustomColorMapAction());
    getCustomColorMapGroupAction().addAction(&getEditor1DAction());

    getMirrorGroupAction().setConfigurationFlag(WidgetAction::ConfigurationFlag::NoLabelInGroup);
    getMirrorGroupAction().addAction(&getMirrorAction(Axis::X));
    getMirrorGroupAction().addAction(&getMirrorAction(Axis::Y));

    const auto updateSharedDataRange = [this]() -> void {
        NumericalRange<float> sharedDataRangeX(std::numeric_limits<float>::max(), std::numeric_limits<float>::lowest());

        for (auto connectedAction : getConnectedActions()) {
            auto privateColorMapAction = dynamic_cast<ColorMapAction*>(connectedAction);

            sharedDataRangeX += privateColorMapAction->getDataRangeAction(Axis::X).getRange();
        }

        getSharedDataRangeAction(Axis::X).setRange(sharedDataRangeX);
    };

    connect(this, &ColorMapAction::actionConnected, this, [this, updateSharedDataRange](const WidgetAction* privateAction) -> void {
        if (!isPublic())
            return;

        auto privateColorMapAction = dynamic_cast<const ColorMapAction*>(privateAction);

        if (privateColorMapAction == nullptr)
            return;

        connect(&privateColorMapAction->getDataRangeAction(Axis::X), &DecimalRangeAction::rangeChanged, this, updateSharedDataRange);
        connect(&privateColorMapAction->getDataRangeAction(Axis::Y), &DecimalRangeAction::rangeChanged, this, updateSharedDataRange);

        updateSharedDataRange();
    });

    connect(this, &ColorMapAction::actionDisconnected, this, [this, updateSharedDataRange](const WidgetAction* privateAction) -> void {
        if (!isPublic())
            return;

        auto privateColorMapAction = dynamic_cast<const ColorMapAction*>(privateAction);

        if (privateColorMapAction == nullptr)
            return;

        disconnect(&privateColorMapAction->getDataRangeAction(Axis::X), &DecimalRangeAction::rangeChanged, this, nullptr);
        disconnect(&privateColorMapAction->getDataRangeAction(Axis::Y), &DecimalRangeAction::rangeChanged, this, nullptr);

        updateSharedDataRange();
    });

    const auto syncRangeWithDataRange = [this]() -> void {
        const auto dataRange = getDataRangeAction(Axis::X).getRange();
        getRangeAction(Axis::X).initialize(dataRange, dataRange);
    };

    const auto syncRangeWithSharedDataRange = [this]() -> void {
        const auto sharedDataRange = getSharedDataRangeAction(Axis::X).getRange();
        getRangeAction(Axis::X).initialize(sharedDataRange, sharedDataRange);
    };

    const auto updateRangeActionReadOnly = [this]() -> void {
        getRangeAction(Axis::X).setEnabled(!getLockToSharedDataRangeAction().isChecked());
    };

    connect(&getLockToSharedDataRangeAction(), &ToggleAction::toggled, this, [updateRangeActionReadOnly, syncRangeWithDataRange, syncRangeWithSharedDataRange](bool toggled) -> void {
        updateRangeActionReadOnly();

        if (toggled)
            syncRangeWithSharedDataRange();
        else
            syncRangeWithDataRange();
    });

    connect(&getSharedDataRangeAction(Axis::X), &DecimalRangeAction::rangeChanged, this, [this](const NumericalRange<float>& range) -> void {
        if (!getLockToSharedDataRangeAction().isChecked())
            return;

        getRangeAction(Axis::X).initialize(range, range);
    });

    connect(&getSharedDataRangeAction(Axis::Y), &DecimalRangeAction::rangeChanged, this, [this](const NumericalRange<float>& range) -> void {
        if (!getLockToSharedDataRangeAction().isChecked())
            return;

        getRangeAction(Axis::Y).initialize(range, range);
    });

    updateDiscretizationActions();
    updateEditorActionReadOnly();
    updateRangeActionReadOnly;
}

void ColorMapAction::initialize(const QString& colorMap)
{
    _colorMapFilterModel.setSourceModel(ColorMapModel::getGlobalInstance());

    getCurrentColorMapAction().setToolTip("Current");
    getRangeAction(Axis::X).setToolTip("Range in the x-axis");
    getRangeAction(Axis::Y).setToolTip("Range in the y-axis");
    getDataRangeAction(Axis::X).setToolTip("Data range in the x-axis");
    getDataRangeAction(Axis::Y).setToolTip("Data range in the y-axis");
    getSharedDataRangeAction(Axis::X).setToolTip("Shared data range in the x-axis");
    getSharedDataRangeAction(Axis::Y).setToolTip("Shared data range in the y-axis");
    getMirrorAction(Axis::X).setToolTip("Mirror in the x-axis");
    getMirrorAction(Axis::Y).setToolTip("Mirror in the y-axis");
    getDiscretizeAction().setToolTip("Whether to discretize the color map");
    getNumberOfDiscreteStepsAction().setToolTip("Number of discrete steps");
    getDiscretizeAlphaAction().setToolTip("Whether to discrete the alpha channel");
    getCustomColorMapAction().setToolTip("Customize the color map");

    getCurrentColorMapAction().initialize(_colorMapFilterModel, colorMap);

    getDataRangeAction(Axis::X).setEnabled(false);
    getDataRangeAction(Axis::Y).setEnabled(false);

    getDataRangeAction(Axis::X).setDefaultWidgetFlags(DecimalRangeAction::MinimumLineEdit | DecimalRangeAction::MaximumLineEdit);
    getDataRangeAction(Axis::Y).setDefaultWidgetFlags(DecimalRangeAction::MinimumLineEdit | DecimalRangeAction::MaximumLineEdit);

    getSharedDataRangeAction(Axis::X).setEnabled(false);
    getSharedDataRangeAction(Axis::Y).setEnabled(false);

    getSharedDataRangeAction(Axis::X).setDefaultWidgetFlags(DecimalRangeAction::MinimumLineEdit | DecimalRangeAction::MaximumLineEdit);
    getSharedDataRangeAction(Axis::Y).setDefaultWidgetFlags(DecimalRangeAction::MinimumLineEdit | DecimalRangeAction::MaximumLineEdit);

    getEditor1DAction().setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);
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
    if (getCurrentColorMapAction().getModel() == nullptr)
        return QImage();

    const auto filteredModelIndex = getCurrentColorMapAction().getModel()->index(getCurrentColorMapAction().getCurrentIndex(), 0);

    auto colorMapImage = filteredModelIndex.siblingAtColumn(static_cast<std::int32_t>(ColorMapModel::Column::Image)).data(Qt::EditRole).value<QImage>();

    colorMapImage = colorMapImage.convertToFormat(QImage::Format_ARGB32);

    if (getCustomColorMapAction().isChecked())
        colorMapImage = getEditor1DAction().getColorMapImage();

    const auto mirrorHorizontally   = getMirrorAction(Axis::X).isChecked();
    const auto mirrorVertically     = getMirrorAction(Axis::Y).isChecked();

    colorMapImage = colorMapImage.mirrored(mirrorHorizontally, mirrorVertically);

    if (getDiscretizeAction().isChecked()) {

        const auto numberOfDiscreteSteps = getNumberOfDiscreteStepsAction().getValue();

        switch (_colorMapFilterModel.getType())
        {
            case ColorMap::Type::OneDimensional:
            {
                QImage discreteColorMapImage(numberOfDiscreteSteps, 1, QImage::Format::Format_ARGB32);

                const auto sourceStepSizeX = static_cast<std::int32_t>(ceilf(colorMapImage.width()) / static_cast<float>(numberOfDiscreteSteps));

                for (int discreteStepIndex = 0; discreteStepIndex < numberOfDiscreteSteps; discreteStepIndex++) {
                    const auto rangeStartX  = discreteStepIndex * sourceStepSizeX;
                    const auto rangeEndR    = rangeStartX + sourceStepSizeX;

                    QVector<QVector<float>> channels(4);

                    for (int p = rangeStartX; p < rangeEndR; p++) {
                        const auto pixel = colorMapImage.pixelColor(p, 0);

                        channels[0].push_back(pixel.redF());
                        channels[1].push_back(pixel.greenF());
                        channels[2].push_back(pixel.blueF());
                        channels[3].push_back(pixel.alphaF());
                    }

                    const auto noPixels = static_cast<float>(sourceStepSizeX);

                    QColor discreteColor;

                    discreteColor.setRedF(std::accumulate(channels[0].begin(), channels[0].end(), 0.0f) / noPixels);
                    discreteColor.setGreenF(std::accumulate(channels[1].begin(), channels[1].end(), 0.0f) / noPixels);
                    discreteColor.setBlueF(std::accumulate(channels[2].begin(), channels[2].end(), 0.0f) / noPixels);
                    discreteColor.setAlphaF(std::accumulate(channels[3].begin(), channels[3].end(), 0.0f) / noPixels);

                    discreteColorMapImage.setPixelColor(discreteStepIndex, 0, discreteColor);
                }

                for (int pixelX = 0; pixelX < colorMapImage.width(); pixelX++) {
                    auto pixelColor = discreteColorMapImage.pixelColor(QPoint(std::min(numberOfDiscreteSteps - 1, static_cast<std::int32_t>(floorf(static_cast<float>(pixelX) / sourceStepSizeX))), 0));

                    if (!_discretizeAlphaAction.isChecked())
                        pixelColor.setAlphaF(colorMapImage.pixelColor(QPoint(pixelX, 0)).alphaF());

                    colorMapImage.setPixelColor(QPoint(pixelX, 0), pixelColor);
                }

                return colorMapImage;
            }

            case ColorMap::Type::TwoDimensional:
            {
                QImage discreteColorMapImage(numberOfDiscreteSteps, numberOfDiscreteSteps, QImage::Format::Format_ARGB32);

                const auto sourceStepSizeX = static_cast<std::int32_t>(floorf(colorMapImage.width()) / static_cast<float>(numberOfDiscreteSteps));
                const auto sourceStepSizeY = static_cast<std::int32_t>(floorf(colorMapImage.height()) / static_cast<float>(numberOfDiscreteSteps));

                for (int stepY = 0; stepY < numberOfDiscreteSteps; stepY++) {
                    for (int stepX = 0; stepX < numberOfDiscreteSteps; stepX++) {
                        const auto rangeStartX  = stepX * sourceStepSizeX;
                        const auto rangeEndX    = rangeStartX + sourceStepSizeX;
                        const auto rangeStartY  = stepY * sourceStepSizeY;
                        const auto rangeEndY    = rangeStartY + sourceStepSizeY;

                        QVector<QVector<float>> channels(4);

                        for (int pixelY = rangeStartY; pixelY < rangeEndY; pixelY++) {
                            for (int pixelX = rangeStartX; pixelX < rangeEndX; pixelX++) {
                                const auto pixel = colorMapImage.pixelColor(pixelX, pixelY);

                                channels[0].push_back(pixel.redF());
                                channels[1].push_back(pixel.greenF());
                                channels[2].push_back(pixel.blueF());
                                channels[3].push_back(pixel.alphaF());
                            }
                        }
                        
                        const auto noPixels = static_cast<float>(sourceStepSizeX * sourceStepSizeY);

                        QColor discreteColor;

                        discreteColor.setRedF(std::accumulate(channels[0].begin(), channels[0].end(), 0.0f) / noPixels);
                        discreteColor.setGreenF(std::accumulate(channels[1].begin(), channels[1].end(), 0.0f) / noPixels);
                        discreteColor.setBlueF(std::accumulate(channels[2].begin(), channels[2].end(), 0.0f) / noPixels);
                        discreteColor.setAlphaF(std::accumulate(channels[3].begin(), channels[3].end(), 0.0f) / noPixels);

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

void ColorMapAction::connectToPublicAction(WidgetAction* publicAction, bool recursive)
{
    auto publicColorMapAction = dynamic_cast<ColorMapAction*>(publicAction);

    Q_ASSERT(publicColorMapAction != nullptr);

    if (publicColorMapAction == nullptr)
        return;

    if (recursive)
        actions().connectPrivateActionToPublicAction(&_currentColorMapAction, &publicColorMapAction->getCurrentColorMapAction(), recursive);
    
    connect(&publicColorMapAction->getSharedDataRangeAction(Axis::X), &DecimalRangeAction::rangeChanged, this, [this](const util::NumericalRange<float>& range) -> void {
        getSharedDataRangeAction(Axis::X).setRange(range);
    });

    connect(&publicColorMapAction->getSharedDataRangeAction(Axis::Y), &DecimalRangeAction::rangeChanged, this, [this](const util::NumericalRange<float>& range) -> void {
        getSharedDataRangeAction(Axis::Y).setRange(range);
    });

    if (recursive) {
        actions().connectPrivateActionToPublicAction(&getMirrorAction(Axis::X), &publicColorMapAction->getMirrorAction(Axis::X), recursive);
        actions().connectPrivateActionToPublicAction(&getMirrorAction(Axis::Y), &publicColorMapAction->getMirrorAction(Axis::Y), recursive);
        actions().connectPrivateActionToPublicAction(&_discretizeAction, &publicColorMapAction->getDiscretizeAction(), recursive);
        actions().connectPrivateActionToPublicAction(&_numberOfDiscreteStepsAction, &publicColorMapAction->getNumberOfDiscreteStepsAction(), recursive);
        actions().connectPrivateActionToPublicAction(&_discretizeAlphaAction, &publicColorMapAction->getDiscretizeAlphaAction(), recursive);
        actions().connectPrivateActionToPublicAction(&_customColorMapAction, &publicColorMapAction->getCustomColorMapAction(), recursive);
    }

    WidgetAction::connectToPublicAction(publicAction, recursive);
}

void ColorMapAction::disconnectFromPublicAction(bool recursive)
{
    if (!isConnected())
        return;

    auto publicColorMapAction = dynamic_cast<ColorMapAction*>(getPublicAction());

    Q_ASSERT(publicColorMapAction != nullptr);

    if (publicColorMapAction == nullptr)
        return;
    
    disconnect(&publicColorMapAction->getSharedDataRangeAction(Axis::X), &DecimalRangeAction::rangeChanged, this, nullptr);
    disconnect(&publicColorMapAction->getSharedDataRangeAction(Axis::Y), &DecimalRangeAction::rangeChanged, this, nullptr);

    if (recursive) {
        actions().disconnectPrivateActionFromPublicAction(&_currentColorMapAction, recursive);
        actions().disconnectPrivateActionFromPublicAction(&getMirrorAction(Axis::X), recursive);
        actions().disconnectPrivateActionFromPublicAction(&getMirrorAction(Axis::Y), recursive);
        actions().disconnectPrivateActionFromPublicAction(&_discretizeAction, recursive);
        actions().disconnectPrivateActionFromPublicAction(&_numberOfDiscreteStepsAction, recursive);
        actions().disconnectPrivateActionFromPublicAction(&_discretizeAlphaAction, recursive);
        actions().disconnectPrivateActionFromPublicAction(&_customColorMapAction, recursive);
    }

    WidgetAction::disconnectFromPublicAction(recursive);
}

void ColorMapAction::fromVariantMap(const QVariantMap& variantMap)
{
    WidgetAction::fromVariantMap(variantMap);

    variantMapMustContain(variantMap, "ColorMapType");

    setColorMapType(static_cast<ColorMap::Type>(variantMap["ColorMapType"].toInt()));

    _currentColorMapAction.fromParentVariantMap(variantMap);
    getRangeAction(Axis::X).fromParentVariantMap(variantMap);
    getRangeAction(Axis::Y).fromParentVariantMap(variantMap);
    getSharedDataRangeAction(Axis::X).fromParentVariantMap(variantMap);
    getSharedDataRangeAction(Axis::Y).fromParentVariantMap(variantMap);
    getLockToSharedDataRangeAction().fromParentVariantMap(variantMap);
    getMirrorAction(Axis::X).fromParentVariantMap(variantMap);
    getMirrorAction(Axis::Y).fromParentVariantMap(variantMap);
    _discretizeAction.fromParentVariantMap(variantMap);
    _numberOfDiscreteStepsAction.fromParentVariantMap(variantMap);
    _discretizeAlphaAction.fromParentVariantMap(variantMap);
    _customColorMapAction.fromParentVariantMap(variantMap);
}

QVariantMap ColorMapAction::toVariantMap() const
{
    QVariantMap variantMap = WidgetAction::toVariantMap();

    variantMap.insert({
        { "ColorMapType", static_cast<std::int32_t>(getColorMapType()) }
    });

    _currentColorMapAction.insertIntoVariantMap(variantMap);
    getRangeAction(Axis::X).insertIntoVariantMap(variantMap);
    getRangeAction(Axis::Y).insertIntoVariantMap(variantMap);
    getSharedDataRangeAction(Axis::X).insertIntoVariantMap(variantMap);
    getSharedDataRangeAction(Axis::Y).insertIntoVariantMap(variantMap);
    getLockToSharedDataRangeAction().insertIntoVariantMap(variantMap);
    getMirrorAction(Axis::X).insertIntoVariantMap(variantMap);
    getMirrorAction(Axis::Y).insertIntoVariantMap(variantMap);
    _discretizeAction.insertIntoVariantMap(variantMap);
    _numberOfDiscreteStepsAction.insertIntoVariantMap(variantMap);
    _discretizeAlphaAction.insertIntoVariantMap(variantMap);
    _customColorMapAction.insertIntoVariantMap(variantMap);

    return variantMap;
}

ColorMapAction::ComboBoxWidget::ComboBoxWidget(QWidget* parent, OptionAction* optionAction, ColorMapAction* colorMapAction) :
    OptionAction::ComboBoxWidget(parent, optionAction),
    _colorMapAction(colorMapAction)
{
    setObjectName("ComboBox");

    QPalette palette = this->palette();

    palette.setColor(QPalette::Text, QColor(0, 0, 0, 0));
    
    setPalette(palette);

    connect(colorMapAction, &ColorMapAction::imageChanged, this, [this](const QImage& colorMapImage) {
        update();
    });

    const auto updateStyle = [this, colorMapAction]() -> void {
        if (colorMapAction->getEditor1DAction().isChecked()) {
            style()->unpolish(this);
            setEnabled(false);
        }
        else {
            style()->polish(this);
            setEnabled(true);
        }
    };

    connect(&colorMapAction->getEditor1DAction(), &ColorMapEditor1DAction::toggled, this, updateStyle);

    updateStyle();
}

void ColorMapAction::ComboBoxWidget::paintEvent(QPaintEvent* paintEvent)
{
    OptionAction::ComboBoxWidget::paintEvent(paintEvent);

    const auto pixmapSize = 2 * size();
    const auto pixmapRect = QRect(QPoint(), pixmapSize);

    QPixmap colorPixmap(pixmapSize);

    colorPixmap.fill(Qt::transparent);

    QPainter painter(&colorPixmap);

    painter.setRenderHint(QPainter::Antialiasing);

    QStyleOption styleOption;

    styleOption.initFrom(this);

    const auto margin = 8;

    const auto colorMapRectangle = pixmapRect.marginsRemoved(QMargins(margin, margin, margin + (_colorMapAction->getEditor1DAction().isChecked() ? 2 : 31), margin + 1));

    auto colorMapImage = _colorMapAction->getColorMapImage();

    if (!_colorMapAction->getEditor1DAction().isChecked() && !isEnabled())
        colorMapImage = colorMapImage.convertToFormat(QImage::Format_Grayscale8);

    const auto penColor = isEnabled() ? styleOption.palette.color(QPalette::Normal, QPalette::Shadow) : styleOption.palette.color(QPalette::Disabled, QPalette::ButtonText);

    colorMapImage = colorMapImage.scaled(colorMapRectangle.size(), Qt::AspectRatioMode::IgnoreAspectRatio).mirrored(false, true);

    QBrush colorMapPixMapBrush(colorMapImage);

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

    QBrush checkerBoardBrush(Qt::black);

    checkerBoardBrush.setTransform(QTransform::fromTranslate(margin, margin));
    checkerBoardBrush.setTexture(QPixmap::fromImage(checkerBoardImage));

    painter.setBrush(checkerBoardBrush);
    painter.drawRoundedRect(colorMapRectangle, 4, 4);
    
    painter.setPen(QPen(penColor, 1.5, Qt::SolidLine, Qt::SquareCap, Qt::SvgMiterJoin));
    painter.setBrush(colorMapPixMapBrush);
    painter.drawRoundedRect(colorMapRectangle, 4, 4);

    QPainter painterColorWidget(this);

    painterColorWidget.drawPixmap(rect(), colorPixmap, pixmapRect);
}

QWidget* ColorMapAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags)
{
    auto widget = new WidgetActionWidget(parent, this);
    auto layout = new QHBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

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
