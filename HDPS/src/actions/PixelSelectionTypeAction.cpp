#include "PixelSelectionTypeAction.h"
#include "PixelSelectionAction.h"
#include "Application.h"

#include "util/PixelSelectionTool.h"

PixelSelectionTypeAction::PixelSelectionTypeAction(PixelSelectionAction& pixelSelectionAction) :
    WidgetAction(&pixelSelectionAction),
    _pixelSelectionAction(pixelSelectionAction),
    _typeAction(this, "Type"),
    _rectangleAction(this, "Rectangle"),
    _brushAction(this, "Brush"),
    _lassoAction(this, "Lasso"),
    _polygonAction(this, "Polygon"),
    _typeActionGroup(this)
{
    setText("Selection overlay");

    auto targetWidget = _pixelSelectionAction.getTargetWidget();
    
    auto& pixelSelectionTool = _pixelSelectionAction.getPixelSelectionTool();

    _rectangleAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);
    _brushAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);
    _lassoAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);
    _polygonAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);

    _rectangleAction.setIcon(PixelSelectionTool::getIcon(PixelSelectionTool::Type::Rectangle));
    _brushAction.setIcon(PixelSelectionTool::getIcon(PixelSelectionTool::Type::Brush));
    _lassoAction.setIcon(PixelSelectionTool::getIcon(PixelSelectionTool::Type::Lasso));
    _polygonAction.setIcon(PixelSelectionTool::getIcon(PixelSelectionTool::Type::Polygon));

    targetWidget->addAction(&_rectangleAction);
    targetWidget->addAction(&_brushAction);
    targetWidget->addAction(&_lassoAction);
    targetWidget->addAction(&_polygonAction);

    _rectangleAction.setCheckable(true);
    _brushAction.setCheckable(true);
    _lassoAction.setCheckable(true);
    _polygonAction.setCheckable(true);

    _rectangleAction.setShortcut(QKeySequence("R"));
    _brushAction.setShortcut(QKeySequence("B"));
    _lassoAction.setShortcut(QKeySequence("L"));
    _polygonAction.setShortcut(QKeySequence("P"));

    _rectangleAction.setToolTip("Select data points inside a rectangle (R)");
    _brushAction.setToolTip("Select data points using a brush tool (B)");
    _lassoAction.setToolTip("Select data points using a lasso (L)");
    _polygonAction.setToolTip("Select data points by drawing a polygon (P)");

    _typeAction.setOptions(pixelSelectionTool.types.keys());

    _typeActionGroup.addAction(&_rectangleAction);
    _typeActionGroup.addAction(&_brushAction);
    _typeActionGroup.addAction(&_lassoAction);
    _typeActionGroup.addAction(&_polygonAction);

    connect(&_typeAction, &OptionAction::currentTextChanged, [this, &pixelSelectionTool](const QString& currentText) {
        pixelSelectionTool.setType(PixelSelectionTool::getTypeEnum(currentText));
    });

    connect(&_rectangleAction, &QAction::triggered, this, [this, &pixelSelectionTool]() {
        pixelSelectionTool.setType(PixelSelectionTool::Type::Rectangle);
    });

    connect(&_brushAction, &QAction::triggered, this, [this, &pixelSelectionTool]() {
        pixelSelectionTool.setType(PixelSelectionTool::Type::Brush);
    });

    connect(&_lassoAction, &QAction::triggered, this, [this, &pixelSelectionTool]() {
        pixelSelectionTool.setType(PixelSelectionTool::Type::Lasso);
    });

    connect(&_polygonAction, &QAction::triggered, this, [this, &pixelSelectionTool]() {
        pixelSelectionTool.setType(PixelSelectionTool::Type::Polygon);
    });

    const auto updateType = [this, &pixelSelectionTool]() {
        const auto type = pixelSelectionTool.getType();

        _typeAction.setCurrentText(PixelSelectionTool::getTypeName(type));
        _rectangleAction.setChecked(type == PixelSelectionTool::Type::Rectangle);
        _brushAction.setChecked(type == PixelSelectionTool::Type::Brush);
        _lassoAction.setChecked(type == PixelSelectionTool::Type::Lasso);
        _polygonAction.setChecked(type == PixelSelectionTool::Type::Polygon);
        //_brushRadiusAction.setEnabled(type == PixelSelectionTool::Type::Brush);
    };

    connect(&pixelSelectionTool, &PixelSelectionTool::typeChanged, this, updateType);

    updateType();
}
