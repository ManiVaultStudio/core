// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

namespace mv::util {

/**
 * Video class
 *
 * Contains video meta data and offers ways to watch it
 *
 * @author Thomas Kroes
 */
struct CORE_EXPORT Video final
{
    QString         _title;         /** Title */
    QStringList     _tags;          /** Tags */
    QDateTime       _date;          /** Date and time */
    QString         _summary;       /** Summary */
    QString         _youTubeId;     /** Global unique identifier of the YouTube video */
    QString         _youTubeUrl;    /** URL of the YouTube video */
};

using Videos = std::vector<Video>;

}
