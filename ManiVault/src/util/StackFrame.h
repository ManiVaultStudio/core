// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"

#include <QString>
#include <QList>

namespace mv::util
{

/**
 * @brief Represents one frame in a captured debug stack trace.
 *
 * StackFrame stores best-effort symbolic information for a call stack entry.
 * Depending on platform and available debug symbols, some fields may be empty
 * or line may remain -1.
 *
 * @maintainer Thomas Kroes (BioVault - Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
struct CORE_EXPORT StackFrame
{
    QString     function;   /**< Demangled or symbolic function name, when available */
    QString     file;       /**< Source file path, when available */
    int         line = -1;  /**< Source line number, or -1 when unavailable */
    QString     module;     /**< Binary module or library containing the frame */
    QString     address;    /**< Instruction address associated with the frame */
    QString     raw;        /**< Raw stack-frame text captured from the platform backend */
};

/** Ordered collection of stack frames captured for a call stack. */
using StackTrace = QList<StackFrame>;

}
