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
    auto scatterPlotWidget = const_cast<ScatterplotPlugin&>(plugin).getScatterplotWidget();

    QObject::connect(_ui->horizontalSlider, &QSlider::sliderReleased, [this, scatterPlotWidget]() {
        const auto sigma = static_cast<float>(_ui->horizontalSlider->value());

        QSignalBlocker spinBoxBlocker(_ui->doubleSpinBox);

        _ui->doubleSpinBox->setValue(static_cast<double>(sigma));

        scatterPlotWidget->setSigma(0.01f * sigma);
    });

    QObject::connect(_ui->doubleSpinBox, qOverload<double>(&QDoubleSpinBox::valueChanged), [this, scatterPlotWidget](double value) {
        const auto sigma = static_cast<float>(value);

        QSignalBlocker sliderBlocker(_ui->horizontalSlider);

        _ui->horizontalSlider->setValue(static_cast<int>(sigma));

        scatterPlotWidget->setSigma(0.01f * sigma);
    });

    QObject::connect(scatterPlotWidget, &ScatterplotWidget::densityComputationStarted, [this]() {
        setEnabled(false);
    });

    QObject::connect(scatterPlotWidget, &ScatterplotWidget::densityComputationEnded, [this]() {
        setEnabled(true);
    });

    setEnabled(false);

    _ui->doubleSpinBox->setValue(30.0);

    const_cast<ScatterplotPlugin&>(plugin).installEventFilter(this);
}

bool DensitySettingsWidget::eventFilter(QObject* target, QEvent* event)
{
    if (event->type() != QEvent::Resize)
        return QWidget::eventFilter(target, event);

    const auto sourceWidgetWidth = static_cast<QResizeEvent*>(event)->size().width();

    auto compact = sourceWidgetWidth <= 1000;

    _ui->popupPushButton->setVisible(compact);
    _ui->label->setVisible(!compact);
    _ui->doubleSpinBox->setVisible(!compact);
    _ui->horizontalSlider->setVisible(!compact);

    return QWidget::eventFilter(target, event);
}