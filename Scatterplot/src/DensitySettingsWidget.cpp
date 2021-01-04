#include "DensitySettingsWidget.h"
#include "ScatterplotPlugin.h"

#include "ui_DensitySettingsWidget.h"

DensitySettingsWidget::DensitySettingsWidget(QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _ui{ std::make_unique<Ui::DensitySettingsWidget>() }
{
    _ui->setupUi(this);
}

void DensitySettingsWidget::initialize(const ScatterplotPlugin& plugin)
{
    QObject::connect(_ui->horizontalSlider, &QSlider::sliderReleased, [this, &plugin]() {
        const auto sigma = static_cast<float>(_ui->horizontalSlider->value());

        QSignalBlocker spinBoxBlocker(_ui->doubleSpinBox);

        _ui->doubleSpinBox->setValue(static_cast<double>(sigma));

        plugin._scatterPlotWidget->setSigma(0.01f * sigma);
    });

    QObject::connect(_ui->doubleSpinBox, qOverload<double>(&QDoubleSpinBox::valueChanged), [this, &plugin](double value) {
        const auto sigma = static_cast<float>(value);

        QSignalBlocker sliderBlocker(_ui->horizontalSlider);

        _ui->horizontalSlider->setValue(static_cast<int>(sigma));

        plugin._scatterPlotWidget->setSigma(0.01f * sigma);
    });

    QObject::connect(plugin._scatterPlotWidget, &ScatterplotWidget::densityComputationStarted, [this]() {
        //setEnabled(false);
    });

    QObject::connect(plugin._scatterPlotWidget, &ScatterplotWidget::densityComputationEnded, [this]() {
        //setEnabled(true);
    });

    //setEnabled(false);

    _ui->doubleSpinBox->setValue(30.0);
}