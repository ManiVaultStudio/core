#include "PlotAction.h"
#include "Application.h"

#include "ScatterplotWidget.h"

using namespace hdps::gui;

PlotAction::PlotAction(ScatterplotWidget* scatterplotWidget) :
    WidgetAction(scatterplotWidget),
    _scatterplotWidget(scatterplotWidget),
    _pointSizeAction(this, "Point size"),
    _pointOpacityAction(this, "Point opacity"),
    _sigmaAction(this, "Sigma")
{
    _pointSizeAction.setDisabled(true);
    _pointSizeAction.setSuffix("px");
    _pointOpacityAction.setSuffix("%");

    const auto updateRenderMode = [this, scatterplotWidget]() {
        const auto renderMode = scatterplotWidget->getRenderMode();

        _pointSizeAction.setVisible(renderMode == ScatterplotWidget::SCATTERPLOT);
        _pointOpacityAction.setVisible(renderMode == ScatterplotWidget::SCATTERPLOT);
        _sigmaAction.setVisible(renderMode != ScatterplotWidget::SCATTERPLOT);
    };

    connect(scatterplotWidget, &ScatterplotWidget::renderModeChanged, this, [this, updateRenderMode](const ScatterplotWidget::RenderMode& renderMode) {
        updateRenderMode();
    });

    updateRenderMode();
}

QMenu* PlotAction::getContextMenu()
{
    auto menu = new QMenu("Plot");

    const auto renderMode = _scatterplotWidget->getRenderMode();

    switch (renderMode)
    {
        case ScatterplotWidget::RenderMode::SCATTERPLOT:
            menu->addAction(&_pointSizeAction);
            menu->addAction(&_pointOpacityAction);
            break;

        case ScatterplotWidget::RenderMode::DENSITY:
        case ScatterplotWidget::RenderMode::LANDSCAPE:
            menu->addAction(&_sigmaAction);
            break;

        default:
            break;
    }

    return menu;
}

PlotAction::Widget::Widget(QWidget* parent, PlotAction* plotAction) :
    WidgetAction::Widget(parent, plotAction),
    _layout(),
    _toolBar(),
    _toolButton(),
    _popupWidget(this, "Plot"),
    _popupWidgetAction(this)
{
    _layout.addWidget(&_toolBar);

    _toolBar.addAction(&plotAction->_pointSizeAction);
    _toolBar.addAction(&plotAction->_pointOpacityAction);
    _toolBar.addAction(&plotAction->_sigmaAction);

    _popupWidgetAction.setDefaultWidget(&_popupWidget);

    _toolButton.setPopupMode(QToolButton::InstantPopup);
    _toolButton.addAction(&_popupWidgetAction);

    setLayout(&_layout);
}

/*
DensitySettingsWidget::DensitySettingsWidget(const hdps::gui::ResponsiveSectionWidget::WidgetState& widgetState, QWidget* parent) :
ScatterplotSettingsWidget(widgetState, parent),
_label(new QLabel("Sigma:")),
_doubleSpinBox(new QDoubleSpinBox()),
_slider(new QSlider())
{
    _doubleSpinBox->setMinimum(1.0);
    _doubleSpinBox->setMaximum(50.0);
    _doubleSpinBox->setDecimals(1);

    _slider->setOrientation(Qt::Horizontal);
    _slider->setMinimum(1);
    _slider->setMaximum(50);

    auto layout = new QHBoxLayout();

    applyLayout(layout);

    layout->addWidget(_label);
    layout->addWidget(_doubleSpinBox);
    layout->addWidget(_slider);

    _doubleSpinBox->setVisible(!isCompact());
    _slider->setFixedWidth(isCompact() ? 50 : 80);
}

void DensitySettingsWidget::setScatterplotPlugin(ScatterplotPlugin* scatterplotPlugin)
{
    _scatterplotPlugin = scatterplotPlugin;

    auto scatterPlotWidget = _scatterplotPlugin->getScatterplotWidget();

    const auto setTooltip = [this](const float& sigma) {
        const auto toolTip = QString("Sigma: %1").arg(QString::number(sigma, 'f', 1));

        _doubleSpinBox->setToolTip(toolTip);
        _slider->setToolTip(toolTip);
    };

    QObject::connect(_slider, &QSlider::sliderReleased, [this, scatterPlotWidget, setTooltip]() {
        const auto sigma = static_cast<float>(_slider->value());

        QSignalBlocker doubleSpinBoxBlocker(_doubleSpinBox);

        _doubleSpinBox->setValue(static_cast<double>(sigma));

        scatterPlotWidget->setSigma(0.01f * sigma);

        setTooltip(sigma);
    });

    QObject::connect(_doubleSpinBox, qOverload<double>(&QDoubleSpinBox::valueChanged), [this, scatterPlotWidget, setTooltip](double value) {
        const auto sigma = static_cast<float>(value);

        QSignalBlocker sliderBlocker(_slider);

        _slider->setValue(static_cast<int>(sigma));

        scatterPlotWidget->setSigma(0.01f * sigma);

        setTooltip(sigma);
    });

    QObject::connect(scatterPlotWidget, &ScatterplotWidget::densityComputationStarted, this, [this]() {
        setEnabled(false);
    });

    QObject::connect(scatterPlotWidget, &ScatterplotWidget::densityComputationEnded, this, [this]() {
        setEnabled(true);
    });

    setEnabled(false);

    _doubleSpinBox->setValue(30.0);
}
*/

/*
PointSettingsWidget::PointSettingsWidget(const ResponsiveSectionWidget::WidgetState& widgetState, QWidget* parent) :
ScatterplotSettingsWidget(widgetState, parent),
_sizeLabel(new QLabel()),
_sizeDoubleSpinBox(new QDoubleSpinBox()),
_sizeSlider(new QSlider()),
_opacityLabel(new QLabel()),
_opacityDoubleSpinBox(new QDoubleSpinBox()),
_opacitySlider(new QSlider())
{
    _sizeLabel->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    _sizeDoubleSpinBox->setMinimum(1.0);
    _sizeDoubleSpinBox->setMaximum(20.0);
    _sizeDoubleSpinBox->setDecimals(1);
    _sizeDoubleSpinBox->setSuffix("px");

    _sizeSlider->setOrientation(Qt::Horizontal);
    _sizeSlider->setMinimum(1000);
    _sizeSlider->setMaximum(20000);

    _opacityLabel->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    _opacityDoubleSpinBox->setMinimum(1.0);
    _opacityDoubleSpinBox->setMaximum(100.0);
    _opacityDoubleSpinBox->setDecimals(0);
    _opacityDoubleSpinBox->setSuffix("%");

    _opacitySlider->setOrientation(Qt::Horizontal);
    _opacitySlider->setMinimum(1);
    _opacitySlider->setMaximum(100);

    if (isForm()) {
        auto layout = new QGridLayout();

        applyLayout(layout);

        layout->addWidget(_sizeLabel, 0, 0);
        layout->addWidget(_sizeDoubleSpinBox, 0, 1);
        layout->addWidget(_sizeSlider, 0, 2);

        layout->addWidget(_opacityLabel, 1, 0);
        layout->addWidget(_opacityDoubleSpinBox, 1, 1);
        layout->addWidget(_opacitySlider, 1, 2);
    }

    if (isSequential()) {
        auto layout = new QHBoxLayout();

        applyLayout(layout);

        layout->addWidget(_sizeLabel);

        if (!isCompact())
            layout->addWidget(_sizeDoubleSpinBox);

        layout->addWidget(_sizeSlider);

        layout->addWidget(_opacityLabel);

        if (!isCompact())
            layout->addWidget(_opacityDoubleSpinBox);

        layout->addWidget(_opacitySlider);
    }

    _sizeLabel->setText(isFull() ? "Point size:" : "Size:");
    _opacityLabel->setText(isFull() ? "Point opacity:" : "Opacity:");

    _sizeSlider->setFixedWidth(isCompact() ? 50 : 80);
    _opacitySlider->setFixedWidth(isCompact() ? 50 : 80);
}

void PointSettingsWidget::setScatterplotPlugin(ScatterplotPlugin* scatterplotPlugin)
{
    _scatterplotPlugin = scatterplotPlugin;

    const auto setSizeTooltip = [this](const float& pointSize) {
        const auto toolTip = QString("Point size: %1").arg(QString::number(pointSize, 'f', 1));

        _sizeDoubleSpinBox->setToolTip(toolTip);
        _sizeSlider->setToolTip(toolTip);
    };

    const auto setOpacityTooltip = [this](const float& pointOpacity) {
        const auto toolTip = QString("Point opacity: %1\%").arg(QString::number(pointOpacity, 'f', 0));

        _opacityDoubleSpinBox->setToolTip(toolTip);
        _opacitySlider->setToolTip(toolTip);
    };

    auto scatterPlotWidget = _scatterplotPlugin->getScatterplotWidget();

    connect(_sizeSlider, &QSlider::valueChanged, [this, scatterPlotWidget, setSizeTooltip](int value) {
        const auto pointSize = static_cast<float>(value) / 1000.0f;

        QSignalBlocker spinBoxBlocker(_sizeDoubleSpinBox);

        _sizeDoubleSpinBox->setValue(static_cast<double>(pointSize));

        scatterPlotWidget->setPointSize(pointSize);

        setSizeTooltip(pointSize);
    });

    connect(_sizeDoubleSpinBox, qOverload<double>(&QDoubleSpinBox::valueChanged), [this, scatterPlotWidget, setSizeTooltip](double value) {
        const auto pointSize = static_cast<float>(value);

        QSignalBlocker sliderBlocker(_sizeSlider);

        _sizeSlider->setValue(static_cast<int>(pointSize * 1000.0f));

        scatterPlotWidget->setPointSize(pointSize);

        setSizeTooltip(pointSize);
    });

    connect(_opacitySlider, &QSlider::valueChanged, [this, scatterPlotWidget, setOpacityTooltip](int value) {
        const auto opacity = static_cast<float>(value);

        QSignalBlocker spinBoxBlocker(_opacityDoubleSpinBox);

        _opacityDoubleSpinBox->setValue(static_cast<double>(opacity));

        scatterPlotWidget->setAlpha(0.01f * opacity);

        setOpacityTooltip(opacity);
    });

    connect(_opacityDoubleSpinBox, qOverload<double>(&QDoubleSpinBox::valueChanged), [this, scatterPlotWidget, setOpacityTooltip](double value) {
        const auto opacity = static_cast<float>(value);

        QSignalBlocker sliderBlocker(_opacitySlider);

        _opacitySlider->setValue(static_cast<int>(opacity));

        scatterPlotWidget->setAlpha(0.01f * opacity);

        setOpacityTooltip(opacity);
    });

    _sizeDoubleSpinBox->setValue(5.0);
    _opacityDoubleSpinBox->setValue(50.0);
}
*/