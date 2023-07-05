// SPDX-License-Identifier: LGPL-3.0-or-later 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QString>

namespace hdps
{
namespace util
{

QString loadFileContents(QString path);
bool ShowFileInFolder(const QString path);

}

}