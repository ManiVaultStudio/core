// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include <QObject>
#include <QPalette>

namespace mv::util
{
	
/**
 * Theme watcher utility class
 *
 * Helper class for responding to application palette (theme) changes.
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT ThemeWatcher : public QObject
{
    Q_OBJECT

public: // Construction/destruction

    /**
     * Construct with pointer to \p parent object
     * @param parent Parent object (maybe nullptr)
     */
    ThemeWatcher(QObject* parent = nullptr);

signals:

    /**
     * Signal emitted when the application palette has changed to \p palette
     * @param palette Current palette
     */
	void paletteChanged(const QPalette& palette);
};

}