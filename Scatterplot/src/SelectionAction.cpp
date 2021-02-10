#include "SelectionAction.h"
#include "Application.h"

#include "ScatterplotPlugin.h"

#include "widgets/ActionPushButton.h"
#include "widgets/ActionCheckBox.h"

using namespace hdps::gui;

SelectionAction::SelectionAction(ScatterplotPlugin* scatterplotPlugin) :
    PluginAction(scatterplotPlugin),
    _typeAction(this, "Type"),
    _rectangleAction("Rectangle"),
    _brushAction("Brush"),
    _lassoAction("Lasso"),
    _polygonAction("Polygon"),
    _typeActionGroup(this),
    _brushRadiusAction(this, "Brush radius", PixelSelectionTool::BRUSH_RADIUS_MIN, PixelSelectionTool::BRUSH_RADIUS_MAX, PixelSelectionTool::BRUSH_RADIUS_DEFAULT),
    _modifierAddAction(this),
    _modifierRemoveAction(""),
    _clearSelectionAction("Select none"),
    _selectAllAction("Select all"),
    _invertSelectionAction("Invert selection"),
    _notifyDuringSelectionAction("Notify during selection")
{
    scatterplotPlugin->addAction(&_rectangleAction);
    scatterplotPlugin->addAction(&_brushAction);
    scatterplotPlugin->addAction(&_lassoAction);
    scatterplotPlugin->addAction(&_polygonAction);
    scatterplotPlugin->addAction(&_modifierAddAction);
    scatterplotPlugin->addAction(&_modifierRemoveAction);
    scatterplotPlugin->addAction(&_clearSelectionAction);
    scatterplotPlugin->addAction(&_selectAllAction);
    scatterplotPlugin->addAction(&_invertSelectionAction);
    scatterplotPlugin->addAction(&_notifyDuringSelectionAction);

    _rectangleAction.setCheckable(true);
    _brushAction.setCheckable(true);
    _lassoAction.setCheckable(true);
    _polygonAction.setCheckable(true);
    _modifierAddAction.setCheckable(true);
    _modifierRemoveAction.setCheckable(true);
    _notifyDuringSelectionAction.setCheckable(true);

    _rectangleAction.setShortcut(QKeySequence("R"));
    _brushAction.setShortcut(QKeySequence("B"));
    _lassoAction.setShortcut(QKeySequence("L"));
    _polygonAction.setShortcut(QKeySequence("P"));
    _modifierAddAction.setShortcut(QKeySequence("Shift"));
    _modifierRemoveAction.setShortcut(QKeySequence(Qt::Key_Control));
    _clearSelectionAction.setShortcut(QKeySequence("E"));
    _selectAllAction.setShortcut(QKeySequence("A"));
    _invertSelectionAction.setShortcut(QKeySequence("I"));
    _notifyDuringSelectionAction.setShortcut(QKeySequence("U"));

    _rectangleAction.setToolTip("Select data points inside a rectangle");
    _brushAction.setToolTip("Select data points using a brush tool");
    _lassoAction.setToolTip("Select data points using a lasso");
    _polygonAction.setToolTip("Select data points by drawing a polygon");
    _brushRadiusAction.setToolTip("Brush selection tool radius");
    _modifierAddAction.setToolTip("Add items to the existing selection");
    _modifierRemoveAction.setToolTip("Remove items from the existing selection");
    _clearSelectionAction.setToolTip("Clears the selection");
    _selectAllAction.setToolTip("Select all data points");
    _invertSelectionAction.setToolTip("Invert the selection");
    _notifyDuringSelectionAction.setToolTip("Notify during selection or only at the end of the selection process");

    /*
    const auto& fontAwesome = Application::getIconFont("FontAwesome");

    _modifierAddAction.setIcon(fontAwesome.getIcon("plus"));
    _modifierRemoveAction.setIcon(fontAwesome.getIcon("minus"));
    */

    _typeAction.setOptions(_scatterplotPlugin->getSelectionTool().types.keys());

    _typeActionGroup.addAction(&_rectangleAction);
    _typeActionGroup.addAction(&_brushAction);
    _typeActionGroup.addAction(&_lassoAction);
    _typeActionGroup.addAction(&_polygonAction);
    
    _brushRadiusAction.setSuffix("px");

    connect(&_typeAction, &OptionAction::currentTextChanged, [this](const QString& currentText) {
        _scatterplotPlugin->getSelectionTool().setType(PixelSelectionTool::getTypeEnum(currentText));
    });

    connect(&_rectangleAction, &QAction::triggered, this, [this]() {
        _scatterplotPlugin->getSelectionTool().setType(PixelSelectionTool::Type::Rectangle);
    });

    connect(&_brushAction, &QAction::triggered, this, [this]() {
        _scatterplotPlugin->getSelectionTool().setType(PixelSelectionTool::Type::Brush);
    });

    connect(&_lassoAction, &QAction::triggered, this, [this]() {
        _scatterplotPlugin->getSelectionTool().setType(PixelSelectionTool::Type::Lasso);
    });

    connect(&_polygonAction, &QAction::triggered, this, [this]() {
        _scatterplotPlugin->getSelectionTool().setType(PixelSelectionTool::Type::Polygon);
    });

    const auto updateType = [this]() {
        const auto type = _scatterplotPlugin->getSelectionTool().getType();

        _typeAction.setCurrentText(PixelSelectionTool::getTypeName(type));
        _rectangleAction.setChecked(type == PixelSelectionTool::Type::Rectangle);
        _brushAction.setChecked(type == PixelSelectionTool::Type::Brush);
        _lassoAction.setChecked(type == PixelSelectionTool::Type::Lasso);
        _polygonAction.setChecked(type == PixelSelectionTool::Type::Polygon);
        _brushRadiusAction.setEnabled(type == PixelSelectionTool::Type::Brush);
    };

    connect(&_scatterplotPlugin->getSelectionTool(), &PixelSelectionTool::typeChanged, this, [this, updateType](const PixelSelectionTool::Type& type) {
        updateType();
    });

    updateType();

    connect(&_scatterplotPlugin->getSelectionTool(), &PixelSelectionTool::brushRadiusChanged, this, [this](const float& brushRadius) {
        _brushRadiusAction.setValue(brushRadius);
    });

    connect(&_brushRadiusAction, &DoubleAction::valueChanged, this, [this](const double& value) {
        _scatterplotPlugin->getSelectionTool().setBrushRadius(value);
    });

    /*
    connect(_modifierAddPushButton, &QPushButton::toggled, [this, &pixelSelectionTool](bool checked) {
        pixelSelectionTool.setModifier(checked ? PixelSelectionTool::Modifier::Add : PixelSelectionTool::Modifier::Replace);
    });

    connect(_modifierRemovePushButton, &QPushButton::toggled, [this, &pixelSelectionTool](bool checked) {
        pixelSelectionTool.setModifier(checked ? PixelSelectionTool::Modifier::Remove : PixelSelectionTool::Modifier::Replace);
    });
    */

    connect(&_selectAllAction, &QAction::triggered, [this]() {
        _scatterplotPlugin->selectAll();
    });

    connect(&_clearSelectionAction, &QAction::triggered, [this]() {
        _scatterplotPlugin->clearSelection();
    });

    connect(&_invertSelectionAction, &QAction::triggered, [this]() {
        _scatterplotPlugin->invertSelection();
    });

    const auto updateNotifyDuringSelection = [this]() -> void {
        _notifyDuringSelectionAction.setChecked(_scatterplotPlugin->getSelectionTool().isNotifyDuringSelection());
    };
    
    connect(&_scatterplotPlugin->getSelectionTool(), &PixelSelectionTool::notifyDuringSelectionChanged, this, [this, updateNotifyDuringSelection](const bool& notifyDuringSelection) {
        updateNotifyDuringSelection();
    });

    updateNotifyDuringSelection();

    connect(&_notifyDuringSelectionAction, &QAction::toggled, [this](bool toggled) {
        _scatterplotPlugin->getSelectionTool().setNotifyDuringSelection(toggled);
    });

    const auto updateSelectionButtons = [this]() {
        _clearSelectionAction.setEnabled(_scatterplotPlugin->canClearSelection());
        _selectAllAction.setEnabled(_scatterplotPlugin->canSelectAll());
        _invertSelectionAction.setEnabled(_scatterplotPlugin->canInvertSelection());
    };

    connect(_scatterplotPlugin, qOverload<>(&ScatterplotPlugin::selectionChanged), this, [this, updateSelectionButtons]() {
        updateSelectionButtons();
    });

    updateSelectionButtons();

    const auto updateRenderMode = [this]() -> void {
        setEnabled(getScatterplotWidget()->getRenderMode() == ScatterplotWidget::SCATTERPLOT);
    };

    connect(getScatterplotWidget(), &ScatterplotWidget::renderModeChanged, this, [this, updateRenderMode](const ScatterplotWidget::RenderMode& renderMode) {
        updateRenderMode();
    });

    updateRenderMode();
}

QMenu* SelectionAction::getContextMenu()
{
    auto menu = new QMenu("Selection");

    const auto addActionToMenu = [menu](QAction* action) -> void {
        auto actionMenu = new QMenu(action->text());

        actionMenu->addAction(action);

        menu->addMenu(actionMenu);
    };

    menu->setEnabled(_scatterplotPlugin->getScatterplotWidget()->getRenderMode() == ScatterplotWidget::RenderMode::SCATTERPLOT);

    menu->addAction(&_rectangleAction);
    menu->addAction(&_brushAction);
    menu->addAction(&_lassoAction);
    menu->addAction(&_polygonAction);

    menu->addSeparator();

    addActionToMenu(&_brushRadiusAction);

    menu->addSeparator();

    menu->addAction(&_clearSelectionAction);
    menu->addAction(&_selectAllAction);
    menu->addAction(&_invertSelectionAction);

    menu->addSeparator();
    
    menu->addAction(&_notifyDuringSelectionAction);

    return menu;
}

SelectionAction::Widget::Widget(QWidget* parent, SelectionAction* selectionAction) :
    WidgetAction::Widget(parent, selectionAction),
    _layout()
{
    
    _layout.addWidget(selectionAction->_typeAction.createWidget(this));
    _layout.addWidget(selectionAction->_brushRadiusAction.createWidget(this));
    _layout.addWidget(new ActionPushButton(&selectionAction->_clearSelectionAction));
    _layout.addWidget(new ActionPushButton(&selectionAction->_selectAllAction));
    _layout.addWidget(new ActionPushButton(&selectionAction->_invertSelectionAction));
    _layout.addWidget(new ActionCheckBox(&selectionAction->_notifyDuringSelectionAction));
    /*
    auto popupLayout = new QGridLayout();

    auto typeWidget = new QWidget();

    auto typeWidgetLayout = new QHBoxLayout();

    typeWidgetLayout->addWidget(selectionAction->_typeAction.createWidget(this));
    typeWidgetLayout->addWidget(new ActionPushButton(&selectionAction->_modifierAddAction));
    typeWidgetLayout->addWidget(new ActionPushButton(&selectionAction->_modifierRemoveAction));

    typeWidget->setLayout(typeWidgetLayout);

    popupLayout->addWidget(new QLabel("Type:"), 0, 0);
    popupLayout->addWidget(typeWidget, 0, 1);

    popupLayout->addWidget(new QLabel("Brush radius:"), 1, 0);
    popupLayout->addWidget(selectionAction->_brushRadiusAction.createWidget(this), 1, 1);

    auto selectWidget = new QWidget();

    auto selectWidgetLayout = new QHBoxLayout();

    selectWidgetLayout->addWidget(new ActionPushButton(&selectionAction->_clearSelectionAction));
    selectWidgetLayout->addWidget(new ActionPushButton(&selectionAction->_selectAllAction));
    selectWidgetLayout->addWidget(new ActionPushButton(&selectionAction->_invertSelectionAction));

    selectWidget->setLayout(selectWidgetLayout);

    popupLayout->addWidget(new QLabel("Select:"), 2, 0);
    popupLayout->addWidget(selectWidget, 2, 1);

    popupLayout->addWidget(selectWidget, 3, 1);

    _popupWidget.setContentLayout(popupLayout);

    _popupWidgetAction.setDefaultWidget(&_popupWidget);

    _toolButton.setPopupMode(QToolButton::InstantPopup);
    _toolButton.addAction(&_popupWidgetAction);
    */

    setLayout(&_layout);
}