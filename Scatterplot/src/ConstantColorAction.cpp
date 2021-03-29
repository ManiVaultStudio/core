#include "ConstantColorAction.h"
#include "Application.h"

#include "ScatterplotPlugin.h"
#include "ScatterplotWidget.h"

#include <QHBoxLayout>
#include <QMenu>

using namespace hdps::gui;

const QColor ConstantColorAction::DEFAULT_COLOR = qRgb(93, 93, 225);

ConstantColorAction::ConstantColorAction(ScatterplotPlugin* scatterplotPlugin) :
    PluginAction(scatterplotPlugin, "Coloring"),
    _constantColorAction(this, "Constant color", DEFAULT_COLOR, DEFAULT_COLOR),
    _resetAction(this, "Reset")
{
    _constantColorAction.setToolTip("Constant color");
    _resetAction.setToolTip("Reset color settings");

    const auto updateConstantColor = [this]() -> void {
        if (_scatterplotPlugin->getNumberOfPoints() == 0)
            return;

        std::vector<Vector3f> colors(_scatterplotPlugin->getNumberOfPoints());

        const auto color = _constantColorAction.getColor();

        std::fill(colors.begin(), colors.end(), Vector3f(color.redF(), color.greenF(), color.blueF()));

        getScatterplotWidget()->setColors(colors);
    };

    const auto updateResetAction = [this]() -> void {
        _resetAction.setEnabled(_constantColorAction.getColor() != DEFAULT_COLOR);
    };

    connect(&_constantColorAction, &ColorAction::colorChanged, this, [this, updateConstantColor, updateResetAction](const QColor& color) {
        updateConstantColor();
        updateResetAction();
    });

    connect(&_resetAction, &QAction::triggered, this, [this, updateResetAction](const QColor& color) {
        _constantColorAction.setColor(DEFAULT_COLOR);
    });

    connect(getScatterplotWidget(), &ScatterplotWidget::coloringModeChanged, this, [this, updateConstantColor](const ScatterplotWidget::ColoringMode& coloringMode) {
        if (coloringMode == ScatterplotWidget::ColoringMode::ConstantColor)
            updateConstantColor();
    });

    connect(_scatterplotPlugin, &ScatterplotPlugin::currentDatasetChanged, this, [this, updateConstantColor](const QString& datasetName) {
        if (getScatterplotWidget()->getColoringMode() == ScatterplotWidget::ColoringMode::ConstantColor)
            updateConstantColor();
    });

    updateResetAction();
    updateConstantColor();
}

QMenu* ConstantColorAction::getContextMenu()
{
    auto menu = new QMenu("Constant color");

    menu->addAction(&_constantColorAction);
    menu->addAction(&_resetAction);

    return menu;
}

ConstantColorAction::Widget::Widget(QWidget* parent, ConstantColorAction* constantColorAction) :
    WidgetAction::Widget(parent, constantColorAction)
{
    auto layout = new QHBoxLayout();

    layout->addWidget(new ColorAction::Widget(this, &constantColorAction->_constantColorAction, false));

    setLayout(layout);
}

ConstantColorAction::PopupWidget::PopupWidget(QWidget* parent, ConstantColorAction* constantColorAction) :
    WidgetAction::Widget(parent, constantColorAction)
{
    auto layout = new QHBoxLayout();

    layout->addWidget(new ColorAction::PopupWidget(this, &constantColorAction->_constantColorAction));

    setLayout(layout);
}