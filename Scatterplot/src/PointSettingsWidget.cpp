#include "PointSettingsWidget.h"
#include "ScatterplotPlugin.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QSlider>

using namespace hdps::gui;

PointSettingsWidget::PointSettingsWidget(QWidget* parent /*= nullptr*/) :
    ResponsiveToolBar::Widget("Point", 100),
    _sizeLabel(new QLabel()),
    _sizeDoubleSpinBox(new QDoubleSpinBox()),
    _sizeSlider(new QSlider()),
    _opacityLabel(new QLabel()),
    _opacityDoubleSpinBox(new QDoubleSpinBox()),
    _opacitySlider(new QSlider())
{
    _popupPushButton->setIcon(Application::getIconFont("FontAwesome").getIcon("cog"));

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

    computeStateSizes();
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

void PointSettingsWidget::updateState()
{
    /*
    switch (_state)
    {
        case State::Popup:
        {
            auto layout = new QHBoxLayout();

            setWidgetLayout(layout);

            layout->addWidget(_sizeLabel);
            layout->addWidget(_sizeDoubleSpinBox);
            layout->addWidget(_sizeSlider);

            layout->addWidget(_opacityLabel);
            layout->addWidget(_opacityDoubleSpinBox);
            layout->addWidget(_opacitySlider);

            setCurrentIndex(0);
            break;
        }

        case State::Compact:
        case State::Full:
        {
            auto layout = new QHBoxLayout();

            setWidgetLayout(layout);

            layout->addWidget(_sizeLabel);
            layout->addWidget(_sizeDoubleSpinBox);
            layout->addWidget(_sizeSlider);

            layout->addWidget(_opacityLabel);
            layout->addWidget(_opacityDoubleSpinBox);
            layout->addWidget(_opacitySlider);

            layout->invalidate();
            layout->activate();

            setCurrentIndex(1);
            break;
        }

        default:
            break;
    }

    _sizeLabel->setText(_state == State::Full ? "Point size:" : "Size:");
    _opacityLabel->setText(_state == State::Full ? "Point opacity:" : "Opacity:");
    
    _sizeSlider->setFixedWidth(_state == State::Compact ? 50 : 80);
    _opacitySlider->setFixedWidth(_state == State::Compact ? 50 : 80);

    _sizeDoubleSpinBox->setVisible(_state != State::Compact);
    _opacityDoubleSpinBox->setVisible(_state != State::Compact);
    */
}