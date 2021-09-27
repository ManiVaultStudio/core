#include "PixelSelectionTypeAction.h"
#include "PixelSelectionAction.h"
#include "Application.h"

#include "util/PixelSelectionTool.h"

using namespace hdps::util;

namespace hdps {

namespace gui {

PixelSelectionTypeAction::PixelSelectionTypeAction(PixelSelectionAction& pixelSelectionAction) :
    WidgetAction(&pixelSelectionAction),
    _pixelSelectionAction(pixelSelectionAction),
    _typeAction(this, "Type"),
    _rectangleAction(this, ""),
    _brushAction(this, ""),
    _lassoAction(this, ""),
    _polygonAction(this, ""),
    _typeActionGroup(this)
{
    setText("Type");

    auto targetWidget = _pixelSelectionAction.getTargetWidget();
    
    auto& pixelSelectionTool = _pixelSelectionAction.getPixelSelectionTool();

    _rectangleAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);
    _brushAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);
    _lassoAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);
    _polygonAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);

    _rectangleAction.setIcon(PixelSelectionTool::getIcon(PixelSelectionType::Rectangle));
    _brushAction.setIcon(PixelSelectionTool::getIcon(PixelSelectionType::Brush));
    _lassoAction.setIcon(PixelSelectionTool::getIcon(PixelSelectionType::Lasso));
    _polygonAction.setIcon(PixelSelectionTool::getIcon(PixelSelectionType::Polygon));

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

    _typeAction.setOptions(pixelSelectionTypes.values());
    _typeAction.setCurrentText("Rectangle");
    _typeAction.setDefaultText("Rectangle");

    _typeActionGroup.addAction(&_rectangleAction);
    _typeActionGroup.addAction(&_brushAction);
    _typeActionGroup.addAction(&_lassoAction);
    _typeActionGroup.addAction(&_polygonAction);

    connect(&_typeAction, &OptionAction::currentTextChanged, [this, &pixelSelectionTool](const QString& currentText) {
        pixelSelectionTool.setType(pixelSelectionTypes.key(currentText));
    });

    connect(&_rectangleAction, &QAction::triggered, this, [this, &pixelSelectionTool]() {
        pixelSelectionTool.setType(PixelSelectionType::Rectangle);
    });

    connect(&_brushAction, &QAction::triggered, this, [this, &pixelSelectionTool]() {
        pixelSelectionTool.setType(PixelSelectionType::Brush);
    });

    connect(&_lassoAction, &QAction::triggered, this, [this, &pixelSelectionTool]() {
        pixelSelectionTool.setType(PixelSelectionType::Lasso);
    });

    connect(&_polygonAction, &QAction::triggered, this, [this, &pixelSelectionTool]() {
        pixelSelectionTool.setType(PixelSelectionType::Polygon);
    });

    const auto updateType = [this, &pixelSelectionTool]() {
        const auto type = pixelSelectionTool.getType();

        _typeAction.setCurrentText(pixelSelectionTypes.value(type));
        _rectangleAction.setChecked(type == PixelSelectionType::Rectangle);
        _brushAction.setChecked(type == PixelSelectionType::Brush);
        _lassoAction.setChecked(type == PixelSelectionType::Lasso);
        _polygonAction.setChecked(type == PixelSelectionType::Polygon);

        setResettable(_typeAction.isResettable());
    };

    connect(&pixelSelectionTool, &PixelSelectionTool::typeChanged, this, updateType);

    updateType();
}

bool PixelSelectionTypeAction::isResettable() const
{
    return _typeAction.isResettable();
}

void PixelSelectionTypeAction::reset()
{
    _typeAction.reset();
}

PixelSelectionTypeAction::Widget::Widget(QWidget* parent, PixelSelectionTypeAction* pixelSelectionTypeAction, const WidgetActionWidget::State& state) :
    WidgetActionWidget(parent, pixelSelectionTypeAction, state)
{
    auto layout = new QHBoxLayout();

    layout->setSpacing(3);
    layout->setMargin(0);

    if (pixelSelectionTypeAction->hasWidgetFlag(PixelSelectionTypeAction::ComboBox))
        layout->addWidget(pixelSelectionTypeAction->getTypeAction().createWidget(this));

    if (pixelSelectionTypeAction->hasWidgetFlag(PixelSelectionTypeAction::PushButtonGroup)) {
        layout->addWidget(pixelSelectionTypeAction->getRectangleAction().createWidget(this));
        layout->addWidget(pixelSelectionTypeAction->getBrushAction().createWidget(this));
        layout->addWidget(pixelSelectionTypeAction->getLassoAction().createWidget(this));
        layout->addWidget(pixelSelectionTypeAction->getPolygonAction().createWidget(this));
    }

    if (pixelSelectionTypeAction->hasWidgetFlag(PixelSelectionTypeAction::ResetPushButton))
        layout->addWidget(pixelSelectionTypeAction->createResetButton(this));

    setLayout(layout);
}

}
}
