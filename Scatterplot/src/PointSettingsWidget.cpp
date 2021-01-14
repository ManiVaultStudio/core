#include "PointSettingsWidget.h"
#include "ScatterplotPlugin.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QSlider>

PointSettingsWidget::PointSettingsWidget(const WidgetStateMixin::State& state, QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    WidgetStateMixin("Point settings"),
    _sizeLabel(new QLabel()),
    _sizeDoubleSpinBox(new QDoubleSpinBox()),
    _sizeSlider(new QSlider()),
    _opacityLabel(new QLabel()),
    _opacityDoubleSpinBox(new QDoubleSpinBox()),
    _opacitySlider(new QSlider())
{
    _sizeSlider->setOrientation(Qt::Horizontal);
    _opacitySlider->setOrientation(Qt::Horizontal);

    setState(state);
}

void PointSettingsWidget::initialize(const ScatterplotPlugin& plugin)
{
    /*
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
    */
}

void PointSettingsWidget::updateState()
{
    QString sizeLabelText, opacityLabelText;

    switch (_state)
    {
        case State::Popup:
        {
            auto layout = new QGridLayout();

            sizeLabelText = "Size:";
            opacityLabelText = "Opacity:";

            layout->addWidget(_sizeLabel, 0, 0);
            layout->addWidget(_sizeDoubleSpinBox, 0, 1);
            layout->addWidget(_sizeSlider, 0, 2);

            layout->addWidget(_opacityLabel, 1, 0);
            layout->addWidget(_opacityDoubleSpinBox, 1, 1);
            layout->addWidget(_opacitySlider, 1, 2);

            setLayout(layout);
            break;
        }

        case State::Compact:
        case State::Full:
        {
            sizeLabelText       = "Point size:";
            opacityLabelText    = "Point opacity:";

            auto layout = new QHBoxLayout();

            layout->addWidget(_sizeLabel);

            if (_state == State::Full)
                layout->addWidget(_sizeDoubleSpinBox);

            layout->addWidget(_sizeSlider);

            layout->addWidget(_opacityLabel);

            if (_state == State::Full)
                layout->addWidget(_opacityDoubleSpinBox);

            layout->addWidget(_opacitySlider);

            setLayout(layout);
            break;
        }

        default:
            break;
    }

    _sizeLabel->setText(sizeLabelText);
    _opacityLabel->setText(opacityLabelText);

    layout()->setMargin(0);
    layout()->setSpacing(3);
}