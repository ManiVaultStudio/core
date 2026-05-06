// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include <QWidget>

namespace mv::util
{

/**
 * Severity level
 *
 * Enum for representing the severity level of an event
 */
enum class SeverityLevel {
    Info,
    Warning,
    Error,
    Fatal
};

/** Map of severity levels to their string representations */
const QMap<SeverityLevel, QString> severityLevelNames = {
    { SeverityLevel::Info, "Info" },
    { SeverityLevel::Warning, "Warning" },
    { SeverityLevel::Error, "Error" },
    { SeverityLevel::Fatal, "Fatal" }
};

/**
 * Get the name of a severity level
 * @param level The severity level to get the name of
 * @return The name of the severity level
 */
QString CORE_EXPORT getSeverityLevelName(SeverityLevel level);

/**
 * Get the severity level from a name
 * @param levelName The name of the severity level to get
 * @return The severity level corresponding to the given name, or std::nullopt if the name is invalid
 */
SeverityLevel CORE_EXPORT getSeverityLevel(const QString& levelName);

/** Vector of severity levels */
using SeverityLevels = QVector<SeverityLevel>;

/** Vector of all severity levels */
static const SeverityLevels allSeverityLevels = {
    SeverityLevel::Info,
    SeverityLevel::Warning,
    SeverityLevel::Error,
    SeverityLevel::Fatal
};

}