#include "SelectionAction.h"
#include "PixelSelectionTool.h"
#include "Application.h"
#include "ScatterplotPlugin.h"
#include "ScatterplotWidget.h"

#include <QHBoxLayout>

using namespace hdps::gui;

SelectionAction::SelectionAction(ScatterplotPlugin* scatterplotPlugin) :
    PluginAction(scatterplotPlugin, "Selection"),
    _typeAction(this, "Type"),
    _rectangleAction(this, "Rectangle"),
    _brushAction(this, "Brush"),
    _lassoAction(this, "Lasso"),
    _polygonAction(this, "Polygon"),
    _typeActionGroup(this),
    _brushRadiusAction(this, "Brush radius", PixelSelectionTool::BRUSH_RADIUS_MIN, PixelSelectionTool::BRUSH_RADIUS_MAX, PixelSelectionTool::BRUSH_RADIUS_DEFAULT, PixelSelectionTool::BRUSH_RADIUS_DEFAULT),
    _modifierAddAction(this, "Add to selection"),
    _modifierRemoveAction(this, "Remove from selection"),
    _modifierActionGroup(this),
    _clearSelectionAction(this, "Select none"),
    _selectAllAction(this, "Select all"),
    _invertSelectionAction(this, "Invert selection"),
    _notifyDuringSelectionAction(this, "Notify during selection")
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

    const auto& fontAwesome = hdps::Application::getIconFont("FontAwesome");

    _modifierAddAction.setIcon(fontAwesome.getIcon("plus"));
    _modifierRemoveAction.setIcon(fontAwesome.getIcon("minus"));

    _typeAction.setOptions(_scatterplotPlugin->getSelectionTool()->types.keys());

    _typeActionGroup.addAction(&_rectangleAction);
    _typeActionGroup.addAction(&_brushAction);
    _typeActionGroup.addAction(&_lassoAction);
    _typeActionGroup.addAction(&_polygonAction);
    
    _brushRadiusAction.setSuffix("px");

    _modifierActionGroup.addAction(&_modifierAddAction);
    _modifierActionGroup.addAction(&_modifierRemoveAction);

    connect(&_typeAction, &OptionAction::currentTextChanged, [this](const QString& currentText) {
        _scatterplotPlugin->getSelectionTool()->setType(PixelSelectionTool::getTypeEnum(currentText));
    });

    connect(&_rectangleAction, &QAction::triggered, this, [this]() {
        _scatterplotPlugin->getSelectionTool()->setType(PixelSelectionTool::Type::Rectangle);
    });

    connect(&_brushAction, &QAction::triggered, this, [this]() {
        _scatterplotPlugin->getSelectionTool()->setType(PixelSelectionTool::Type::Brush);
    });

    connect(&_lassoAction, &QAction::triggered, this, [this]() {
        _scatterplotPlugin->getSelectionTool()->setType(PixelSelectionTool::Type::Lasso);
    });

    connect(&_polygonAction, &QAction::triggered, this, [this]() {
        _scatterplotPlugin->getSelectionTool()->setType(PixelSelectionTool::Type::Polygon);
    });

    const auto updateType = [this]() {
        const auto type = _scatterplotPlugin->getSelectionTool()->getType();

        _typeAction.setCurrentText(PixelSelectionTool::getTypeName(type));
        _rectangleAction.setChecked(type == PixelSelectionTool::Type::Rectangle);
        _brushAction.setChecked(type == PixelSelectionTool::Type::Brush);
        _lassoAction.setChecked(type == PixelSelectionTool::Type::Lasso);
        _polygonAction.setChecked(type == PixelSelectionTool::Type::Polygon);
        _brushRadiusAction.setEnabled(type == PixelSelectionTool::Type::Brush);
    };

    connect(_scatterplotPlugin->getSelectionTool(), &PixelSelectionTool::typeChanged, this, [this, updateType](const PixelSelectionTool::Type& type) {
        updateType();
    });

    updateType();

    connect(_scatterplotPlugin->getSelectionTool(), &PixelSelectionTool::brushRadiusChanged, this, [this](const float& brushRadius) {
        _brushRadiusAction.setValue(brushRadius);
    });

    connect(&_brushRadiusAction, &DoubleAction::valueChanged, this, [this](const double& value) {
        _scatterplotPlugin->getSelectionTool()->setBrushRadius(value);
    });

    connect(&_modifierAddAction, &QAction::toggled, [this](bool checked) {
        _scatterplotPlugin->getSelectionTool()->setModifier(checked ? PixelSelectionTool::Modifier::Add : PixelSelectionTool::Modifier::Replace);
    });

    connect(&_modifierRemoveAction, &QAction::toggled, [this](bool checked) {
        _scatterplotPlugin->getSelectionTool()->setModifier(checked ? PixelSelectionTool::Modifier::Remove : PixelSelectionTool::Modifier::Replace);
    });

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
        _notifyDuringSelectionAction.setChecked(_scatterplotPlugin->getSelectionTool()->isNotifyDuringSelection());
    };
    
    connect(_scatterplotPlugin->getSelectionTool(), &PixelSelectionTool::notifyDuringSelectionChanged, this, [this, updateNotifyDuringSelection](const bool& notifyDuringSelection) {
        updateNotifyDuringSelection();
    });

    updateNotifyDuringSelection();

    connect(&_notifyDuringSelectionAction, &QAction::toggled, [this](bool toggled) {
        _scatterplotPlugin->getSelectionTool()->setNotifyDuringSelection(toggled);
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

    _scatterplotPlugin->installEventFilter(this);
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

    menu->addAction(&_modifierAddAction);
    menu->addAction(&_modifierRemoveAction);

    menu->addSeparator();

    menu->addAction(&_clearSelectionAction);
    menu->addAction(&_selectAllAction);
    menu->addAction(&_invertSelectionAction);

    menu->addSeparator();
    
    menu->addAction(&_notifyDuringSelectionAction);

    return menu;
}

bool SelectionAction::eventFilter(QObject* object, QEvent* event)
{
    const auto keyEvent = dynamic_cast<QKeyEvent*>(event);

    if (!keyEvent)
        return QObject::eventFilter(object, event);

    if (keyEvent->isAutoRepeat())
        return QObject::eventFilter(object, event);

    switch (keyEvent->type())
    {
        case QEvent::KeyPress:
        {
            switch (keyEvent->key())
            {
                case Qt::Key_Shift:
                    _modifierAddAction.setChecked(true);
                    break;

                case Qt::Key_Control:
                    _modifierRemoveAction.setChecked(true);
                    break;

                default:
                    break;
            }

            break;
        }

        case QEvent::KeyRelease:
        {
            switch (keyEvent->key())
            {
                case Qt::Key_Shift:
                    _modifierAddAction.setChecked(false);
                    break;

                case Qt::Key_Control:
                    _modifierRemoveAction.setChecked(false);
                    break;

                default:
                    break;
            }

            break;
        }

        default:
            break;
    }

    return QObject::eventFilter(object, event);
}

SelectionAction::Widget::Widget(QWidget* parent, SelectionAction* selectionAction) :
    WidgetAction::Widget(parent, selectionAction)
{
    auto layout = new QHBoxLayout();

    layout->addWidget(new OptionAction::Widget(this, &selectionAction->_typeAction, false));
    layout->addWidget(new DoubleAction::Widget(this, &selectionAction->_brushRadiusAction, DoubleAction::Widget::Slider));
    layout->addWidget(new StandardAction::PushButton(this, &selectionAction->_modifierAddAction, StandardAction::PushButton::Icon));
    layout->addWidget(new StandardAction::PushButton(this, &selectionAction->_modifierRemoveAction, StandardAction::PushButton::Icon));
    layout->addWidget(new StandardAction::PushButton(this, &selectionAction->_clearSelectionAction));
    layout->addWidget(new StandardAction::PushButton(this, &selectionAction->_selectAllAction));
    layout->addWidget(new StandardAction::PushButton(this, &selectionAction->_invertSelectionAction));
    layout->addWidget(new StandardAction::CheckBox(this, &selectionAction->_notifyDuringSelectionAction));

    setLayout(layout);

    const auto renderModeChanged = [this, selectionAction]() {
        setEnabled(selectionAction->getScatterplotWidget()->getRenderMode() == ScatterplotWidget::SCATTERPLOT);
    };

    connect(selectionAction->getScatterplotWidget(), &ScatterplotWidget::renderModeChanged, this, [this, renderModeChanged](const ScatterplotWidget::RenderMode& renderMode) {
        renderModeChanged();
    });

    renderModeChanged();
}

SelectionAction::PopupWidget::PopupWidget(QWidget* parent, SelectionAction* selectionAction) :
    WidgetAction::PopupWidget(parent, selectionAction)
{
    auto layout = new QGridLayout();

    const auto getTypeWidget = [this, selectionAction]() -> QWidget* {
        auto layout = new QHBoxLayout();

        layout->setMargin(0);

        layout->addWidget(new OptionAction::Widget(this, &selectionAction->_typeAction, false));
        layout->addWidget(new StandardAction::PushButton(this, &selectionAction->_modifierAddAction, StandardAction::PushButton::Icon));
        layout->addWidget(new StandardAction::PushButton(this, &selectionAction->_modifierRemoveAction, StandardAction::PushButton::Icon));

        layout->itemAt(0)->widget()->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

        auto widget = new QWidget();

        widget->setLayout(layout);

        return widget;
    };

    const auto getSelectWidget = [this, selectionAction]() -> QWidget* {
        auto layout = new QHBoxLayout();

        layout->setMargin(0);

        layout->addWidget(new StandardAction::PushButton(this, &selectionAction->_clearSelectionAction));
        layout->addWidget(new StandardAction::PushButton(this, &selectionAction->_selectAllAction));
        layout->addWidget(new StandardAction::PushButton(this, &selectionAction->_invertSelectionAction));
        layout->addStretch(1);

        auto widget = new QWidget();

        widget->setLayout(layout);

        return widget;
    };

    layout->addWidget(new QLabel("Type:"), 0, 0);
    layout->addWidget(getTypeWidget(), 0, 1);
    
    layout->addWidget(new QLabel("Brush radius:"), 1, 0);
    layout->addWidget(new DoubleAction::Widget(this, &selectionAction->_brushRadiusAction), 1, 1);

    layout->addWidget(getSelectWidget(), 2, 1);

    layout->addWidget(new StandardAction::CheckBox(this, &selectionAction->_notifyDuringSelectionAction), 3, 1);

    layout->itemAtPosition(1, 1)->widget()->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    setLayout(layout);
}