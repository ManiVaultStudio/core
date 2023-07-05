// SPDX-License-Identifier: LGPL-3.0-or-later 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "VerticalGroupAction.h"

namespace hdps::gui {

VerticalGroupAction::VerticalGroupAction(QObject* parent, const QString& title, const bool& expanded /*= false*/) :
    GroupAction(parent, title, expanded)
{
    setDefaultWidgetFlags(GroupAction::Vertical);
}

}