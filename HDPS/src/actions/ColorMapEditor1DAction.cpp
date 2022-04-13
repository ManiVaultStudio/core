#include "ColorMapEditor1DAction.h"
#include "ColorMapAction.h"
#include "Application.h"

#include <QVBoxLayout>
#include <QHBoxLayout>

namespace hdps {

namespace gui {

ColorMapEditor1DAction::ColorMapEditor1DAction(ColorMapAction& colorMapAction) :
    WidgetAction(&colorMapAction),
    _colorMapAction(colorMapAction),
    _nodeAction(*this)
{
    setText("Custom");
    setIcon(Application::getIconFont("FontAwesome").getIcon("chart-line"));
    setCheckable(true);
}

void ColorMapEditor1DAction::connectToPublicAction(WidgetAction* publicAction)
{
    auto publicColorMapEditor1DAction = dynamic_cast<ColorMapEditor1DAction*>(publicAction);

    Q_ASSERT(publicColorMapEditor1DAction != nullptr);

    WidgetAction::connectToPublicAction(publicAction);
}

void ColorMapEditor1DAction::disconnectFromPublicAction()
{
    WidgetAction::disconnectFromPublicAction();
}

ColorMapEditor1DAction::Widget::Widget(QWidget* parent, ColorMapEditor1DAction* colorMapEditor1DAction) :
    WidgetActionWidget(parent, colorMapEditor1DAction),
    _colorMapEditor1DWidget(this, *colorMapEditor1DAction),
    _goToFirstNodeAction(this, "First node"),
    _goToPreviousNodeAction(this, "Previous node"),
    _goToNextNodeAction(this, "Next node"),
    _goToLastNodeAction(this, "Last node"),
    _removeNodeAction(this, "Remove node")
{
    setAutoFillBackground(true);
    setFixedWidth(300);

    _goToFirstNodeAction.setDefaultWidgetFlags(TriggerAction::Icon);
    _goToPreviousNodeAction.setDefaultWidgetFlags(TriggerAction::Icon);
    _goToNextNodeAction.setDefaultWidgetFlags(TriggerAction::Icon);
    _goToLastNodeAction.setDefaultWidgetFlags(TriggerAction::Icon);
    _removeNodeAction.setDefaultWidgetFlags(TriggerAction::Icon);

    _goToFirstNodeAction.setToolTip("Go to the first node");
    _goToPreviousNodeAction.setToolTip("Go to the previous node");
    _goToNextNodeAction.setToolTip("Go to the next node");
    _goToLastNodeAction.setToolTip("Go to the last node");
    _removeNodeAction.setToolTip("Remove the selected node");

    auto& fontAwesome = Application::getIconFont("FontAwesome");

    _goToFirstNodeAction.setIcon(fontAwesome.getIcon("angle-double-left"));
    _goToPreviousNodeAction.setIcon(fontAwesome.getIcon("angle-left"));
    _goToNextNodeAction.setIcon(fontAwesome.getIcon("angle-right"));
    _goToLastNodeAction.setIcon(fontAwesome.getIcon("angle-double-right"));
    _removeNodeAction.setIcon(fontAwesome.getIcon("trash"));

    auto layout         = new QVBoxLayout();
    auto toolbarLayout  = new QHBoxLayout();

    toolbarLayout->setSpacing(3);
    toolbarLayout->addStretch(1);
    toolbarLayout->addWidget(_goToFirstNodeAction.createWidget(this));
    toolbarLayout->addWidget(_goToPreviousNodeAction.createWidget(this));
    toolbarLayout->addWidget(_goToNextNodeAction.createWidget(this));
    toolbarLayout->addWidget(_goToLastNodeAction.createWidget(this));
    toolbarLayout->addWidget(_removeNodeAction.createWidget(this));

    layout->addStretch(1);
    layout->addWidget(&_colorMapEditor1DWidget);
    layout->addLayout(toolbarLayout);
    layout->addWidget(colorMapEditor1DAction->getNodeAction().createWidget(this));

    setPopupLayout(layout);

    connect(&_goToFirstNodeAction, &TriggerAction::triggered, this, [this] {
        _colorMapEditor1DWidget.selectNode(_colorMapEditor1DWidget.getNodes().first());
    });

    connect(&_goToPreviousNodeAction, &TriggerAction::triggered, this, [this] {
        _colorMapEditor1DWidget.selectNode(_colorMapEditor1DWidget.getPreviousNode(_colorMapEditor1DWidget.getCurrentNode()));
    });

    connect(&_goToNextNodeAction, &TriggerAction::triggered, this, [this] {
        _colorMapEditor1DWidget.selectNode(_colorMapEditor1DWidget.getNextNode(_colorMapEditor1DWidget.getCurrentNode()));
    });

    connect(&_goToLastNodeAction, &TriggerAction::triggered, this, [this] {
        _colorMapEditor1DWidget.selectNode(_colorMapEditor1DWidget.getNodes().last());
    });

    connect(&_removeNodeAction, &TriggerAction::triggered, this, [this] {
        const auto currentNode  = _colorMapEditor1DWidget.getCurrentNode();
        const auto nodes        = _colorMapEditor1DWidget.getNodes();

        if (currentNode != nodes.first() || currentNode != nodes.last())
            return;

        _colorMapEditor1DWidget.removeNode(currentNode);
    });

    const auto updateActions = [this]() -> void {
        const auto currentNode  = _colorMapEditor1DWidget.getCurrentNode();
        const auto nodes        = _colorMapEditor1DWidget.getNodes();

        _goToFirstNodeAction.setEnabled(currentNode && currentNode != nodes.first());
        _goToPreviousNodeAction.setEnabled(currentNode && _colorMapEditor1DWidget.getPreviousNode(currentNode) != currentNode);
        _goToNextNodeAction.setEnabled(currentNode && _colorMapEditor1DWidget.getNextNode(currentNode) != currentNode);
        _goToLastNodeAction.setEnabled(currentNode && currentNode != nodes.last());
        _removeNodeAction.setEnabled(currentNode && currentNode != nodes.first() && currentNode != nodes.last());
    };

    connect(&_colorMapEditor1DWidget, &ColorMapEditor1DWidget::currentNodeChanged, this, updateActions);

    updateActions();
}

}
}
