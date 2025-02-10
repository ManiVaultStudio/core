// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ThemeWatcher.h"
#include "Application.h"

#include <QDebug>

namespace mv::util
{

ThemeWatcher::ThemeWatcher(QObject* parent /*= nullptr*/) :
    QObject(parent)
{
    if (!Application::current()) {
        qWarning() << "ThemeWatcher: Application is not available";
        return;
    }
    
    connect(Application::current(), &Application::paletteChanged, this, &ThemeWatcher::paletteChanged);
}

}