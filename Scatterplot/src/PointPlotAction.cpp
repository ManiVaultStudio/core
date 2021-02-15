#include "PointPlotAction.h"
#include "Application.h"

#include "ScatterplotWidget.h"

using namespace hdps::gui;

PointPlotAction::PointPlotAction(ScatterplotPlugin* scatterplotPlugin) :
    PluginAction(scatterplotPlugin),
    _pointSizeByAction(this, "Point size by"),
    _pointSizeAction(this, "Point size", 1.0, 50.0, DEFAULT_POINT_SIZE),
    _pointOpacityByAction(this, "Point opacity by"),
    _pointOpacityAction(this, "Point opacity", 0.0, 100.0, DEFAULT_POINT_OPACITY),
    _resetAction(this, "Reset")
{
    _pointSizeByAction.setVisible(false);
    _pointOpacityByAction.setVisible(false);

    _pointSizeByAction.setOptions(QStringList() << "Constant" << "Dimension");
    _pointOpacityByAction.setOptions(QStringList() << "Constant" << "Dimension");

    _pointSizeAction.setSuffix("px");
    _pointOpacityAction.setSuffix("%");
    
    _resetAction.setToolTip("Reset point plot settings");

    const auto updateRenderMode = [this]() -> void {
        setVisible(getScatterplotWidget()->getRenderMode() == ScatterplotWidget::SCATTERPLOT);
    };

    const auto updatePointSize = [this]() -> void {
        getScatterplotWidget()->setPointSize(_pointSizeAction.getValue());
    };

    const auto updatePointOpacity = [this]() -> void {
        getScatterplotWidget()->setAlpha(0.01 * _pointOpacityAction.getValue());
    };

    const auto updateResetAction = [this]() -> void {
        _resetAction.setEnabled(canReset());
    };

    connect(getScatterplotWidget(), &ScatterplotWidget::renderModeChanged, this, [this, updateRenderMode, updateResetAction](const ScatterplotWidget::RenderMode& renderMode) {
        updateRenderMode();
        updateResetAction();
    });

    connect(&_pointSizeAction, &DoubleAction::valueChanged, this, [this, updatePointSize, updateResetAction](const double& value) {
        updatePointSize();
        updateResetAction();
    });

    connect(&_pointOpacityAction, &DoubleAction::valueChanged, this, [this, updatePointOpacity, updateResetAction](const double& value) {
        updatePointOpacity();
        updateResetAction();
    });

    connect(&_resetAction, &QAction::triggered, this, [this]() {
        reset();
    });

    updateRenderMode();
    updatePointSize();
    updatePointOpacity();
    updateResetAction();
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
    
    menu->addSeparator();

    menu->addAction(&_resetAction);

    return menu;
}

bool PointPlotAction::canReset() const
{
    return _pointSizeAction.getValue() != DEFAULT_POINT_SIZE || _pointOpacityAction.getValue() != DEFAULT_POINT_OPACITY;
}

void PointPlotAction::reset()
{
    _pointSizeAction.setValue(DEFAULT_POINT_SIZE);
    _pointOpacityAction.setValue(DEFAULT_POINT_OPACITY);
}

PointPlotAction::Widget::Widget(QWidget* parent, PointPlotAction* pointPlotAction) :
    WidgetAction::Widget(parent, pointPlotAction),
    _layout(),
    _pointSizelabel("Point size:"),
    _pointOpacitylabel("Point opacity:")
{
    setToolTip("Point plot settings");

    _layout.addWidget(&_pointSizelabel);
    _layout.addWidget(pointPlotAction->_pointSizeByAction.createWidget(this));
    _layout.addWidget(pointPlotAction->_pointSizeAction.createWidget(this));

    _layout.addWidget(&_pointOpacitylabel);
    _layout.addWidget(pointPlotAction->_pointOpacityByAction.createWidget(this));
    _layout.addWidget(pointPlotAction->_pointOpacityAction.createWidget(this));

    setLayout(&_layout);
}