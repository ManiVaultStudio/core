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
    QStackedWidget(parent),
    _widgetState(this),
    _popupPushButton(new PopupPushButton()),
    _widget(new QWidget()),
    _sizeLabel(new QLabel()),
    _sizeDoubleSpinBox(new QDoubleSpinBox()),
    _sizeSlider(new QSlider()),
    _opacityLabel(new QLabel()),
    _opacityDoubleSpinBox(new QDoubleSpinBox()),
    _opacitySlider(new QSlider())
{
    initializeUI();

    connect(&_widgetState, &WidgetState::updateState, [this](const WidgetState::State& state) {
        auto& fontAwesome = Application::getIconFont("FontAwesome");

        const auto setWidgetLayout = [this](QLayout* layout) -> void {
            if (_widget->layout())
                delete _widget->layout();

            layout->setMargin(ResponsiveToolBar::LAYOUT_MARGIN);
            layout->setSpacing(ResponsiveToolBar::LAYOUT_SPACING);

            _widget->setLayout(layout);
        };

        switch (state)
        {
            case WidgetState::State::Popup:
            {
                auto layout = new QGridLayout();

                setWidgetLayout(layout);

                layout->addWidget(_sizeLabel, 0, 0);
                layout->addWidget(_sizeDoubleSpinBox, 0, 1);
                layout->addWidget(_sizeSlider, 0, 2);

                layout->addWidget(_opacityLabel, 1, 0);
                layout->addWidget(_opacityDoubleSpinBox, 1, 1);
                layout->addWidget(_opacitySlider, 1, 2);

                setCurrentWidget(_popupPushButton);
                removeWidget(_widget);
                break;
            }

            case WidgetState::State::Compact:
            case WidgetState::State::Full:
            {
                auto layout = new QHBoxLayout();

                setWidgetLayout(layout);

                layout->addWidget(_sizeLabel);
                layout->addWidget(_sizeDoubleSpinBox);
                layout->addWidget(_sizeSlider);

                layout->addWidget(_opacityLabel);
                layout->addWidget(_opacityDoubleSpinBox);
                layout->addWidget(_opacitySlider);

                if (count() == 1)
                    addWidget(_widget);

                setCurrentWidget(_widget);
                break;
            }

            default:
                break;
        }

        _sizeLabel->setText(state == WidgetState::State::Full ? "Point size:" : "Size:");
        _opacityLabel->setText(state == WidgetState::State::Full ? "Point opacity:" : "Opacity:");

        _sizeSlider->setFixedWidth(state == WidgetState::State::Compact ? 50 : 80);
        _opacitySlider->setFixedWidth(state == WidgetState::State::Compact ? 50 : 80);

        _sizeDoubleSpinBox->setVisible(state != WidgetState::State::Compact);
        _opacityDoubleSpinBox->setVisible(state != WidgetState::State::Compact);
    });

    _widgetState.initialize();
}

void PointSettingsWidget::initializeUI()
{
    setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    setWindowTitle("Point settings");
    setToolTip("Point settings");

    _popupPushButton->setWidget(_widget);
    _popupPushButton->setIcon(Application::getIconFont("FontAwesome").getIcon("paint-brush"));
    
    _widget->setWindowTitle("Point rendering");

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

    addWidget(_popupPushButton);
    addWidget(_widget);
}

void PointSettingsWidget::setScatterPlotPlugin(const ScatterplotPlugin& plugin)
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