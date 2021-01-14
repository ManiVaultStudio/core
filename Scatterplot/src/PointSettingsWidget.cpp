#include "PointSettingsWidget.h"
#include "ScatterplotPlugin.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QSlider>

PointSettingsWidget::PointSettingsWidget(QWidget* parent /*= nullptr*/) :
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

    const auto applyLayout = [this](QLayout* stateLayout) {
        Q_ASSERT(stateLayout != nullptr);

        if (layout())
            delete layout();

        stateLayout->setMargin(0);

        setLayout(stateLayout);
    };

    switch (_state)
    {
        case State::Popup:
        {
            auto stateLayout = new QGridLayout();

            sizeLabelText       = "Size:";
            opacityLabelText    = "Opacity:";

            stateLayout->addWidget(_sizeLabel, 0, 0);
            stateLayout->addWidget(_sizeDoubleSpinBox, 0, 1);
            stateLayout->addWidget(_sizeSlider, 0, 2);

            stateLayout->addWidget(_opacityLabel, 1, 0);
            stateLayout->addWidget(_opacityDoubleSpinBox, 1, 1);
            stateLayout->addWidget(_opacitySlider, 1, 2);

            applyLayout(stateLayout);

            break;
        }

        case State::Compact:
        case State::Full:
        {
            sizeLabelText       = "Point size:";
            opacityLabelText    = "Point opacity:";

            auto stateLayout = new QHBoxLayout();

            stateLayout->addWidget(_sizeLabel);

            if (_state == State::Full)
                stateLayout->addWidget(_sizeDoubleSpinBox);

            stateLayout->addWidget(_sizeSlider);

            stateLayout->addWidget(_opacityLabel);

            if (_state == State::Full)
                stateLayout->addWidget(_opacityDoubleSpinBox);

            stateLayout->addWidget(_opacitySlider);

            applyLayout(stateLayout);

            break;
        }

        default:
            break;
    }

    _sizeLabel->setText(sizeLabelText);
    _opacityLabel->setText(opacityLabelText);
}