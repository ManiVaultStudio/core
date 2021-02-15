#include "ColoringAction.h"
#include "Application.h"

#include "ScatterplotPlugin.h"

using namespace hdps::gui;

ColoringAction::ColoringAction(ScatterplotPlugin* scatterplotPlugin) :
    PluginAction(scatterplotPlugin),
    _colorByAction(this, "Color by"),
    _colorByConstantColorAction("Color by constant color"),
    _colorByDimensionAction("Color by dimension"),
    _colorByColorDataAction("Color by color data"),
    _colorByActionGroup(this),
    _constantColorAction(this, "Constant color", qRgb(93, 93, 225)),
    _dimensionAction(this, "Color dimension"),
    _colorDataAction(this, "Color data"),
    _removeColorDataAction("Remove"),
    _resetAction("Reset")
{
    scatterplotPlugin->addAction(&_colorByAction);
    scatterplotPlugin->addAction(&_colorByDimensionAction);
    scatterplotPlugin->addAction(&_colorByColorDataAction);
    scatterplotPlugin->addAction(&_dimensionAction);
    scatterplotPlugin->addAction(&_colorDataAction);
    scatterplotPlugin->addAction(&_removeColorDataAction);
    scatterplotPlugin->addAction(&_resetAction);

    _colorByAction.setOptions(QStringList() << "Constant color" << "Dimension" << "Color data");

    _colorByAction.setToolTip("Color by");
    _colorByConstantColorAction.setToolTip("Color data points with a constant color");
    _colorByDimensionAction.setToolTip("Color data points by chosen dimension");
    _colorByColorDataAction.setToolTip("Color data points with a color data set");
    _constantColorAction.setToolTip("Constant color");
    _dimensionAction.setToolTip("Color dimension");
    _colorDataAction.setToolTip("Color data");
    _removeColorDataAction.setToolTip("Remove color data");
    _resetAction.setToolTip("Reset color settings");

    _colorByConstantColorAction.setCheckable(true);
    _colorByDimensionAction.setCheckable(true);
    _colorByColorDataAction.setCheckable(true);

    _colorDataAction.setEnabled(false);

    _colorByActionGroup.addAction(&_colorByConstantColorAction);
    _colorByActionGroup.addAction(&_colorByDimensionAction);
    _colorByActionGroup.addAction(&_colorByColorDataAction);

    const auto renderModeChanged = [this]() -> void {
        setEnabled(getScatterplotWidget()->getRenderMode() == ScatterplotWidget::SCATTERPLOT);
    };

    connect(getScatterplotWidget(), &ScatterplotWidget::renderModeChanged, this, [this, renderModeChanged](const ScatterplotWidget::RenderMode& renderMode) {
        renderModeChanged();
    });

    connect(&_colorByAction, &OptionAction::currentIndexChanged, this, [this](const std::uint32_t& currentIndex) {
        getScatterplotWidget()->setColoringMode(static_cast<ScatterplotWidget::ColoringMode>(currentIndex));
    });

    connect(&_colorByConstantColorAction, &QAction::triggered, this, [this]() {
        getScatterplotWidget()->setColoringMode(ScatterplotWidget::ColoringMode::ConstantColor);
    });

    connect(&_colorByDimensionAction, &QAction::triggered, this, [this]() {
        getScatterplotWidget()->setColoringMode(ScatterplotWidget::ColoringMode::Dimension);
    });

    connect(&_colorByColorDataAction, &QAction::triggered, this, [this]() {
        getScatterplotWidget()->setColoringMode(ScatterplotWidget::ColoringMode::ColorData);
    });

    const auto updateConstantColor = [this]() -> void {
        if (_scatterplotPlugin->getNumberOfPoints() == 0)
            return;

        std::vector<Vector3f> colors(_scatterplotPlugin->getNumberOfPoints());

        const auto color = _constantColorAction.getColor();

        std::fill(colors.begin(), colors.end(), Vector3f(color.redF(), color.greenF(), color.blueF()));

        getScatterplotWidget()->setColors(colors);
    };

    const auto updateDimension = [this]() -> void {
        _scatterplotPlugin->setColorDimension(_dimensionAction.getCurrentIndex());
    };

    const auto coloringModeChanged = [this, updateConstantColor, updateDimension]() -> void {
        const auto coloringMode = getScatterplotWidget()->getColoringMode();

        _colorByAction.setCurrentIndex(static_cast<std::int32_t>(coloringMode));

        _colorByConstantColorAction.setChecked(coloringMode == ScatterplotWidget::ColoringMode::ConstantColor);
        _colorByDimensionAction.setChecked(coloringMode == ScatterplotWidget::ColoringMode::Dimension);
        _colorByColorDataAction.setChecked(coloringMode == ScatterplotWidget::ColoringMode::ColorData);

        switch (coloringMode)
        {
            case ScatterplotWidget::ColoringMode::ConstantColor:
                updateConstantColor();
                break;

            case ScatterplotWidget::ColoringMode::Dimension:
                updateDimension();
                break;

            case ScatterplotWidget::ColoringMode::ColorData:
                break;

            default:
                break;
        }
    };

    connect(getScatterplotWidget(), &ScatterplotWidget::coloringModeChanged, this, [this, coloringModeChanged](const ScatterplotWidget::ColoringMode& coloringMode) {
        coloringModeChanged();
    });

    connect(&_constantColorAction, &ColorAction::colorChanged, this, [this, updateConstantColor](const QColor& color) {
        updateConstantColor();
    });

    connect(&_dimensionAction, &OptionAction::currentIndexChanged, this, [this](const std::uint32_t& currentIndex) {
        _scatterplotPlugin->setColorDimension(currentIndex);
    });

    connect(_scatterplotPlugin, &ScatterplotPlugin::currentDatasetChanged, this, [this, coloringModeChanged](const QString& datasetName) {
        coloringModeChanged();
    });

    renderModeChanged();
    coloringModeChanged();
}

QMenu* ColoringAction::getContextMenu()
{
    auto menu = new QMenu("Color");

    const auto addActionToMenu = [menu](QAction* action) -> void {
        auto actionMenu = new QMenu(action->text());

        actionMenu->addAction(action);

        menu->addMenu(actionMenu);
    };

    const auto renderMode = _scatterplotPlugin->getScatterplotWidget()->getRenderMode();

    menu->setEnabled(renderMode == ScatterplotWidget::RenderMode::SCATTERPLOT);

    menu->addAction(&_colorByConstantColorAction);
    menu->addAction(&_colorByDimensionAction);
    menu->addAction(&_colorByColorDataAction);
    
    menu->addSeparator();

    switch (_scatterplotPlugin->getScatterplotWidget()->getColoringMode())
    {
        case ScatterplotWidget::ColoringMode::ConstantColor:
            addActionToMenu(&_constantColorAction);
            break;

        case ScatterplotWidget::ColoringMode::Dimension:
            addActionToMenu(&_dimensionAction);
            break;

        case ScatterplotWidget::ColoringMode::ColorData:
            addActionToMenu(&_colorDataAction);
            break;

        default:
            break;
    }

    return menu;
}

void ColoringAction::setDimensions(const std::uint32_t& numberOfDimensions, const std::vector<QString>& dimensionNames /*= std::vector<QString>()*/)
{
    _dimensionAction.setOptions(common::getDimensionNamesStringList(numberOfDimensions, dimensionNames));
}

void ColoringAction::setDimensions(const std::vector<QString>& dimensionNames)
{
    setDimensions(static_cast<std::uint32_t>(dimensionNames.size()), dimensionNames);
}

ColoringAction::Widget::Widget(QWidget* parent, ColoringAction* coloringAction) :
    WidgetAction::Widget(parent, coloringAction),
    _layout(),
    _colorByLabel("Color by:")
{
    _layout.addWidget(&_colorByLabel);
    _layout.addWidget(new OptionAction::Widget(this, &coloringAction->_colorByAction));
    _layout.addWidget(new ColorAction::Widget(this, &coloringAction->_constantColorAction));
    _layout.addWidget(new OptionAction::Widget(this, &coloringAction->_dimensionAction));
    _layout.addWidget(new StringAction::Widget(this, &coloringAction->_colorDataAction));

    setLayout(&_layout);

    const auto coloringModeChanged = [this, coloringAction]() -> void {
        const auto coloringMode = static_cast<ScatterplotWidget::ColoringMode>(coloringAction->_colorByAction.getCurrentIndex());

        coloringAction->_constantColorAction.setVisible(coloringMode == ScatterplotWidget::ColoringMode::ConstantColor);
        coloringAction->_dimensionAction.setVisible(coloringMode == ScatterplotWidget::ColoringMode::Dimension);
        coloringAction->_colorDataAction.setVisible(coloringMode == ScatterplotWidget::ColoringMode::ColorData);
    };

    connect(&coloringAction->_colorByAction, &OptionAction::currentIndexChanged, this, [this, coloringModeChanged](const std::uint32_t& currentIndex) {
        coloringModeChanged();
    });

    coloringModeChanged();
}