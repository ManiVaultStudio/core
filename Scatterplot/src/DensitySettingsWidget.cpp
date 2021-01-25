#include "DensitySettingsWidget.h"
#include "ScatterplotPlugin.h"

#include "widgets/ResponsiveToolBar.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QSlider>

using namespace hdps::gui;

DensitySettingsWidget::DensitySettingsWidget(QWidget* parent /*= nullptr*/) :
    ResponsiveToolBar::StatefulWidget(parent, "Density"),
    _label(new QLabel("Sigma")),
    _doubleSpinBox(new QDoubleSpinBox()),
    _slider(new QSlider())
{
    initializeUI();
}

void DensitySettingsWidget::initializeUI()
{
    _doubleSpinBox->setMinimum(1.0);
    _doubleSpinBox->setMaximum(50.0);
    _doubleSpinBox->setDecimals(1);

    _slider->setOrientation(Qt::Horizontal);
    _slider->setMinimum(1);
    _slider->setMaximum(50);
}

void DensitySettingsWidget::setScatterPlotPlugin(const ScatterplotPlugin& plugin)
{
    auto scatterPlotWidget = const_cast<ScatterplotPlugin&>(plugin).getScatterplotWidget();

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

QLayout* DensitySettingsWidget::getLayout(const ResponsiveToolBar::WidgetState& state)
{
    auto layout = new QHBoxLayout();

    layout->addWidget(_label);
    layout->addWidget(_doubleSpinBox);
    layout->addWidget(_slider);

    _doubleSpinBox->setVisible(state != ResponsiveToolBar::WidgetState::Compact);
    _slider->setFixedWidth(state == ResponsiveToolBar::WidgetState::Compact ? 50 : 80);

    return layout;
}