#include "ColorMapDiscreteAction.h"
#include "ColorMapAction.h"
#include "ColorMapSettingsAction.h"

#include <QVBoxLayout>
#include <QGroupBox>

using namespace hdps::util;

namespace hdps {

namespace gui {

ColorMapDiscreteAction::ColorMapDiscreteAction(ColorMapSettingsAction& colorMapSettingsAction) :
    WidgetAction(&colorMapSettingsAction),
    _numberOfStepsAction(this, "Number of steps", 2, 10, 5, 5),
    _discretizeAlphaAction(this, "Discretize alpha", false, false)
{
    setText("Discrete");
    setCheckable(true);

    _numberOfStepsAction.setToolTip("Number of discrete steps");
    _discretizeAlphaAction.setToolTip("Whether to discrete the alpha channel");
}

void ColorMapDiscreteAction::connectToPublicAction(WidgetAction* publicAction)
{
    auto publicColorMapDiscreteAction = dynamic_cast<ColorMapDiscreteAction*>(publicAction);

    Q_ASSERT(publicColorMapDiscreteAction != nullptr);

    _numberOfStepsAction.connectToPublicAction(&publicColorMapDiscreteAction->getNumberOfStepsAction());
    _discretizeAlphaAction.connectToPublicAction(&publicColorMapDiscreteAction->getDiscretizeAlphaAction());

    connect(this, &WidgetAction::toggled, publicColorMapDiscreteAction, &WidgetAction::setChecked);
    connect(publicColorMapDiscreteAction, &WidgetAction::toggled, this, &WidgetAction::setChecked);

    setChecked(publicColorMapDiscreteAction->isChecked());

    WidgetAction::connectToPublicAction(publicAction);
}

void ColorMapDiscreteAction::disconnectFromPublicAction()
{
    _numberOfStepsAction.disconnectFromPublicAction();
    _discretizeAlphaAction.disconnectFromPublicAction();

    WidgetAction::disconnectFromPublicAction();
}

void ColorMapDiscreteAction::fromVariantMap(const QVariantMap& variantMap)
{
    if (!variantMap.contains("Enabled"))
        return;

    setChecked(variantMap["Enabled"].toBool());
}

QVariantMap ColorMapDiscreteAction::toVariantMap() const
{
    return { { "Enabled", isChecked()} };
}

ColorMapDiscreteAction::Widget::Widget(QWidget* parent, ColorMapDiscreteAction* colorMapDiscreteAction) :
    WidgetActionWidget(parent, colorMapDiscreteAction)
{
    auto layout         = new QVBoxLayout();
    auto groupBox       = new QGroupBox();
    auto groupBoxLayout = new QGridLayout();

    layout->setMargin(4);
    layout->addWidget(groupBox);

    groupBox->setCheckable(true);
    groupBox->setLayout(groupBoxLayout);

    groupBoxLayout->addWidget(colorMapDiscreteAction->getNumberOfStepsAction().createLabelWidget(this), 0, 0);
    groupBoxLayout->addWidget(colorMapDiscreteAction->getNumberOfStepsAction().createWidget(this), 0, 1);
    groupBoxLayout->addWidget(colorMapDiscreteAction->getDiscretizeAlphaAction().createWidget(this), 1, 1);
    groupBoxLayout->setColumnStretch(1, 1);

    const auto updateGroupBox = [this, groupBox, colorMapDiscreteAction]() {
        QSignalBlocker blocker(groupBox);

        groupBox->setTitle(colorMapDiscreteAction->text());
        groupBox->setChecked(colorMapDiscreteAction->isChecked());
    };

    connect(colorMapDiscreteAction, &ToggleAction::toggled, this, updateGroupBox);

    connect(groupBox, &QGroupBox::toggled, this, [this, colorMapDiscreteAction](bool toggled) {
        colorMapDiscreteAction->setChecked(toggled);
    });

    setLayout(layout);

    updateGroupBox();
}

}
}
