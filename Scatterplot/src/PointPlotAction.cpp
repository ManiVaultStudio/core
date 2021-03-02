#include "PointPlotAction.h"
#include "Application.h"

#include "ScatterplotWidget.h"

using namespace hdps::gui;

PointPlotAction::PointPlotAction(ScatterplotPlugin* scatterplotPlugin) :
    PluginAction(scatterplotPlugin, "Point"),
    _pointSizeAction(this, "Point size", 1.0, 50.0, DEFAULT_POINT_SIZE, DEFAULT_POINT_SIZE),
    _pointOpacityAction(this, "Point opacity", 0.0, 100.0, DEFAULT_POINT_OPACITY, DEFAULT_POINT_OPACITY)
{
    _pointSizeAction.setSuffix("px");
    _pointOpacityAction.setSuffix("%");

    const auto updateRenderMode = [this]() -> void {
        setVisible(getScatterplotWidget()->getRenderMode() == ScatterplotWidget::SCATTERPLOT);
    };

    const auto updatePointSize = [this]() -> void {
        getScatterplotWidget()->setPointSize(_pointSizeAction.getValue());
    };

    const auto updatePointOpacity = [this]() -> void {
        getScatterplotWidget()->setAlpha(0.01 * _pointOpacityAction.getValue());
    };

    connect(getScatterplotWidget(), &ScatterplotWidget::renderModeChanged, this, [this, updateRenderMode](const ScatterplotWidget::RenderMode& renderMode) {
        updateRenderMode();
    });

    connect(&_pointSizeAction, &DoubleAction::valueChanged, this, [this, updatePointSize](const double& value) {
        updatePointSize();
    });

    connect(&_pointOpacityAction, &DoubleAction::valueChanged, this, [this, updatePointOpacity](const double& value) {
        updatePointOpacity();
    });

    updateRenderMode();
    updatePointSize();
    updatePointOpacity();
}

QMenu* PointPlotAction::getContextMenu()
{
    auto menu = new QMenu("Plot settings");

    const auto renderMode = getScatterplotWidget()->getRenderMode();

    const auto addActionToMenu = [menu](QAction* action) {
        auto actionMenu = new QMenu(action->text());

        actionMenu->addAction(action);

        menu->addMenu(actionMenu);
    };

    addActionToMenu(&_pointSizeAction);
    addActionToMenu(&_pointOpacityAction);

    return menu;
}

PointPlotAction::Widget::Widget(QWidget* parent, PointPlotAction* pointPlotAction) :
    WidgetAction::Widget(parent, pointPlotAction),
    _layout(),
    _pointSizelabel("Point size:"),
    _pointOpacitylabel("Point opacity:")
{
    setToolTip("Point plot settings");

    _layout.addWidget(&_pointSizelabel);
    _layout.addWidget(new DoubleAction::Widget(this, &pointPlotAction->_pointSizeAction, DoubleAction::Widget::Configuration::Slider));

    _layout.addWidget(&_pointOpacitylabel);
    _layout.addWidget(new DoubleAction::Widget(this, &pointPlotAction->_pointOpacityAction, DoubleAction::Widget::Configuration::Slider));

    setLayout(&_layout);
}

PointPlotAction::PopupWidget::PopupWidget(QWidget* parent, PointPlotAction* pointPlotAction) :
    WidgetAction::PopupWidget(parent, pointPlotAction)
{
    auto layout = new QGridLayout();

    layout->addWidget(new QLabel("Size:"), 0, 0);
    layout->addWidget(new DoubleAction::Widget(this, &pointPlotAction->_pointSizeAction), 0, 2);

    layout->addWidget(new QLabel("Opacity:"), 1, 0);
    layout->addWidget(new DoubleAction::Widget(this, &pointPlotAction->_pointOpacityAction), 1, 2);

    setLayout(layout);
}