// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "RecentFile.h"

#ifdef _DEBUG
    #define RECENT_FILE_VERBOSE
#endif

namespace mv::util {

RecentFile::RecentFile(const QString& filePath /*= ""*/, const QDateTime& dateTime /*= QDateTime::currentDateTime()*/):
    _filePath(filePath),
    _dateTime(dateTime)
{
}

QString RecentFile::getFilePath() const
{
    return _filePath;
}

QDateTime RecentFile::getDateTime() const
{
    return _dateTime;
}

}
