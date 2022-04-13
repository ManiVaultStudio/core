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
    _nodeAction(*this),
    _goToFirstNodeAction(this, "First node"),
    _goToPreviousNodeAction(this, "Previous node"),
    _goToNextNodeAction(this, "Next node"),
    _goToLastNodeAction(this, "Last node"),
    _removeNodeAction(this, "Remove node")
{
    setText("Custom");
    setIcon(Application::getIconFont("FontAwesome").getIcon("chart-line"));
    setCheckable(true);

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
    _colorMapEditor1DWidget(this, *colorMapEditor1DAction)
{
    setAutoFillBackground(true);
    setFixedWidth(300);

    auto layout         = new QVBoxLayout();
    auto toolbarLayout  = new QHBoxLayout();

    toolbarLayout->setSpacing(3);
    toolbarLayout->addStretch(1);
    toolbarLayout->addWidget(colorMapEditor1DAction->getGoToFirstNodeAction().createWidget(this));
    toolbarLayout->addWidget(colorMapEditor1DAction->getGoToPreviousNodeAction().createWidget(this));
    toolbarLayout->addWidget(colorMapEditor1DAction->getGoToNextNodeAction().createWidget(this));
    toolbarLayout->addWidget(colorMapEditor1DAction->getGoToLastNodeAction().createWidget(this));
    toolbarLayout->addWidget(colorMapEditor1DAction->getRemoveNodeAction().createWidget(this));

    layout->addStretch(1);
    layout->addWidget(&_colorMapEditor1DWidget);
    layout->addLayout(toolbarLayout);
    layout->addWidget(colorMapEditor1DAction->getNodeAction().createWidget(this));

    setPopupLayout(layout);
}

}
}
