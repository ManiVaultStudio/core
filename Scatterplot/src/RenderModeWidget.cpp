#include "RenderModeWidget.h"
#include "ScatterplotPlugin.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

using namespace hdps::gui;

RenderModeWidget::RenderModeWidget(const hdps::gui::ResponsiveSectionWidget::State& state, QWidget* parent /*= nullptr*/) :
    ScatterplotSettingsWidget(state, parent),
    _scatterPlotPushButton(new QPushButton(this)),
    _densityPlotPushButton(new QPushButton(this)),
    _contourPlotPushButton(new QPushButton(this))
{
    _scatterPlotPushButton->setCheckable(true);
    _densityPlotPushButton->setCheckable(true);
    _contourPlotPushButton->setCheckable(true);

    _scatterPlotPushButton->setToolTip("Change render mode to scatter plot");
    _densityPlotPushButton->setToolTip("Change render mode to density map plot");
    _contourPlotPushButton->setToolTip("Change render mode to contour map plot");

    QLayout* layout = nullptr;

    const auto& fontAwesome = Application::getIconFont("FontAwesome");

    switch (_state)
    {
        case ResponsiveSectionWidget::State::Popup:
        {
            layout = new QVBoxLayout();

            _scatterPlotPushButton->setText("Scatter Plot");
            _densityPlotPushButton->setText("Density Plot");
            _contourPlotPushButton->setText("Contour Plot");

            _scatterPlotPushButton->setIcon(QIcon());
            _densityPlotPushButton->setIcon(QIcon());
            _contourPlotPushButton->setIcon(QIcon());

            break;
        }

        case ResponsiveSectionWidget::State::Compact:
        {
            layout = new QHBoxLayout();

            _scatterPlotPushButton->setText("");
            _densityPlotPushButton->setText("");
            _contourPlotPushButton->setText("");

            _scatterPlotPushButton->setIcon(fontAwesome.getIcon("braille"));
            _densityPlotPushButton->setIcon(fontAwesome.getIcon("cloud"));
            _contourPlotPushButton->setIcon(fontAwesome.getIcon("mountain"));

            break;
        }

        case ResponsiveSectionWidget::State::Full:
        {
            layout = new QHBoxLayout();

            _scatterPlotPushButton->setText("Scatter");
            _densityPlotPushButton->setText("Density");
            _contourPlotPushButton->setText("Contour");

            _scatterPlotPushButton->setIcon(QIcon());
            _densityPlotPushButton->setIcon(QIcon());
            _contourPlotPushButton->setIcon(QIcon());

            break;
        }

        default:
            break;
    }
    
    layout->setMargin(0);
    layout->setSpacing(4);

    layout->addWidget(_scatterPlotPushButton);
    layout->addWidget(_densityPlotPushButton);
    layout->addWidget(_contourPlotPushButton);

    setLayout(layout);
}

void RenderModeWidget::setScatterplotPlugin(ScatterplotPlugin* scatterplotPlugin)
{
    _scatterplotPlugin = scatterplotPlugin;

    auto scatterPlotWidget = _scatterplotPlugin->getScatterplotWidget();

    const auto updateToggles = [this, scatterPlotWidget]() {
        const auto renderMode = scatterPlotWidget->getRenderMode();

        _scatterPlotPushButton->setChecked(renderMode == ScatterplotWidget::RenderMode::SCATTERPLOT);
        _densityPlotPushButton->setChecked(renderMode == ScatterplotWidget::RenderMode::DENSITY);
        _contourPlotPushButton->setChecked(renderMode == ScatterplotWidget::RenderMode::LANDSCAPE);
    };

    QObject::connect(_scatterPlotPushButton, &QPushButton::clicked, this, [this, scatterPlotWidget, updateToggles]() {
        scatterPlotWidget->setRenderMode(ScatterplotWidget::RenderMode::SCATTERPLOT);
        updateToggles();
    });

    QObject::connect(_densityPlotPushButton, &QPushButton::clicked, this, [this, scatterPlotWidget, updateToggles]() {
        scatterPlotWidget->setRenderMode(ScatterplotWidget::RenderMode::DENSITY);
        updateToggles();
    });

    QObject::connect(_contourPlotPushButton, &QPushButton::clicked, this, [this, scatterPlotWidget, updateToggles]() {
        scatterPlotWidget->setRenderMode(ScatterplotWidget::RenderMode::LANDSCAPE);
        updateToggles();
    });

    QObject::connect(scatterPlotWidget, &ScatterplotWidget::renderModeChanged, this, [this, updateToggles](const ScatterplotWidget::RenderMode& renderMode) {
        updateToggles();
    });

    updateToggles();
}