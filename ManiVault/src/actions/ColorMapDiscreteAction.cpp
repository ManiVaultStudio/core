// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ColorMapDiscreteAction.h"
#include "ColorMapAction.h"

#include <QVBoxLayout>
#include <QGroupBox>

using namespace mv::util;

namespace mv::gui {

ColorMapDiscreteAction::ColorMapDiscreteAction(ColorMapSettingsAction& colorMapSettingsAction) :
    WidgetAction(&colorMapSettingsAction, "Discrete"),
    _numberOfStepsAction(this, "Number of steps", 2, 10, 5, 5),
    _discretizeAlphaAction(this, "Discretize alpha", false, false)
{
    setText("Discrete");
    setCheckable(true);
    setSerializationName("Discrete");

    
}

void ColorMapDiscreteAction::connectToPublicAction(WidgetAction* publicAction)
{
    
}

void ColorMapDiscreteAction::disconnectFromPublicAction()
{
    
}

void ColorMapDiscreteAction::fromVariantMap(const QVariantMap& variantMap)
{
    WidgetAction::fromVariantMap(variantMap);

    
}

QVariantMap ColorMapDiscreteAction::toVariantMap() const
{
    QVariantMap variantMap = WidgetAction::toVariantMap();

    

    return variantMap;
}

ColorMapDiscreteAction::Widget::Widget(QWidget* parent, ColorMapDiscreteAction* colorMapDiscreteAction) :
    WidgetActionWidget(parent, colorMapDiscreteAction)
{
    auto layout         = new QVBoxLayout();
    auto groupBox       = new QGroupBox();
    auto groupBoxLayout = new QGridLayout();

    layout->setContentsMargins(4, 4, 4, 4);
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
