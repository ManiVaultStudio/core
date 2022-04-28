#include "ColorMapEditor1DAction.h"
#include "ColorMapAction.h"
#include "Application.h"

#include <QVBoxLayout>
#include <QHBoxLayout>

#ifdef _DEBUG
    #define COLOR_MAP_EDITOR_1D_ACTION
#endif

namespace hdps {

namespace gui {

ColorMapEditor1DAction::ColorMapEditor1DAction(ColorMapAction& colorMapAction) :
    WidgetAction(&colorMapAction),
    _colorMapAction(colorMapAction),
    _nodes(),
    _nodeAction(*this)
{
    setText("Custom");
    setIcon(Application::getIconFont("FontAwesome").getIcon("chart-line"));
    setCheckable(true);

    addNode(QPointF(0.0f, 0.0f));
    addNode(QPointF(1.0f, 1.0f));
}

QVector<ColorMapEditor1DNode*>& ColorMapEditor1DAction::getNodes()
{
    return _nodes;
}

ColorMapEditor1DNode* ColorMapEditor1DAction::addNode(const QPointF& normalizedCoordinate)
{
    auto node = new ColorMapEditor1DNode(*this, normalizedCoordinate);

    _nodes << node;

    sortNodes();

    emit nodeAdded(node);

    return node;
}

void ColorMapEditor1DAction::removeNode(ColorMapEditor1DNode* node)
{
    Q_ASSERT(node != nullptr);

    if (!_nodes.contains(node))
        return;

    _nodes.removeOne(node);

    sortNodes();

    const auto index = node->getIndex();

    emit nodeAboutToBeRemoved(node);

    delete node;

    emit nodeRemoved(index);
}

ColorMapEditor1DNode* ColorMapEditor1DAction::getPreviousNode(ColorMapEditor1DNode* node) const
{
    const auto indexOfSelectedNode = _nodes.indexOf(node);

    if (indexOfSelectedNode >= 1)
        return _nodes[indexOfSelectedNode - 1];

    return _nodes.first();
}

ColorMapEditor1DNode* ColorMapEditor1DAction::getNextNode(ColorMapEditor1DNode* node) const
{
    const auto indexOfSelectedNode = _nodes.indexOf(node);

    if (indexOfSelectedNode < (_nodes.count() - 1))
        return _nodes[indexOfSelectedNode + 1];

    return _nodes.last();
}

void ColorMapEditor1DAction::sortNodes()
{
    std::sort(_nodes.begin(), _nodes.end(), [](auto nodeA, auto nodeB) -> bool {
        return nodeA->getNormalizedCoordinate().x() < nodeB->getNormalizedCoordinate().x();
    });

    for (auto node : _nodes)
        node->setIndex(_nodes.indexOf(node));
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

    connect(&_goToPreviousNodeAction, &TriggerAction::triggered, this, [this, colorMapEditor1DAction] {
        _colorMapEditor1DWidget.selectNode(colorMapEditor1DAction->getPreviousNode(_colorMapEditor1DWidget.getCurrentNode()));
    });

    connect(&_goToNextNodeAction, &TriggerAction::triggered, this, [this, colorMapEditor1DAction] {
        _colorMapEditor1DWidget.selectNode(colorMapEditor1DAction->getNextNode(_colorMapEditor1DWidget.getCurrentNode()));
    });

    connect(&_goToLastNodeAction, &TriggerAction::triggered, this, [this] {
        _colorMapEditor1DWidget.selectNode(_colorMapEditor1DWidget.getNodes().last());
    });

    connect(&_removeNodeAction, &TriggerAction::triggered, this, [this, colorMapEditor1DAction] {
        const auto currentNode  = _colorMapEditor1DWidget.getCurrentNode();
        const auto nodes        = _colorMapEditor1DWidget.getNodes();

        if (currentNode == nodes.first() || currentNode == nodes.last())
            return;

        colorMapEditor1DAction->removeNode(currentNode);
    });

    const auto updateActions = [this, colorMapEditor1DAction]() -> void {
        const auto currentNode  = _colorMapEditor1DWidget.getCurrentNode();
        const auto nodes        = colorMapEditor1DAction->getNodes();

        if (currentNode) {
            _goToFirstNodeAction.setEnabled(currentNode && currentNode != nodes.first());
            _goToPreviousNodeAction.setEnabled(currentNode && colorMapEditor1DAction->getPreviousNode(currentNode) != currentNode);
            _goToNextNodeAction.setEnabled(currentNode && colorMapEditor1DAction->getNextNode(currentNode) != currentNode);
            _goToLastNodeAction.setEnabled(currentNode && currentNode != nodes.last());
            _removeNodeAction.setEnabled(currentNode && currentNode != nodes.first() && currentNode != nodes.last());
        }
        else {
            _goToFirstNodeAction.setEnabled(false);
            _goToPreviousNodeAction.setEnabled(false);
            _goToNextNodeAction.setEnabled(false);
            _goToLastNodeAction.setEnabled(false);
            _removeNodeAction.setEnabled(false);
        }
    };

    connect(&_colorMapEditor1DWidget, &ColorMapEditor1DWidget::currentNodeChanged, this, updateActions);

    updateActions();
}

}
}
