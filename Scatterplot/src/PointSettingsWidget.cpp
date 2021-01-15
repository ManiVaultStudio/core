#include "PointSettingsWidget.h"
#include "ScatterplotPlugin.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QSlider>

PointSettingsWidget::PointSettingsWidget(QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    WidgetStateMixin("Plot"),
    _sizeLabel(new QLabel()),
    _sizeDoubleSpinBox(new QDoubleSpinBox()),
    _sizeSlider(new QSlider()),
    _opacityLabel(new QLabel()),
    _opacityDoubleSpinBox(new QDoubleSpinBox()),
    _opacitySlider(new QSlider())
{
    _sizeDoubleSpinBox->setMinimum(1.0);
    _sizeDoubleSpinBox->setMaximum(20.0);
    _sizeDoubleSpinBox->setDecimals(1);
    _sizeDoubleSpinBox->setSuffix("px");

    _sizeSlider->setOrientation(Qt::Horizontal);
    _sizeSlider->setMinimum(1000);
    _sizeSlider->setMaximum(20000);

    _opacityDoubleSpinBox->setMinimum(1.0);
    _opacityDoubleSpinBox->setMaximum(100.0);
    _opacityDoubleSpinBox->setDecimals(1);
    _opacityDoubleSpinBox->setSuffix("%");

    _opacitySlider->setOrientation(Qt::Horizontal);
    _opacitySlider->setMinimum(1);
    _opacitySlider->setMaximum(100);

    const auto sizeToolTipText = "Point size";

    _sizeLabel->setToolTip(sizeToolTipText);
    _sizeDoubleSpinBox->setToolTip(sizeToolTipText);
    _sizeSlider->setToolTip(sizeToolTipText);

    const auto opacityToolTipText = "Point opacity";

    _opacityLabel->setToolTip(opacityToolTipText);
    _opacityDoubleSpinBox->setToolTip(opacityToolTipText);
    _opacitySlider->setToolTip(opacityToolTipText);
}

void PointSettingsWidget::initialize(const ScatterplotPlugin& plugin)
{
    auto scatterPlotWidget = const_cast<ScatterplotPlugin&>(plugin).getScatterplotWidget();

    connect(_sizeSlider, &QSlider::valueChanged, [this, scatterPlotWidget](int value) {
        const auto pointSize = static_cast<float>(value) / 1000.0f;

        QSignalBlocker spinBoxBlocker(_sizeDoubleSpinBox);

        _sizeDoubleSpinBox->setValue(static_cast<double>(pointSize));

        scatterPlotWidget->setPointSize(pointSize);
    });

    connect(_sizeDoubleSpinBox, qOverload<double>(&QDoubleSpinBox::valueChanged), [this, scatterPlotWidget](double value) {
        const auto pointSize = static_cast<float>(value);

        QSignalBlocker sliderBlocker(_sizeSlider);

        _sizeSlider->setValue(static_cast<int>(pointSize * 1000.0f));

        scatterPlotWidget->setPointSize(pointSize);
    });

    connect(_opacitySlider, &QSlider::valueChanged, [this, scatterPlotWidget](int value) {
        const auto opacity = static_cast<float>(value);

        QSignalBlocker spinBoxBlocker(_opacityDoubleSpinBox);

        _opacityDoubleSpinBox->setValue(static_cast<double>(opacity));

        scatterPlotWidget->setAlpha(0.01f * opacity);
    });

    connect(_opacityDoubleSpinBox, qOverload<double>(&QDoubleSpinBox::valueChanged), [this, scatterPlotWidget](double value) {
        const auto opacity = static_cast<float>(value);

        QSignalBlocker sliderBlocker(_opacitySlider);

        _opacitySlider->setValue(static_cast<int>(opacity));

        scatterPlotWidget->setAlpha(0.01f * opacity);
    });

    _sizeDoubleSpinBox->setValue(5.0);
    _opacityDoubleSpinBox->setValue(50.0);
}

WidgetStateMixin::State PointSettingsWidget::getState(const QSize& sourceWidgetSize) const
{
    const auto width = sourceWidgetSize.width();

    auto state = WidgetStateMixin::State::Popup;

    if (width >= 1000 && width < 1500)
        state = WidgetStateMixin::State::Compact;

    if (width >= 1500)
        state = WidgetStateMixin::State::Full;

    return state;
}

void PointSettingsWidget::updateState()
{
    if (layout()) {
        delete layout();
    }

    QString sizeLabelText, opacityLabelText;

    const auto applyLayout = [this](QLayout* stateLayout) {
        Q_ASSERT(stateLayout != nullptr);

        stateLayout->setMargin(WidgetStateMixin::LAYOUT_MARGIN);
        stateLayout->setSpacing(WidgetStateMixin::LAYOUT_SPACING);

        setLayout(stateLayout);
    };

    switch (_state)
    {
        case State::Popup:
        {
            auto stateLayout = new QGridLayout();

            applyLayout(stateLayout);

            sizeLabelText       = "Size:";
            opacityLabelText    = "Opacity:";

            stateLayout->addWidget(_sizeLabel, 0, 0);
            stateLayout->addWidget(_sizeDoubleSpinBox, 0, 1);
            stateLayout->addWidget(_sizeSlider, 0, 2);

            stateLayout->addWidget(_opacityLabel, 1, 0);
            stateLayout->addWidget(_opacityDoubleSpinBox, 1, 1);
            stateLayout->addWidget(_opacitySlider, 1, 2);

            break;
        }

        case State::Compact:
        {
            sizeLabelText       = "Size:";
            opacityLabelText    = "Opacity:";

            auto stateLayout = new QHBoxLayout();

            applyLayout(stateLayout);

            stateLayout->addWidget(_sizeLabel);
            stateLayout->addWidget(_sizeDoubleSpinBox);
            stateLayout->addWidget(_sizeSlider);

            stateLayout->addWidget(_opacityLabel);
            stateLayout->addWidget(_opacityDoubleSpinBox);
            stateLayout->addWidget(_opacitySlider);

            break;
        }

        case State::Full:
        {
            sizeLabelText       = "Point size:";
            opacityLabelText    = "Point opacity:";

            auto stateLayout = new QHBoxLayout();

            applyLayout(stateLayout);

            stateLayout->addWidget(_sizeLabel);
            stateLayout->addWidget(_sizeDoubleSpinBox);
            stateLayout->addWidget(_sizeSlider);

            stateLayout->addWidget(_opacityLabel);
            stateLayout->addWidget(_opacityDoubleSpinBox);
            stateLayout->addWidget(_opacitySlider);

            break;
        }

        default:
            break;
    }

    _sizeDoubleSpinBox->setVisible(_state != WidgetStateMixin::State::Compact);
    _opacityDoubleSpinBox->setVisible(_state != WidgetStateMixin::State::Compact);

    _sizeSlider->setFixedWidth(_state == WidgetStateMixin::State::Compact ? 50 : 80);
    _opacitySlider->setFixedWidth(_state == WidgetStateMixin::State::Compact ? 50 : 80);

    _sizeLabel->setText(sizeLabelText);
    _opacityLabel->setText(opacityLabelText);
}