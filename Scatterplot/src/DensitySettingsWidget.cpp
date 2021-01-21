#include "DensitySettingsWidget.h"
#include "ScatterplotPlugin.h"

#include <QLabel>
#include <QDoubleSpinBox>
#include <QSlider>

using namespace hdps::gui;

DensitySettingsWidget::DensitySettingsWidget(QWidget* parent /*= nullptr*/) :
    QStackedWidget(parent),
    _widgetState(this),
    _popupPushButton(new PopupPushButton()),
    _widget(new QWidget()),
    _label(new QLabel()),
    _doubleSpinBox(new QDoubleSpinBox()),
    _slider(new QSlider())
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
                /*
                auto layout = new QHBoxLayout();

                setWidgetLayout(layout);

                layout->addWidget(_scatterPlotPushButton);
                layout->addWidget(_densityPlotPushButton);
                layout->addWidget(_contourPlotPushButton);

                _scatterPlotPushButton->setText("Scatter Plot");
                _densityPlotPushButton->setText("Density Plot");
                _contourPlotPushButton->setText("Contour Plot");
                */
                setCurrentWidget(_popupPushButton);
                break;
            }

            case WidgetState::State::Compact:
            case WidgetState::State::Full:
            {
                auto layout = new QHBoxLayout();

                setWidgetLayout(layout);

                layout->addWidget(_label);
                layout->addWidget(_doubleSpinBox);
                layout->addWidget(_slider);

                setCurrentIndex(state == WidgetState::State::Popup ? 0 : 1);

                _doubleSpinBox->setVisible(state != WidgetState::State::Compact);
                _slider->setFixedWidth(state == WidgetState::State::Compact ? 50 : 80);

                setCurrentWidget(_widget);
                break;
            }

            default:
                break;
        }
    });

    _widgetState.initialize();
}

void DensitySettingsWidget::initializeUI()
{
    setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    _popupPushButton->setIcon(Application::getIconFont("FontAwesome").getIcon("cog"));

    _label->setText("Sigma:");

    _doubleSpinBox->setMinimum(1.0);
    _doubleSpinBox->setMaximum(50.0);
    _doubleSpinBox->setDecimals(1);

    _slider->setOrientation(Qt::Horizontal);
    _slider->setMinimum(1);
    _slider->setMaximum(50);

    const auto toolTipText = "Density sigma";

    _label->setToolTip(toolTipText);
    _doubleSpinBox->setToolTip(toolTipText);
    _slider->setToolTip(toolTipText);

    addWidget(_popupPushButton);
    addWidget(_widget);
}

void DensitySettingsWidget::setScatterPlotPlugin(const ScatterplotPlugin& plugin)
{
    auto scatterPlotWidget = const_cast<ScatterplotPlugin&>(plugin).getScatterplotWidget();

    QObject::connect(_slider, &QSlider::sliderReleased, [this, scatterPlotWidget]() {
        const auto sigma = static_cast<float>(_slider->value());

        QSignalBlocker doubleSpinBoxBlocker(_doubleSpinBox);

        _doubleSpinBox->setValue(static_cast<double>(sigma));

        scatterPlotWidget->setSigma(0.01f * sigma);
    });

    QObject::connect(_doubleSpinBox, qOverload<double>(&QDoubleSpinBox::valueChanged), [this, scatterPlotWidget](double value) {
        const auto sigma = static_cast<float>(value);

        QSignalBlocker sliderBlocker(_slider);

        _slider->setValue(static_cast<int>(sigma));

        scatterPlotWidget->setSigma(0.01f * sigma);
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