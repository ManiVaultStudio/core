#include "PixelSelectionAction.h"

#include "Application.h"

#include "util/PixelSelectionTool.h"

#include <QKeyEvent>

using namespace hdps::util;

namespace hdps {

namespace gui {

PixelSelectionAction::PixelSelectionAction(QObject* parent, QWidget* targetWidget, PixelSelectionTool& pixelSelectionTool, const PixelSelectionTypes& pixelSelectionTypes /*= util::defaultPixelSelectionTypes*/) :
    WidgetAction(parent),
    _targetWidget(targetWidget),
    _pixelSelectionTool(pixelSelectionTool),
    _pixelSelectionTypes(pixelSelectionTypes),
    _overlayColor(this, "Overlay color", QColor(255, 0, 0), QColor(255, 0, 0)),
    _overlayOpacity(this, "Overlay opacity", 0.0f, 100.0f, 75.0f, 75.0f, 1),
    _typeAction(this, "Type"),
    _rectangleAction(this, "Rectangle"),
    _brushAction(this, "Brush"),
    _lassoAction(this, "Lasso"),
    _polygonAction(this, "Polygon"),
    _sampleAction(this, "Sample"),
    _typeActionGroup(this),
    _modifierReplaceAction(this, "Replace"),
    _modifierAddAction(this, "Add"),
    _modifierSubtractAction(this, "Subtract"),
    _modifierActionGroup(this),
    _clearSelectionAction(this, "None"),
    _selectAllAction(this, "All"),
    _invertSelectionAction(this, "Invert"),
    _brushRadiusAction(this, "Brush radius", PixelSelectionTool::BRUSH_RADIUS_MIN, PixelSelectionTool::BRUSH_RADIUS_MAX, PixelSelectionTool::BRUSH_RADIUS_DEFAULT, PixelSelectionTool::BRUSH_RADIUS_DEFAULT),
    _notifyDuringSelectionAction(this, "Notify during selection", true, true)
{
    setText("Selection");
    setIcon(hdps::Application::getIconFont("FontAwesome").getIcon("mouse-pointer"));

    initOverlay();
    initType();
    initModifier();
    initOperations();
    initMiscellaneous();

    setShortcutsEnabled(true);

    _targetWidget->installEventFilter(this);
}

QWidget* PixelSelectionAction::getTargetWidget()
{
    return _targetWidget;
}

PixelSelectionTool& PixelSelectionAction::getPixelSelectionTool()
{
    return _pixelSelectionTool;
}

PixelSelectionTypes PixelSelectionAction::getAllowedTypes() const
{
    return _pixelSelectionTypes;
}

void PixelSelectionAction::setAllowedTypes(const util::PixelSelectionTypes& pixelSelectionTypes)
{
    _pixelSelectionTypes = pixelSelectionTypes;
}

void PixelSelectionAction::setShortcutsEnabled(const bool& shortcutsEnabled)
{
    if (shortcutsEnabled) {
        _targetWidget->addAction(&_rectangleAction);
        _targetWidget->addAction(&_brushAction);
        _targetWidget->addAction(&_lassoAction);
        _targetWidget->addAction(&_polygonAction);
        _targetWidget->addAction(&_sampleAction);
        _targetWidget->addAction(&_modifierAddAction);
        _targetWidget->addAction(&_modifierSubtractAction);
        _targetWidget->addAction(&_clearSelectionAction);
        _targetWidget->addAction(&_selectAllAction);
        _targetWidget->addAction(&_invertSelectionAction);
        _targetWidget->addAction(&_brushRadiusAction);
        _targetWidget->addAction(&_notifyDuringSelectionAction);
    }
    else {
        _targetWidget->removeAction(&_rectangleAction);
        _targetWidget->removeAction(&_brushAction);
        _targetWidget->removeAction(&_lassoAction);
        _targetWidget->removeAction(&_polygonAction);
        _targetWidget->removeAction(&_sampleAction);
        _targetWidget->removeAction(&_modifierAddAction);
        _targetWidget->removeAction(&_modifierSubtractAction);
        _targetWidget->removeAction(&_clearSelectionAction);
        _targetWidget->removeAction(&_selectAllAction);
        _targetWidget->removeAction(&_invertSelectionAction);
        _targetWidget->removeAction(&_brushRadiusAction);
        _targetWidget->removeAction(&_notifyDuringSelectionAction);
    }
}

void PixelSelectionAction::initOverlay()
{
    _overlayOpacity.setSuffix("%");
}

void PixelSelectionAction::initType()
{
    auto targetWidget = getTargetWidget();

    auto& pixelSelectionTool = getPixelSelectionTool();

    _rectangleAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);
    _brushAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);
    _lassoAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);
    _polygonAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);
    _sampleAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);

    _rectangleAction.setShortcut(QKeySequence("R"));
    _brushAction.setShortcut(QKeySequence("B"));
    _lassoAction.setShortcut(QKeySequence("L"));
    _polygonAction.setShortcut(QKeySequence("P"));
    _sampleAction.setShortcut(QKeySequence("S"));

    _rectangleAction.setIcon(PixelSelectionTool::getIcon(PixelSelectionType::Rectangle));
    _brushAction.setIcon(PixelSelectionTool::getIcon(PixelSelectionType::Brush));
    _lassoAction.setIcon(PixelSelectionTool::getIcon(PixelSelectionType::Lasso));
    _polygonAction.setIcon(PixelSelectionTool::getIcon(PixelSelectionType::Polygon));
    _sampleAction.setIcon(PixelSelectionTool::getIcon(PixelSelectionType::Sample));

    _rectangleAction.setToolTip("Select pixels inside a rectangle (R)");
    _brushAction.setToolTip("Select pixels using a brush tool (B)");
    _lassoAction.setToolTip("Select pixels using a lasso (L)");
    _polygonAction.setToolTip("Select pixels by drawing a polygon (P)");
    _sampleAction.setToolTip("Sample pixel by dragging over the image (S)");

    _typeAction.setOptions(util::pixelSelectionTypes.values());
    _typeAction.setCurrentText("Rectangle");
    _typeAction.setDefaultText("Rectangle");

    _typeActionGroup.addAction(&_rectangleAction);
    _typeActionGroup.addAction(&_brushAction);
    _typeActionGroup.addAction(&_lassoAction);
    _typeActionGroup.addAction(&_polygonAction);
    _typeActionGroup.addAction(&_sampleAction);

    connect(&_typeAction, &OptionAction::currentTextChanged, [this, &pixelSelectionTool](const QString& currentText) {
        pixelSelectionTool.setType(util::pixelSelectionTypes.key(currentText));
    });

    connect(&_rectangleAction, &QAction::toggled, this, [this, &pixelSelectionTool](bool toggled) {
        if (toggled)
            pixelSelectionTool.setType(PixelSelectionType::Rectangle);
    });

    connect(&_brushAction, &QAction::toggled, this, [this, &pixelSelectionTool](bool toggled) {
        if (toggled)
            pixelSelectionTool.setType(PixelSelectionType::Brush);
    });

    connect(&_lassoAction, &QAction::toggled, this, [this, &pixelSelectionTool](bool toggled) {
        if (toggled)
            pixelSelectionTool.setType(PixelSelectionType::Lasso);
    });

    connect(&_polygonAction, &QAction::toggled, this, [this, &pixelSelectionTool](bool toggled) {
        if (toggled)
            pixelSelectionTool.setType(PixelSelectionType::Polygon);
    });

    connect(&_sampleAction, &QAction::toggled, this, [this, &pixelSelectionTool](bool toggled) {
        if (toggled)
            pixelSelectionTool.setType(PixelSelectionType::Sample);
    });

    const auto updateType = [this, &pixelSelectionTool]() {
        const auto type = pixelSelectionTool.getType();

        _typeAction.setCurrentText(util::pixelSelectionTypes.value(type));

        _rectangleAction.setChecked(type == PixelSelectionType::Rectangle);
        _brushAction.setChecked(type == PixelSelectionType::Brush);
        _lassoAction.setChecked(type == PixelSelectionType::Lasso);
        _polygonAction.setChecked(type == PixelSelectionType::Polygon);
        _sampleAction.setChecked(type == PixelSelectionType::Sample);

        setResettable(_typeAction.isResettable());
    };

    connect(&pixelSelectionTool, &PixelSelectionTool::typeChanged, this, updateType);

    updateType();
}

void PixelSelectionAction::initModifier()
{
    _modifierAddAction.setDefaultWidgetFlags(ToggleAction::PushButton);
    _modifierSubtractAction.setDefaultWidgetFlags(ToggleAction::PushButton);

    _modifierAddAction.setCheckable(true);
    _modifierSubtractAction.setCheckable(true);

    const auto& fontAwesome = hdps::Application::getIconFont("FontAwesome");

    _modifierAddAction.setIcon(fontAwesome.getIcon("plus"));
    _modifierSubtractAction.setIcon(fontAwesome.getIcon("minus"));

    _modifierAddAction.setToolTip("Add items to the existing selection");
    _modifierSubtractAction.setToolTip("Remove items from the existing selection");

    _modifierActionGroup.addAction(&_modifierAddAction);
    _modifierActionGroup.addAction(&_modifierSubtractAction);

    connect(&_modifierAddAction, &QAction::toggled, [this](bool checked) {
        _pixelSelectionTool.setModifier(checked ? PixelSelectionModifierType::Add : PixelSelectionModifierType::Replace);
    });

    connect(&_modifierSubtractAction, &QAction::toggled, [this](bool checked) {
        _pixelSelectionTool.setModifier(checked ? PixelSelectionModifierType::Remove : PixelSelectionModifierType::Replace);
    });

    const auto updateResettable = [this]() -> void {
        setResettable(_modifierAddAction.isResettable() || _modifierSubtractAction.isResettable());
    };

    connect(&_modifierAddAction, &ColorAction::resettableChanged, this, updateResettable);
    connect(&_modifierSubtractAction, &DecimalAction::resettableChanged, this, updateResettable);

    updateResettable();
}

void PixelSelectionAction::initOperations()
{
    _clearSelectionAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);
    _selectAllAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);
    _invertSelectionAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);

    _clearSelectionAction.setShortcut(QKeySequence("E"));
    _selectAllAction.setShortcut(QKeySequence("A"));
    _invertSelectionAction.setShortcut(QKeySequence("I"));

    _clearSelectionAction.setToolTip("Clears the selection (E)");
    _selectAllAction.setToolTip("Select all data points (A)");
    _invertSelectionAction.setToolTip("Invert the selection (I)");
}

void PixelSelectionAction::initMiscellaneous()
{
    _notifyDuringSelectionAction.setDefaultWidgetFlags(ToggleAction::CheckBox);

    _notifyDuringSelectionAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);

    _notifyDuringSelectionAction.setShortcut(QKeySequence("U"));

    _brushRadiusAction.setToolTip("Brush selection tool radius");

    const auto& fontAwesome = hdps::Application::getIconFont("FontAwesome");

    _brushRadiusAction.setSuffix("px");

    connect(&_pixelSelectionTool, &PixelSelectionTool::brushRadiusChanged, this, [this](const float& brushRadius) {
        _brushRadiusAction.setValue(brushRadius);
    });

    connect(&_brushRadiusAction, &DecimalAction::valueChanged, this, [this](const double& value) {
        _pixelSelectionTool.setBrushRadius(value);
    });

    _notifyDuringSelectionAction.setToolTip("Notify during selection or only at the end of the selection process (U)");

    const auto updateNotifyDuringSelection = [this]() -> void {
        _notifyDuringSelectionAction.setChecked(_pixelSelectionTool.isNotifyDuringSelection());
    };

    connect(&_pixelSelectionTool, &PixelSelectionTool::notifyDuringSelectionChanged, this, [this, updateNotifyDuringSelection](const bool& notifyDuringSelection) {
        updateNotifyDuringSelection();
    });

    updateNotifyDuringSelection();

    connect(&_notifyDuringSelectionAction, &QAction::toggled, [this](bool toggled) {
        _pixelSelectionTool.setNotifyDuringSelection(toggled);
    });

    const auto updateType = [this]() {
        _brushRadiusAction.setEnabled(_pixelSelectionTool.getType() == PixelSelectionType::Brush);
    };

    connect(&_pixelSelectionTool, &PixelSelectionTool::typeChanged, this, updateType);

    updateType();
}

QMenu* PixelSelectionAction::getContextMenu()
{
    auto menu = new QMenu("Selection");

    const auto addActionToMenu = [menu](QAction* action) -> void {
        auto actionMenu = new QMenu(action->text());

        actionMenu->addAction(action);

        menu->addMenu(actionMenu);
    };

    if (pixelSelectionTypes.contains(PixelSelectionType::Rectangle))
        menu->addAction(&_rectangleAction);

    if (pixelSelectionTypes.contains(PixelSelectionType::Brush))
        menu->addAction(&_brushAction);

    if (pixelSelectionTypes.contains(PixelSelectionType::Lasso))
        menu->addAction(&_lassoAction);

    if (pixelSelectionTypes.contains(PixelSelectionType::Polygon))
        menu->addAction(&_polygonAction);

    if (pixelSelectionTypes.contains(PixelSelectionType::Sample))
        menu->addAction(&_sampleAction);

    menu->addSeparator();

    addActionToMenu(&_brushRadiusAction);

    menu->addSeparator();

    menu->addAction(&_modifierAddAction);
    menu->addAction(&_modifierSubtractAction);

    menu->addSeparator();

    menu->addAction(&_selectAllAction);
    menu->addAction(&_clearSelectionAction);
    menu->addAction(&_invertSelectionAction);

    menu->addSeparator();

    menu->addAction(&_notifyDuringSelectionAction);

    return menu;
}

bool PixelSelectionAction::eventFilter(QObject* object, QEvent* event)
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
                    _modifierSubtractAction.setChecked(true);
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
                    _modifierSubtractAction.setChecked(false);
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

}
}
