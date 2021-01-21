#include "SubsetSettingsWidget.h"
#include "ScatterplotPlugin.h"

#include "widgets/ResponsiveToolBar.h"

#include <QPushButton>
#include <QCheckBox>

using namespace hdps::gui;

SubsetSettingsWidget::SubsetSettingsWidget(QWidget* parent /*= nullptr*/) :
    QStackedWidget(parent),
    _widgetState(this),
    _popupPushButton(new PopupPushButton()),
    _widget(new QWidget()),
    _createSubsetPushButton(new QPushButton()),
    _fromSourceCheckBox(new QCheckBox())
{
    initializeUI();

    connect(&_widgetState, &WidgetState::updateState, [this](const WidgetState::State& state) {
        const auto setWidgetLayout = [this](QLayout* layout) -> void {
            if (_widget->layout())
                delete _widget->layout();

            layout->setMargin(ResponsiveToolBar::LAYOUT_MARGIN);
            layout->setSpacing(ResponsiveToolBar::LAYOUT_SPACING);

            _widget->setLayout(layout);
        };

        auto layout = new QHBoxLayout();

        setWidgetLayout(layout);

        switch (state)
        {
            case WidgetState::State::Popup:
            {
                _createSubsetPushButton->setText("Create");
                _fromSourceCheckBox->setText("From source dataset");
                
                setCurrentWidget(_popupPushButton);
                break;
            }

            case WidgetState::State::Compact:
            case WidgetState::State::Full:
            {
                layout->addWidget(_createSubsetPushButton);
                layout->addWidget(_fromSourceCheckBox);

                _createSubsetPushButton->setText("Create subset");
                _fromSourceCheckBox->setText(state == WidgetState::State::Compact ? "Source" : "From source");

                setCurrentWidget(_widget);
                break;
            }

            default:
                break;
        }
    });

    _widgetState.initialize();
}

void SubsetSettingsWidget::initializeUI()
{
    setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    _popupPushButton->setWidget(_widget);
    _popupPushButton->setIcon(Application::getIconFont("FontAwesome").getIcon("crop-alt"));
    
    connect(_popupPushButton, &PopupPushButton::popupClosed, [this]() {
        addWidget(_widget);
    });

    _widget->setWindowTitle("Subset");

    _createSubsetPushButton->setToolTip("Create a subset from the selected data points");

    _fromSourceCheckBox->setToolTip("Create a subset from source or derived data");
    _fromSourceCheckBox->setText("From source");

    addWidget(_popupPushButton);
    addWidget(_widget);
}

void SubsetSettingsWidget::setScatterPlotPlugin(const ScatterplotPlugin& plugin)
{
    auto& scatterplotPlugin = const_cast<ScatterplotPlugin&>(plugin);
    auto scatterPlotWidget  = const_cast<ScatterplotPlugin&>(plugin).getScatterplotWidget();

    const auto updateUI = [this, &scatterplotPlugin, scatterPlotWidget]() {
        const auto isScatterPlot = scatterPlotWidget->getRenderMode() == ScatterplotWidget::RenderMode::SCATTERPLOT;

        setEnabled(isScatterPlot && scatterplotPlugin.getNumSelectedPoints() >= 1);
    };

    QObject::connect(&plugin, qOverload<>(&ScatterplotPlugin::selectionChanged), [this, updateUI]() {
        updateUI();
    });

    QObject::connect(scatterPlotWidget, &ScatterplotWidget::renderModeChanged, [this, updateUI](const ScatterplotWidget::RenderMode& renderMode) {
        updateUI();
    });

    updateUI();
}