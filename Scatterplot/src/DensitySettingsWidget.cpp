#include "DensitySettingsWidget.h"
#include "ScatterplotPlugin.h"

#include <QLabel>
#include <QDoubleSpinBox>
#include <QSlider>

DensitySettingsWidget::DensitySettingsWidget(QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    WidgetStateMixin("Plot"),
    _label(new QLabel()),
    _doubleSpinBox(new QDoubleSpinBox()),
    _slider(new QSlider())
{
    _label->setText("Sigma:");
    
    _doubleSpinBox->setMinimum(1.0);
    _doubleSpinBox->setMaximum(50.0);
    _doubleSpinBox->setDecimals(1);

    _slider->setOrientation(Qt::Horizontal);
    _slider->setMinimum(1);
    _slider->setMaximum(50);

    const auto toolTipText = "Density sigma";

    _label->setToolTip(toolTipText);
    _doubleSpinBox->setToolTip(toolTipText);
    _slider->setToolTip(toolTipText);
}

void DensitySettingsWidget::initialize(const ScatterplotPlugin& plugin)
{
    auto scatterPlotWidget = const_cast<ScatterplotPlugin&>(plugin).getScatterplotWidget();

    QObject::connect(_slider, &QSlider::sliderReleased, [this, scatterPlotWidget]() {
        const auto sigma = static_cast<float>(_slider->value());

        QSignalBlocker doubleSpinBoxBlocker(_doubleSpinBox);

        _doubleSpinBox->setValue(static_cast<double>(sigma));

        scatterPlotWidget->setSigma(0.01f * sigma);
    });

    QObject::connect(_doubleSpinBox, qOverload<double>(&QDoubleSpinBox::valueChanged), [this, scatterPlotWidget](double value) {
        const auto sigma = static_cast<float>(value);

        QSignalBlocker sliderBlocker(_slider);

        _slider->setValue(static_cast<int>(sigma));

        scatterPlotWidget->setSigma(0.01f * sigma);
    });

    QObject::connect(scatterPlotWidget, &ScatterplotWidget::densityComputationStarted, [this]() {
        setEnabled(false);
    });

    QObject::connect(scatterPlotWidget, &ScatterplotWidget::densityComputationEnded, [this]() {
        setEnabled(true);
    });

    setEnabled(false);

    _doubleSpinBox->setValue(30.0);

    const_cast<ScatterplotPlugin&>(plugin).installEventFilter(this);
}

WidgetStateMixin::State DensitySettingsWidget::getState(const QSize& sourceWidgetSize) const
{
    const auto width = sourceWidgetSize.width();

    auto state = WidgetStateMixin::State::Popup;

    if (width >= 1000 && width < 1500)
        state = WidgetStateMixin::State::Compact;

    if (width >= 1500)
        state = WidgetStateMixin::State::Full;

    return state;
}

void DensitySettingsWidget::updateState()
{
    if (layout())
        delete layout();

    auto stateLayout = new QHBoxLayout();

    setLayout(stateLayout);

    stateLayout->addWidget(_label);
    stateLayout->addWidget(_doubleSpinBox);
    stateLayout->addWidget(_slider);

    _doubleSpinBox->setVisible(_state != WidgetStateMixin::State::Compact);

    _slider->setFixedWidth(_state == WidgetStateMixin::State::Popup ? 100 : 40);

    stateLayout->setMargin(0);
}