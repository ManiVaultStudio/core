#include "PixelSelectionOperationsAction.h"
#include "PixelSelectionAction.h"
#include "Application.h"

#include "util/PixelSelectionTool.h"

PixelSelectionOperationsAction::PixelSelectionOperationsAction(PixelSelectionAction& pixelSelectionAction) :
    WidgetAction(&pixelSelectionAction),
    _pixelSelectionAction(pixelSelectionAction),
    _clearSelectionAction(this, "Select none"),
    _selectAllAction(this, "Select all"),
    _invertSelectionAction(this, "Invert selection")
{
    setText("Selection operations");

    auto targetWidget = _pixelSelectionAction.getTargetWidget();

    auto& pixelSelectionTool = _pixelSelectionAction.getPixelSelectionTool();

    _clearSelectionAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);
    _selectAllAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);
    _invertSelectionAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);

    targetWidget->addAction(&_clearSelectionAction);
    targetWidget->addAction(&_selectAllAction);
    targetWidget->addAction(&_invertSelectionAction);

    _clearSelectionAction.setShortcut(QKeySequence("E"));
    _selectAllAction.setShortcut(QKeySequence("A"));
    _invertSelectionAction.setShortcut(QKeySequence("I"));

    _clearSelectionAction.setToolTip("Clears the selection (E)");
    _selectAllAction.setToolTip("Select all data points (A)");
    _invertSelectionAction.setToolTip("Invert the selection (I)");

    const auto updateSelectionButtons = [this]() {
        //_clearSelectionAction.setEnabled(_scatterplotPlugin->canClearSelection());
        //_selectAllAction.setEnabled(_scatterplotPlugin->canSelectAll());
        //_invertSelectionAction.setEnabled(_scatterplotPlugin->canInvertSelection());
    };

    //connect(_scatterplotPlugin, qOverload<>(&ScatterplotPlugin::selectionChanged), this, [this, updateSelectionButtons]() {
    //    updateSelectionButtons();
    //});

    updateSelectionButtons();
}

PixelSelectionOperationsAction::Widget::Widget(QWidget* parent, PixelSelectionOperationsAction* pixelSelectionOperationsAction, const WidgetActionWidget::State& state) :
    WidgetActionWidget(parent, pixelSelectionOperationsAction, state)
{
    auto layout = new QHBoxLayout();

    layout->setSpacing(3);
    layout->setMargin(0);

    layout->addWidget(pixelSelectionOperationsAction->getSelectAllAction().createWidget(this));
    layout->addWidget(pixelSelectionOperationsAction->getClearSelectionAction().createWidget(this));
    layout->addWidget(pixelSelectionOperationsAction->getInvertSelectionAction().createWidget(this));

    setLayout(layout);
}
