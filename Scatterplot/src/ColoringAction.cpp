#include "ColoringAction.h"
#include "Application.h"

#include "ScatterplotPlugin.h"

using namespace hdps::gui;

ColoringAction::ColoringAction(ScatterplotPlugin* scatterplotPlugin) :
    PluginAction(scatterplotPlugin, "Coloring"),
    _colorByAction(this, "Color by"),
    _colorByConstantColorAction(this, "Color by constant color"),
    _colorByDimensionAction(this, "Color by dimension"),
    _colorByColorDataAction(this, "Color by color data"),
    _colorByActionGroup(this),
    _constantColorAction(scatterplotPlugin),
    _colorDimensionAction(scatterplotPlugin),
    _colorDataAction(scatterplotPlugin)
{
    scatterplotPlugin->addAction(&_colorByAction);
    scatterplotPlugin->addAction(&_colorByDimensionAction);
    scatterplotPlugin->addAction(&_colorByColorDataAction);
    scatterplotPlugin->addAction(&_colorDimensionAction);
    scatterplotPlugin->addAction(&_colorDataAction);

    _colorByAction.setOptions(QStringList() << "Constant color" << "Color dimension" << "Color data");

    _colorByAction.setToolTip("Color by");
    _colorByConstantColorAction.setToolTip("Color data points with a constant color");
    _colorByDimensionAction.setToolTip("Color data points by chosen dimension");
    _colorByColorDataAction.setToolTip("Color data points with a color data set");
    _constantColorAction.setToolTip("Constant color");
    _colorDimensionAction.setToolTip("Color dimension");
    _colorDataAction.setToolTip("Color data");

    _colorByConstantColorAction.setCheckable(true);
    _colorByDimensionAction.setCheckable(true);
    _colorByColorDataAction.setCheckable(true);

    _colorDataAction.setEnabled(false);

    _colorByActionGroup.addAction(&_colorByConstantColorAction);
    _colorByActionGroup.addAction(&_colorByDimensionAction);
    _colorByActionGroup.addAction(&_colorByColorDataAction);

    const auto updateColoringMode = [this]() {
        getScatterplotWidget()->setColoringMode(static_cast<ScatterplotWidget::ColoringMode>(_colorByAction.getCurrentIndex()));
    };

    connect(&_colorByAction, &OptionAction::currentIndexChanged, this, [this, updateColoringMode](const std::uint32_t& currentIndex) {
        updateColoringMode();
    });

    connect(&_colorByConstantColorAction, &QAction::triggered, this, [this]() {
        getScatterplotWidget()->setColoringMode(ScatterplotWidget::ColoringMode::ConstantColor);
    });

    connect(&_colorByDimensionAction, &QAction::triggered, this, [this]() {
        getScatterplotWidget()->setColoringMode(ScatterplotWidget::ColoringMode::ColorDimension);
    });

    connect(&_colorByColorDataAction, &QAction::triggered, this, [this]() {
        getScatterplotWidget()->setColoringMode(ScatterplotWidget::ColoringMode::ColorData);
    });

    const auto coloringModeChanged = [this]() -> void {
        const auto coloringMode = getScatterplotWidget()->getColoringMode();

        _colorByAction.setCurrentIndex(static_cast<std::int32_t>(coloringMode));

        _colorByConstantColorAction.setChecked(coloringMode == ScatterplotWidget::ColoringMode::ConstantColor);
        _colorByDimensionAction.setChecked(coloringMode == ScatterplotWidget::ColoringMode::ColorDimension);
        _colorByColorDataAction.setChecked(coloringMode == ScatterplotWidget::ColoringMode::ColorData);
    };

    connect(getScatterplotWidget(), &ScatterplotWidget::coloringModeChanged, this, [this, coloringModeChanged](const ScatterplotWidget::ColoringMode& coloringMode) {
        coloringModeChanged();
    });

    connect(_scatterplotPlugin, &ScatterplotPlugin::currentDatasetChanged, this, [this, coloringModeChanged](const QString& datasetName) {
        coloringModeChanged();
    });

    updateColoringMode();
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
            menu->addMenu(_constantColorAction.getContextMenu());
            break;

        case ScatterplotWidget::ColoringMode::ColorDimension:
            menu->addMenu(_colorDimensionAction.getContextMenu());
            break;

        case ScatterplotWidget::ColoringMode::ColorData:
            menu->addMenu(_colorDataAction.getContextMenu());
            break;

        default:
            break;
    }

    return menu;
}

void ColoringAction::setDimensions(const std::uint32_t& numberOfDimensions, const std::vector<QString>& dimensionNames /*= std::vector<QString>()*/)
{
    _colorDimensionAction.setDimensions(numberOfDimensions, dimensionNames);
}

void ColoringAction::setDimensions(const std::vector<QString>& dimensionNames)
{
    setDimensions(static_cast<std::uint32_t>(dimensionNames.size()), dimensionNames);
}

ColoringAction::Widget::Widget(QWidget* parent, ColoringAction* coloringAction) :
    WidgetAction::Widget(parent, coloringAction),
    _layout(),
    _colorByLabel("Color by:"),
    _colorByWidget(this, &coloringAction->_colorByAction, false),
    _stackedWidget(),
    _constantColorWidget(this, &coloringAction->_constantColorAction),
    _colorDimensionWidget(this, &coloringAction->_colorDimensionAction),
    _colorDataWidget(this, &coloringAction->_colorDataAction)
{
    _layout.addWidget(&_colorByLabel);
    _layout.addWidget(&_colorByWidget);
    _layout.addWidget(&_stackedWidget);

    _stackedWidget.addWidget(&_constantColorWidget);
    _stackedWidget.addWidget(&_colorDimensionWidget);
    _stackedWidget.addWidget(&_colorDataWidget);

    setLayout(&_layout);

    const auto coloringModeChanged = [this, coloringAction]() -> void {
        _stackedWidget.setCurrentIndex(coloringAction->_colorByAction.getCurrentIndex());
    };

    connect(&coloringAction->_colorByAction, &OptionAction::currentIndexChanged, this, [this, coloringModeChanged](const std::uint32_t& currentIndex) {
        coloringModeChanged();
    });

    const auto renderModeChanged = [this, coloringAction]() {
        setEnabled(coloringAction->getScatterplotWidget()->getRenderMode() == ScatterplotWidget::SCATTERPLOT);
    };

    connect(coloringAction->getScatterplotWidget(), &ScatterplotWidget::renderModeChanged, this, [this, renderModeChanged](const ScatterplotWidget::RenderMode& renderMode) {
        renderModeChanged();
    });

    renderModeChanged();
    coloringModeChanged();
}

ColoringAction::PopupWidget::PopupWidget(QWidget* parent, ColoringAction* coloringAction) :
    WidgetAction::PopupWidget(parent, coloringAction),
    _layout(),
    _colorByLabel("Color by:"),
    _colorByWidget(this, &coloringAction->_colorByAction),
    _stackedWidget(),
    _constantColorWidget(this, &coloringAction->_constantColorAction),
    _colorDimensionWidget(this, &coloringAction->_colorDimensionAction),
    _colorDataWidget(this, &coloringAction->_colorDataAction)
{
    _layout.addWidget(&_colorByLabel);
    _layout.addWidget(&_colorByWidget);
    _layout.addWidget(&_stackedWidget);

    _stackedWidget.addWidget(&_constantColorWidget);
    _stackedWidget.addWidget(&_colorDimensionWidget);
    _stackedWidget.addWidget(&_colorDataWidget);

    setLayout(&_layout);

    const auto coloringModeChanged = [this, coloringAction]() -> void {
        _stackedWidget.setCurrentIndex(coloringAction->_colorByAction.getCurrentIndex());
    };

    connect(&coloringAction->_colorByAction, &OptionAction::currentIndexChanged, this, [this, coloringModeChanged](const std::uint32_t& currentIndex) {
        coloringModeChanged();
    });

    coloringModeChanged();
}