#include "PointSettingsWidget.h"
#include "ScatterplotPlugin.h"

#include "ui_PointSettingsWidget.h"

PointSettingsWidget::PointSettingsWidget(QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _ui{ std::make_unique<Ui::PointSettingsWidget>() }
{
    _ui->setupUi(this);
}

void PointSettingsWidget::initialize(const ScatterplotPlugin& plugin)
{
    auto scatterPlotWidget = const_cast<ScatterplotPlugin&>(plugin).getScatterplotWidget();

    connect(_ui->sizeHorizontalSlider, &QSlider::valueChanged, [this, scatterPlotWidget](int value) {
        const auto pointSize = static_cast<float>(value) / 1000.0f;

        QSignalBlocker spinBoxBlocker(_ui->sizeDoubleSpinBox);

        _ui->sizeDoubleSpinBox->setValue(static_cast<double>(pointSize));

        scatterPlotWidget->setPointSize(pointSize);
    });

    connect(_ui->sizeDoubleSpinBox, qOverload<double>(&QDoubleSpinBox::valueChanged), [this, scatterPlotWidget](double value) {
        const auto pointSize = static_cast<float>(value);

        QSignalBlocker sliderBlocker(_ui->sizeHorizontalSlider);

        _ui->sizeHorizontalSlider->setValue(static_cast<int>(pointSize * 1000.0f));

        scatterPlotWidget->setPointSize(pointSize);
    });

    connect(_ui->opacityHorizontalSlider, &QSlider::valueChanged, [this, scatterPlotWidget](int value) {
        const auto opacity = static_cast<float>(value);

        QSignalBlocker spinBoxBlocker(_ui->opacityDoubleSpinBox);

        _ui->opacityDoubleSpinBox->setValue(static_cast<double>(opacity));

        scatterPlotWidget->setAlpha(0.01f * opacity);
    });

    connect(_ui->opacityDoubleSpinBox, qOverload<double>(&QDoubleSpinBox::valueChanged), [this, scatterPlotWidget](double value) {
        const auto opacity = static_cast<float>(value);

        QSignalBlocker sliderBlocker(_ui->opacityHorizontalSlider);

        _ui->opacityHorizontalSlider->setValue(static_cast<int>(opacity));

        scatterPlotWidget->setAlpha(0.01f * opacity);
    });

    _ui->sizeDoubleSpinBox->setValue(5.0);
    _ui->opacityDoubleSpinBox->setValue(50.0);

    const_cast<ScatterplotPlugin&>(plugin).installEventFilter(this);
}

bool PointSettingsWidget::eventFilter(QObject* target, QEvent* event)
{
    if (event->type() != QEvent::Resize)
        return QWidget::eventFilter(target, event);

    const auto sourceWidgetWidth = static_cast<QResizeEvent*>(event)->size().width();

    auto compact = sourceWidgetWidth <= 1000;

    _ui->popupPushButton->setVisible(compact);
    _ui->sizeLabel->setVisible(!compact);
    _ui->sizeDoubleSpinBox->setVisible(!compact);
    _ui->sizeHorizontalSlider->setVisible(!compact);
    _ui->opacityLabel->setVisible(!compact);
    _ui->opacityDoubleSpinBox->setVisible(!compact);
    _ui->opacityHorizontalSlider->setVisible(!compact);

    return QWidget::eventFilter(target, event);
}