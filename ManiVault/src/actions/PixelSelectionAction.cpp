// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PixelSelectionAction.h"
#include "Application.h"
#include "CoreInterface.h"

#include <QKeyEvent>
#include <QMenu>

using namespace mv::util;

namespace mv::gui {

PixelSelectionAction::PixelSelectionAction(QObject* parent, const QString& title) :
    GroupAction(parent, title),
    _initialized(false),
    _targetWidget(nullptr),
    _pixelSelectionTool(nullptr),
    _pixelSelectionTypes(),
    _overlayColorAction(this, "Overlay color", QColor(255, 0, 0)),
    _overlayOpacityAction(this, "Overlay opacity", 0.0f, 100.0f, 75.0f, 1),
    _typeModel(this),
    _typeAction(this, "Type"),
    _rectangleAction(this, "Rectangle"),
    _brushAction(this, "Brush"),
    _lassoAction(this, "Lasso"),
    _polygonAction(this, "Polygon"),
    _sampleAction(this, "Sample"),
    _roiAction(this, "ROI"),
    _typeActionGroup(this),
    _modifierAction(this, "Modifier", { "Replace", "Add", "Subtract" }),
    _modifierReplaceAction(this, "Replace"),
    _modifierAddAction(this, "Add"),
    _modifierSubtractAction(this, "Subtract"),
    _selectAction(this, "Select"),
    _clearSelectionAction(this, "None"),
    _selectAllAction(this, "All"),
    _invertSelectionAction(this, "Invert"),
    _brushRadiusAction(this, "Brush radius", PixelSelectionTool::BRUSH_RADIUS_MIN, PixelSelectionTool::BRUSH_RADIUS_MAX, PixelSelectionTool::BRUSH_RADIUS_DEFAULT),
    _notifyDuringSelectionAction(this, "Notify during selection", true)
{
    setIconByName("mouse-pointer");

    addAction(&_overlayColorAction);
    addAction(&_overlayOpacityAction);
    addAction(&_typeAction);
    addAction(&_modifierAction);
    addAction(&_selectAction);
    addAction(&_brushRadiusAction);
    addAction(&_notifyDuringSelectionAction);

    _overlayColorAction.setToolTip("Selection color");

    _overlayOpacityAction.setSuffix("%");
    _overlayOpacityAction.setToolTip("Selection opacity");

    _typeAction.setCustomModel(&_typeModel);

    _rectangleAction.setIcon(getPixelSelectionTypeIcon(PixelSelectionType::Rectangle));
    _rectangleAction.setToolTip("Select pixels inside a rectangle (R)");
    _rectangleAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);
    _rectangleAction.setShortcut(QKeySequence("R"));

    _brushAction.setIcon(getPixelSelectionTypeIcon(PixelSelectionType::Brush));
    _brushAction.setToolTip("Select pixels using a brush tool (B)");
    _brushAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);
    _brushAction.setShortcut(QKeySequence("B"));

    _lassoAction.setIcon(getPixelSelectionTypeIcon(PixelSelectionType::Lasso));
    _lassoAction.setToolTip("Select pixels using a lasso (L)");
    _lassoAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);
    _lassoAction.setShortcut(QKeySequence("L"));

    _polygonAction.setIcon(getPixelSelectionTypeIcon(PixelSelectionType::Polygon));
    _polygonAction.setToolTip("Select pixels by drawing a polygon (P)");
    _polygonAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);
    _polygonAction.setShortcut(QKeySequence("P"));

    _sampleAction.setIcon(getPixelSelectionTypeIcon(PixelSelectionType::Sample));
    _sampleAction.setToolTip("Sample pixel by dragging over the image (S)");
    _sampleAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);
    _sampleAction.setShortcut(QKeySequence("S"));

    _roiAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);
    _roiAction.setIcon(getPixelSelectionTypeIcon(PixelSelectionType::ROI));
    _roiAction.setToolTip("Sample within region of interest (I)");
    _roiAction.setShortcut(QKeySequence("I"));
    
    _modifierAction.setToolTip("Type of selection modifier");
    _modifierAction.setCurrentIndex(static_cast<std::int32_t>(PixelSelectionModifierType::Replace));

    _modifierReplaceAction.setIconByName("exchange-alt");
    _modifierReplaceAction.setCheckable(true);
    _modifierReplaceAction.setDefaultWidgetFlags(ToggleAction::PushButton);

    _modifierAddAction.setIconByName("plus");
    _modifierAddAction.setToolTip("Add items to the existing selection");
    _modifierAddAction.setCheckable(true);
    _modifierAddAction.setDefaultWidgetFlags(ToggleAction::PushButton);

    _modifierSubtractAction.setIconByName("minus");
    _modifierSubtractAction.setToolTip("Remove items from the existing selection");
    _modifierSubtractAction.setDefaultWidgetFlags(ToggleAction::PushButton);
    _modifierSubtractAction.setCheckable(true);

    _selectAction.setDefaultWidgetFlags(GroupAction::Horizontal);

    _selectAction.addAction(&_clearSelectionAction);
    _selectAction.addAction(&_selectAllAction);
    _selectAction.addAction(&_invertSelectionAction);

    _clearSelectionAction.setToolTip("Clears the selection (E)");
    _clearSelectionAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);
    _clearSelectionAction.setShortcut(QKeySequence("E"));

    _selectAllAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);
    _selectAllAction.setShortcut(QKeySequence("A"));
    _selectAllAction.setToolTip("Select all data points (A)");

    _invertSelectionAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);
    _invertSelectionAction.setShortcut(QKeySequence("I"));
    _invertSelectionAction.setToolTip("Invert the selection (I)");

    _brushRadiusAction.setToolTip("Brush selection tool radius");
    _brushRadiusAction.setSuffix("px");

    _notifyDuringSelectionAction.setDefaultWidgetFlags(ToggleAction::CheckBox);
    _notifyDuringSelectionAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);
    _notifyDuringSelectionAction.setShortcut(QKeySequence("U"));
    _notifyDuringSelectionAction.setToolTip("Notify during selection or only at the end of the selection process (U)");

    const auto updatePixelSelectionTypesModel = [this]() {
        PixelSelectionTypes types;

        for (auto type : _typeAction.getOptions()) {
            if (!pixelSelectionTypes.values().contains(type))
                continue;

            types << pixelSelectionTypes.key(type);
        }

        _typeModel.setPixelSelectionTypes(types);
    };

    updatePixelSelectionTypesModel();

    connect(&_typeAction, &OptionAction::modelChanged, this, updatePixelSelectionTypesModel);

    connect(&_rectangleAction, &QAction::toggled, this, [this](bool toggled) {
        if (toggled)
            _typeAction.setCurrentIndex(static_cast<std::int32_t>(PixelSelectionType::Rectangle));
    });

    connect(&_brushAction, &QAction::toggled, this, [this](bool toggled) {
        if (toggled)
            _typeAction.setCurrentIndex(static_cast<std::int32_t>(PixelSelectionType::Brush));
    });

    connect(&_lassoAction, &QAction::toggled, this, [this](bool toggled) {
        if (toggled)
            _typeAction.setCurrentIndex(static_cast<std::int32_t>(PixelSelectionType::Lasso));
    });

    connect(&_polygonAction, &QAction::toggled, this, [this](bool toggled) {
        if (toggled)
            _typeAction.setCurrentIndex(static_cast<std::int32_t>(PixelSelectionType::Polygon));
    });

    connect(&_sampleAction, &QAction::toggled, this, [this](bool toggled) {
        if (toggled)
            _typeAction.setCurrentIndex(static_cast<std::int32_t>(PixelSelectionType::Sample));
    });

    connect(&_roiAction, &QAction::toggled, this, [this](bool toggled) {
        if (toggled)
            _typeAction.setCurrentIndex(static_cast<std::int32_t>(PixelSelectionType::ROI));
    });

    const auto updateType = [this]() {
        _brushRadiusAction.setEnabled(_typeAction.getCurrentIndex() == static_cast<std::int32_t>(PixelSelectionType::Brush) || _typeAction.getCurrentIndex() == static_cast<std::int32_t>(PixelSelectionType::Sample));
    };

    updateType();

    connect(&_typeAction, &OptionAction::currentTextChanged, this, updateType);
}

void PixelSelectionAction::initialize(QWidget* targetWidget, util::PixelSelectionTool* pixelSelectionTool, const util::PixelSelectionTypes& pixelSelectionTypes /*= util::defaultPixelSelectionTypes*/)
{
    Q_ASSERT(targetWidget != nullptr);
    Q_ASSERT(pixelSelectionTool != nullptr);

    if (targetWidget == nullptr || pixelSelectionTool == nullptr)
        return;

    _targetWidget           = targetWidget;
    _pixelSelectionTool     = pixelSelectionTool;
    _pixelSelectionTypes    = pixelSelectionTypes;

    _typeModel.setPixelSelectionTypes(pixelSelectionTypes);

    initType();
    initModifier();
    initMiscellaneous();

    setShortcutsEnabled(true);

    _targetWidget->installEventFilter(this);
    _initialized = true;
}

QWidget* PixelSelectionAction::getTargetWidget()
{
    return _targetWidget;
}

PixelSelectionTool* PixelSelectionAction::getPixelSelectionTool()
{
    return _pixelSelectionTool;
}

void PixelSelectionAction::setShortcutsEnabled(const bool& shortcutsEnabled)
{
    if (!isInitialized())
        return;

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

void PixelSelectionAction::initType()
{
    if (_typeModel.rowCount() > 0)
        _typeAction.setCurrentIndex(0);

    if (_pixelSelectionTypes.contains(PixelSelectionType::Rectangle))
        _typeActionGroup.addAction(&_rectangleAction);

    if (_pixelSelectionTypes.contains(PixelSelectionType::Brush))
        _typeActionGroup.addAction(&_brushAction);

    if (_pixelSelectionTypes.contains(PixelSelectionType::Lasso))
        _typeActionGroup.addAction(&_lassoAction);

    if (_pixelSelectionTypes.contains(PixelSelectionType::Polygon))
        _typeActionGroup.addAction(&_polygonAction);

    if (_pixelSelectionTypes.contains(PixelSelectionType::Sample))
        _typeActionGroup.addAction(&_sampleAction);

    if (_pixelSelectionTypes.contains(PixelSelectionType::ROI))
        _typeActionGroup.addAction(&_roiAction);

    connect(&_typeAction, &OptionAction::currentTextChanged, [this](const QString& currentText) {
        _pixelSelectionTool->setType(util::pixelSelectionTypes.key(currentText));
    });

    const auto updateType = [this]() {
        const auto type = _pixelSelectionTool->getType();

        _typeAction.setCurrentText(util::pixelSelectionTypes.value(type));

        _rectangleAction.setChecked(type == PixelSelectionType::Rectangle);
        _brushAction.setChecked(type == PixelSelectionType::Brush);
        _lassoAction.setChecked(type == PixelSelectionType::Lasso);
        _polygonAction.setChecked(type == PixelSelectionType::Polygon);
        _sampleAction.setChecked(type == PixelSelectionType::Sample);
        _roiAction.setChecked(type == PixelSelectionType::ROI);
    };

    connect(_pixelSelectionTool, &PixelSelectionTool::typeChanged, this, updateType);

    updateType();
}

void PixelSelectionAction::initModifier()
{
    connect(&_modifierReplaceAction, &QAction::toggled, [this](bool toggled) {
        if (toggled)
            _modifierAction.setCurrentIndex(static_cast<std::int32_t>(PixelSelectionModifierType::Replace));
    });

    connect(&_modifierAddAction, &QAction::toggled, [this](bool toggled) {
        _modifierAction.setCurrentIndex(toggled ? static_cast<std::int32_t>(PixelSelectionModifierType::Add) : static_cast<std::int32_t>(PixelSelectionModifierType::Replace));
    });

    connect(&_modifierSubtractAction, &QAction::toggled, [this](bool toggled) {
        _modifierAction.setCurrentIndex(toggled ? static_cast<std::int32_t>(PixelSelectionModifierType::Subtract) : static_cast<std::int32_t>(PixelSelectionModifierType::Replace));
    });

    if (!isInitialized())
        return;

    const auto updateModifier = [this]() -> void {
        const auto modifier = static_cast<PixelSelectionModifierType>(_modifierAction.getCurrentIndex());

        _pixelSelectionTool->setModifier(modifier);

        _modifierReplaceAction.setChecked(modifier == PixelSelectionModifierType::Replace);
        _modifierAddAction.setChecked(modifier == PixelSelectionModifierType::Add);
        _modifierSubtractAction.setChecked(modifier == PixelSelectionModifierType::Subtract);
    };

    updateModifier();

    connect(&_modifierAction, &OptionAction::currentIndexChanged, this, updateModifier);
}

void PixelSelectionAction::initMiscellaneous()
{
    connect(_pixelSelectionTool, &PixelSelectionTool::brushRadiusChanged, this, [this](const float& brushRadius) {
        _brushRadiusAction.setValue(brushRadius);
    });

    connect(&_brushRadiusAction, &DecimalAction::valueChanged, this, [this](const double& value) {
        _pixelSelectionTool->setBrushRadius(value);
    });

    const auto updateNotifyDuringSelection = [this]() -> void {
        _notifyDuringSelectionAction.setChecked(_pixelSelectionTool->isNotifyDuringSelection());
    };

    connect(_pixelSelectionTool, &PixelSelectionTool::notifyDuringSelectionChanged, this, [this, updateNotifyDuringSelection](const bool& notifyDuringSelection) {
        updateNotifyDuringSelection();
    });

    updateNotifyDuringSelection();

    connect(&_notifyDuringSelectionAction, &QAction::toggled, [this](bool toggled) {
        _pixelSelectionTool->setNotifyDuringSelection(toggled);
    });
}

bool PixelSelectionAction::isInitialized() const
{
    return (_targetWidget != nullptr) && (_pixelSelectionTool != nullptr);
}

QMenu* PixelSelectionAction::getContextMenu()
{
    auto menu = new QMenu("Selection");

    const auto addActionToMenu = [menu](QAction* action) -> void {
        auto actionMenu = new QMenu(action->text());

        actionMenu->addAction(action);

        menu->addMenu(actionMenu);
    };

    if (_pixelSelectionTypes.contains(PixelSelectionType::Rectangle))
        menu->addAction(&_rectangleAction);

    if (_pixelSelectionTypes.contains(PixelSelectionType::Brush))
        menu->addAction(&_brushAction);

    if (_pixelSelectionTypes.contains(PixelSelectionType::Lasso))
        menu->addAction(&_lassoAction);

    if (_pixelSelectionTypes.contains(PixelSelectionType::Polygon))
        menu->addAction(&_polygonAction);

    if (_pixelSelectionTypes.contains(PixelSelectionType::Sample))
        menu->addAction(&_sampleAction);

    menu->addSeparator();

    if (_pixelSelectionTypes.contains(PixelSelectionType::Brush))
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
    if (!isEnabled())
        return QObject::eventFilter(object, event);

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

void PixelSelectionAction::connectToPublicAction(WidgetAction* publicAction, bool recursive)
{
    auto publicPixelSelectionAction = dynamic_cast<PixelSelectionAction*>(publicAction);

    Q_ASSERT(publicPixelSelectionAction != nullptr);

    if (publicPixelSelectionAction == nullptr)
        return;

    if (recursive) {
        actions().connectPrivateActionToPublicAction(&_overlayColorAction, &publicPixelSelectionAction->getOverlayColorAction(), recursive);
        actions().connectPrivateActionToPublicAction(&_overlayOpacityAction, &publicPixelSelectionAction->getOverlayOpacityAction(), recursive);
        actions().connectPrivateActionToPublicAction(&_typeAction, &publicPixelSelectionAction->getTypeAction(), recursive);
        actions().connectPrivateActionToPublicAction(&_modifierAction, &publicPixelSelectionAction->getModifierAction(), recursive);
        actions().connectPrivateActionToPublicAction(&_clearSelectionAction, &publicPixelSelectionAction->getClearSelectionAction(), recursive);
        actions().connectPrivateActionToPublicAction(&_selectAllAction, &publicPixelSelectionAction->getSelectAllAction(), recursive);
        actions().connectPrivateActionToPublicAction(&_invertSelectionAction, &publicPixelSelectionAction->getInvertSelectionAction(), recursive);
        actions().connectPrivateActionToPublicAction(&_brushRadiusAction, &publicPixelSelectionAction->getBrushRadiusAction(), recursive);
        actions().connectPrivateActionToPublicAction(&_notifyDuringSelectionAction, &publicPixelSelectionAction->getNotifyDuringSelectionAction(), recursive);
    }

    WidgetAction::connectToPublicAction(publicAction, recursive);
}

void PixelSelectionAction::disconnectFromPublicAction(bool recursive)
{
    if (!isConnected())
        return;

    if (recursive) {
        actions().disconnectPrivateActionFromPublicAction(&_overlayColorAction, recursive);
        actions().disconnectPrivateActionFromPublicAction(&_overlayOpacityAction, recursive);
        actions().disconnectPrivateActionFromPublicAction(&_typeAction, recursive);
        actions().disconnectPrivateActionFromPublicAction(&_modifierAction, recursive);
        actions().disconnectPrivateActionFromPublicAction(&_clearSelectionAction, recursive);
        actions().disconnectPrivateActionFromPublicAction(&_selectAllAction, recursive);
        actions().disconnectPrivateActionFromPublicAction(&_invertSelectionAction, recursive);
        actions().disconnectPrivateActionFromPublicAction(&_brushRadiusAction, recursive);
        actions().disconnectPrivateActionFromPublicAction(&_notifyDuringSelectionAction, recursive);
    }

    WidgetAction::disconnectFromPublicAction(recursive);
}

void PixelSelectionAction::fromVariantMap(const QVariantMap& variantMap)
{
    GroupAction::fromVariantMap(variantMap);

    _overlayColorAction.fromParentVariantMap(variantMap);
    _overlayOpacityAction.fromParentVariantMap(variantMap);
    _typeAction.fromParentVariantMap(variantMap);
    _modifierAction.fromParentVariantMap(variantMap);
    _brushRadiusAction.fromParentVariantMap(variantMap);
    _notifyDuringSelectionAction.fromParentVariantMap(variantMap);
}

QVariantMap PixelSelectionAction::toVariantMap() const
{
    auto variantMap = GroupAction::toVariantMap();

    _overlayColorAction.insertIntoVariantMap(variantMap);
    _overlayOpacityAction.insertIntoVariantMap(variantMap);
    _typeAction.insertIntoVariantMap(variantMap);
    _modifierAction.insertIntoVariantMap(variantMap);
    _brushRadiusAction.insertIntoVariantMap(variantMap);
    _notifyDuringSelectionAction.insertIntoVariantMap(variantMap);

    return variantMap;
}

}
