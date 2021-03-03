#include "ColorDimensionAction.h"
#include "Application.h"

#include "ScatterplotPlugin.h"

using namespace hdps::gui;

ColorDimensionAction::ColorDimensionAction(ScatterplotPlugin* scatterplotPlugin) :
    PluginAction(scatterplotPlugin, "Coloring"),
    _colorDimensionAction(this, "Color dimension")
{
    scatterplotPlugin->addAction(&_colorDimensionAction);

    const auto updateColorDimension = [this]() -> void {
        if (!_colorDimensionAction.hasSelection())
            return;

        if (getScatterplotWidget()->getColoringMode() == ScatterplotWidget::ColoringMode::ColorDimension)
            _scatterplotPlugin->setColorDimension(_colorDimensionAction.getCurrentIndex());
    };

    connect(&_colorDimensionAction, &OptionAction::currentIndexChanged, this, [this, updateColorDimension](const std::uint32_t& currentIndex) {
        updateColorDimension();
    });

    connect(getScatterplotWidget(), &ScatterplotWidget::coloringModeChanged, this, [this, updateColorDimension](const ScatterplotWidget::ColoringMode& coloringMode) {
        if (coloringMode == ScatterplotWidget::ColoringMode::ColorDimension)
            updateColorDimension();
    });

    updateColorDimension();
}

QMenu* ColorDimensionAction::getContextMenu()
{
    auto menu = new QMenu("Color dimension");

    menu->addAction(&_colorDimensionAction);

    return menu;
}

void ColorDimensionAction::setDimensions(const std::uint32_t& numberOfDimensions, const std::vector<QString>& dimensionNames /*= std::vector<QString>()*/)
{
    _colorDimensionAction.setOptions(common::getDimensionNamesStringList(numberOfDimensions, dimensionNames));
}

void ColorDimensionAction::setDimensions(const std::vector<QString>& dimensionNames)
{
    setDimensions(static_cast<std::uint32_t>(dimensionNames.size()), dimensionNames);
}

ColorDimensionAction::Widget::Widget(QWidget* parent, ColorDimensionAction* colorDimensionAction) :
    WidgetAction::Widget(parent, colorDimensionAction),
    _layout()
{
    _layout.addWidget(new OptionAction::Widget(this, &colorDimensionAction->_colorDimensionAction, false));

    setLayout(&_layout);
}

ColorDimensionAction::PopupWidget::PopupWidget(QWidget* parent, ColorDimensionAction* colorDimensionAction) :
    WidgetAction::PopupWidget(parent, colorDimensionAction),
    _layout()
{
    _layout.addWidget(new OptionAction::Widget(this, &colorDimensionAction->_colorDimensionAction));

    setLayout(&_layout);
}