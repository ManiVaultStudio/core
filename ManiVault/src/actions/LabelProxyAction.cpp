// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "LabelProxyAction.h"

#include <QDebug>

namespace mv::gui {

LabelProxyAction::LabelProxyAction(QObject* parent, const QString& title, WidgetAction* sourceAction) :
    WidgetAction(parent, title),
    _sourceAction(sourceAction)
{
    Q_ASSERT(_sourceAction != nullptr);
}

}
