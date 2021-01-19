#include "DensitySettingsWidget.h"
#include "ScatterplotPlugin.h"

#include <QLabel>
#include <QDoubleSpinBox>
#include <QSlider>

using namespace hdps::gui;

DensitySettingsWidget::DensitySettingsWidget(QWidget* parent /*= nullptr*/) :
    ResponsiveToolBar::Widget("Plot", 100),
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

    computeStateSizes();
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

void DensitySettingsWidget::updateState()
{
    auto layout = new QHBoxLayout();

    setWidgetLayout(layout);

    layout->addWidget(_label);
    layout->addWidget(_doubleSpinBox);
    layout->addWidget(_slider);

    layout->invalidate();
    layout->activate();

    switch (_state)
    {
        case State::Popup:
            setCurrentIndex(0);
            break;

        case State::Compact:
        case State::Full:
            setCurrentIndex(1);
            break;

        default:
            break;
    }

    _doubleSpinBox->setVisible(_state != State::Compact);
    _slider->setFixedWidth(_state == State::Compact ? 50 : 80);
}