#include "PointSettingsWidget.h"
#include "ScatterplotPlugin.h"

#include "widgets/ResponsiveToolBar.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QSlider>

using namespace hdps::gui;

PointSettingsWidget::PointSettingsWidget(QWidget* parent /*= nullptr*/) :
    ResponsiveToolBar::StatefulWidget(parent, "Point"),
    _scatterplotPlugin(nullptr),
    _sizeLabel(new QLabel()),
    _sizeDoubleSpinBox(new QDoubleSpinBox()),
    _sizeSlider(new QSlider()),
    _opacityLabel(new QLabel()),
    _opacityDoubleSpinBox(new QDoubleSpinBox()),
    _opacitySlider(new QSlider())
{
    initializeUI();
}

void PointSettingsWidget::initializeUI()
{
    _sizeLabel->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    
    _sizeDoubleSpinBox->setMinimum(1.0);
    _sizeDoubleSpinBox->setMaximum(20.0);
    _sizeDoubleSpinBox->setDecimals(1);
    _sizeDoubleSpinBox->setSuffix("px");

    _sizeSlider->setOrientation(Qt::Horizontal);
    _sizeSlider->setMinimum(1000);
    _sizeSlider->setMaximum(20000);

    _opacityLabel->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    _opacityDoubleSpinBox->setMinimum(1.0);
    _opacityDoubleSpinBox->setMaximum(100.0);
    _opacityDoubleSpinBox->setDecimals(0);
    _opacityDoubleSpinBox->setSuffix("%");

    _opacitySlider->setOrientation(Qt::Horizontal);
    _opacitySlider->setMinimum(1);
    _opacitySlider->setMaximum(100);
}

void PointSettingsWidget::setScatterPlotPlugin(const ScatterplotPlugin& plugin)
{
    _scatterplotPlugin = &const_cast<ScatterplotPlugin&>(plugin);

    const auto setSizeTooltip = [this](const float& pointSize) {
        const auto toolTip = QString("Point size: %1").arg(QString::number(pointSize, 'f', 1));

        _sizeDoubleSpinBox->setToolTip(toolTip);
        _sizeSlider->setToolTip(toolTip);
    };

    const auto setOpacityTooltip = [this](const float& pointOpacity) {
        const auto toolTip = QString("Point opacity: %1\%").arg(QString::number(pointOpacity, 'f', 0));

        _opacityDoubleSpinBox->setToolTip(toolTip);
        _opacitySlider->setToolTip(toolTip);
    };

    auto scatterPlotWidget = _scatterplotPlugin->getScatterplotWidget();

    connect(_sizeSlider, &QSlider::valueChanged, [this, scatterPlotWidget, setSizeTooltip](int value) {
        const auto pointSize = static_cast<float>(value) / 1000.0f;

        QSignalBlocker spinBoxBlocker(_sizeDoubleSpinBox);

        _sizeDoubleSpinBox->setValue(static_cast<double>(pointSize));

        scatterPlotWidget->setPointSize(pointSize);

        setSizeTooltip(pointSize);
    });

    connect(_sizeDoubleSpinBox, qOverload<double>(&QDoubleSpinBox::valueChanged), [this, scatterPlotWidget, setSizeTooltip](double value) {
        const auto pointSize = static_cast<float>(value);

        QSignalBlocker sliderBlocker(_sizeSlider);

        _sizeSlider->setValue(static_cast<int>(pointSize * 1000.0f));

        scatterPlotWidget->setPointSize(pointSize);

        setSizeTooltip(pointSize);
    });

    connect(_opacitySlider, &QSlider::valueChanged, [this, scatterPlotWidget, setOpacityTooltip](int value) {
        const auto opacity = static_cast<float>(value);

        QSignalBlocker spinBoxBlocker(_opacityDoubleSpinBox);

        _opacityDoubleSpinBox->setValue(static_cast<double>(opacity));

        scatterPlotWidget->setAlpha(0.01f * opacity);

        setOpacityTooltip(opacity);
    });

    connect(_opacityDoubleSpinBox, qOverload<double>(&QDoubleSpinBox::valueChanged), [this, scatterPlotWidget, setOpacityTooltip](double value) {
        const auto opacity = static_cast<float>(value);

        QSignalBlocker sliderBlocker(_opacitySlider);

        _opacitySlider->setValue(static_cast<int>(opacity));

        scatterPlotWidget->setAlpha(0.01f * opacity);

        setOpacityTooltip(opacity);
    });

    _sizeDoubleSpinBox->setValue(5.0);
    _opacityDoubleSpinBox->setValue(50.0);
}

QLayout* PointSettingsWidget::getLayout(const ResponsiveToolBar::WidgetState& state)
{
    QLayout* stateLayout = nullptr;

    switch (state)
    {
        case ResponsiveToolBar::WidgetState::Popup:
        {
            auto layout = new QGridLayout();

            layout->addWidget(_sizeLabel, 0, 0);
            layout->addWidget(_sizeDoubleSpinBox, 0, 1);
            layout->addWidget(_sizeSlider, 0, 2);

            layout->addWidget(_opacityLabel, 1, 0);
            layout->addWidget(_opacityDoubleSpinBox, 1, 1);
            layout->addWidget(_opacitySlider, 1, 2);

            stateLayout = layout;

            break;
        }

        case ResponsiveToolBar::WidgetState::Compact:
        case ResponsiveToolBar::WidgetState::Full:
        {
            auto layout = new QHBoxLayout();

            layout->addWidget(_sizeLabel);
            layout->addWidget(_sizeDoubleSpinBox);
            layout->addWidget(_sizeSlider);

            layout->addWidget(_opacityLabel);
            layout->addWidget(_opacityDoubleSpinBox);
            layout->addWidget(_opacitySlider);

            stateLayout = layout;

            break;
        }

        default:
            break;
    }

    /*
    _sizeLabel->setText(state == ResponsiveToolBar::WidgetState::Full ? "Point size:" : "Size:");
    _opacityLabel->setText(state == ResponsiveToolBar::WidgetState::Full ? "Point opacity:" : "Opacity:");

    _sizeSlider->setFixedWidth(state == ResponsiveToolBar::WidgetState::Compact ? 50 : 80);
    _opacitySlider->setFixedWidth(state == ResponsiveToolBar::WidgetState::Compact ? 50 : 80);

    _sizeDoubleSpinBox->setVisible(state != ResponsiveToolBar::WidgetState::Compact);
    _opacityDoubleSpinBox->setVisible(state != ResponsiveToolBar::WidgetState::Compact);
    */

    return stateLayout;
}

QSize PointSettingsWidget::getSizeHint(const hdps::gui::ResponsiveToolBar::WidgetState& state)
{
    auto pointSettingsWidget = QSharedPointer<PointSettingsWidget>::create();

    pointSettingsWidget->setScatterPlotPlugin(*_scatterplotPlugin);
    pointSettingsWidget->setState(state);

    return pointSettingsWidget->sizeHint();
}