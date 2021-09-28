#include "PixelSelectionTypeAction.h"
#include "PixelSelectionAction.h"
#include "Application.h"

using namespace hdps::util;

namespace hdps {

namespace gui {

PixelSelectionTypeAction::PixelSelectionTypeAction(PixelSelectionAction& pixelSelectionAction, const PixelSelectionTypes& pixelSelectionTypes /*= util::defaultPixelSelectionTypes*/) :
    WidgetAction(&pixelSelectionAction),
    _pixelSelectionAction(pixelSelectionAction),
    _pixelSelectionTypes(pixelSelectionTypes),
    _typeAction(this, "Type"),
    _rectangleAction(this, ""),
    _brushAction(this, ""),
    _lassoAction(this, ""),
    _polygonAction(this, ""),
    _sampleAction(this, ""),
    _typeActionGroup(this)
{
    setText("Type");

    auto targetWidget = _pixelSelectionAction.getTargetWidget();
    
    auto& pixelSelectionTool = _pixelSelectionAction.getPixelSelectionTool();

    _rectangleAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);
    _brushAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);
    _lassoAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);
    _polygonAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);
    _sampleAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);

    _rectangleAction.setIcon(PixelSelectionTool::getIcon(PixelSelectionType::Rectangle));
    _brushAction.setIcon(PixelSelectionTool::getIcon(PixelSelectionType::Brush));
    _lassoAction.setIcon(PixelSelectionTool::getIcon(PixelSelectionType::Lasso));
    _polygonAction.setIcon(PixelSelectionTool::getIcon(PixelSelectionType::Polygon));
    _sampleAction.setIcon(PixelSelectionTool::getIcon(PixelSelectionType::Sample));

    targetWidget->addAction(&_rectangleAction);
    targetWidget->addAction(&_brushAction);
    targetWidget->addAction(&_lassoAction);
    targetWidget->addAction(&_polygonAction);
    targetWidget->addAction(&_sampleAction);

    _rectangleAction.setCheckable(true);
    _brushAction.setCheckable(true);
    _lassoAction.setCheckable(true);
    _polygonAction.setCheckable(true);
    _sampleAction.setCheckable(true);

    _rectangleAction.setShortcut(QKeySequence("R"));
    _brushAction.setShortcut(QKeySequence("B"));
    _lassoAction.setShortcut(QKeySequence("L"));
    _polygonAction.setShortcut(QKeySequence("P"));
    _sampleAction.setShortcut(QKeySequence("S"));

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

    connect(&_sampleAction, &QAction::triggered, this, [this, &pixelSelectionTool]() {
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

bool PixelSelectionTypeAction::isResettable() const
{
    return _typeAction.isResettable();
}

void PixelSelectionTypeAction::reset()
{
    _typeAction.reset();
}

PixelSelectionTypes PixelSelectionTypeAction::getAllowedTypes() const
{
    return _pixelSelectionTypes;
}

void PixelSelectionTypeAction::setAllowedTypes(const util::PixelSelectionTypes& pixelSelectionTypes)
{
    _pixelSelectionTypes = pixelSelectionTypes;
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

        // Get allowed pixel selection types
        const auto pixelSelectionTypes = pixelSelectionTypeAction->getAllowedTypes();

        if (pixelSelectionTypes.contains(PixelSelectionType::Rectangle))
            layout->addWidget(pixelSelectionTypeAction->getRectangleAction().createWidget(this));

        if (pixelSelectionTypes.contains(PixelSelectionType::Brush))
            layout->addWidget(pixelSelectionTypeAction->getBrushAction().createWidget(this));

        if (pixelSelectionTypes.contains(PixelSelectionType::Lasso))
            layout->addWidget(pixelSelectionTypeAction->getLassoAction().createWidget(this));

        if (pixelSelectionTypes.contains(PixelSelectionType::Polygon))
            layout->addWidget(pixelSelectionTypeAction->getPolygonAction().createWidget(this));

        if (pixelSelectionTypes.contains(PixelSelectionType::Sample))
            layout->addWidget(pixelSelectionTypeAction->getSampleAction().createWidget(this));
    }

    if (pixelSelectionTypeAction->hasWidgetFlag(PixelSelectionTypeAction::ResetPushButton))
        layout->addWidget(pixelSelectionTypeAction->createResetButton(this));

    setLayout(layout);
}

}
}
