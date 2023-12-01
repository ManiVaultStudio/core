// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "TemporaryDir.h"
#include "Application.h"

#include <QDir>

namespace mv::util {

TemporaryDir::TemporaryDir() :
    QTemporaryDir(QDir::cleanPath(QDir::tempPath() + QDir::separator() + "ManiVault" + QDir::separator() + Application::current()->getId()))
{
    qDebug() << QDir::cleanPath(QDir::tempPath() + QDir::separator() + "ManiVault" + QDir::separator() + Application::current()->getId());
}

}
