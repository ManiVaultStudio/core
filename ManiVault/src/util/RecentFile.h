// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QString>
#include <QDateTime>

#include <vector>

namespace mv::util {

/**
 * Recent file class
 *
 * Utility class for recent file (contains file path and date time).
 *
 * Note: This action is developed for internal use only
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT RecentFile final {
public:

    /**
     * Construct with \p filePath and \p dateTime
     * @param filePath File path of the recent file
     * @param dateTime Date and time the recent file was last opened
     */
    RecentFile(const QString& filePath = "", const QDateTime& dateTime = QDateTime::currentDateTime());

    /**
     * Get file path
     * @return File path of the recent file
     */
    QString getFilePath() const;

    /**
     * Get date and time
     * @return Date and time the recent file was last opened
     */
    QDateTime getDateTime() const;

    /**
     * Compare equality with \other recent files
     * @param other Other recent file to compare with
     * @return Boolean whether equal to the other recent file
     */
    bool operator==(const RecentFile& other) const {
        return getFilePath() == other.getFilePath();
    }

    /**
     * Establish whether smaller than the \p other recent file
     * @param other Other recent file to compare with
     * @return Boolean whether smaller than the other recent file
     */
    bool operator<(const RecentFile& other) const {
        return getDateTime() > other.getDateTime();
    }

private:
    QString       _filePath;  /** File path of the recent file */
    QDateTime     _dateTime;  /** Date and time the recent file was last opened */
};

using RecentFiles = std::vector<RecentFile>;


}
