// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include <QObject>

namespace mv::util {

/**
 * Splash screen bridge utility class
 *
 * For interaction between splash screen HTML and C++ code
 *
 * Note: This utility class is developed for internal use only
 *
 * @author Thomas Kroes
 */
class SplashScreenBridge : public QObject
{
    Q_OBJECT

    Q_PROPERTY(std::int32_t progress READ progress NOTIFY progressChanged)

public:

    /**
     * Construct with pointer to \p parent object
     * @param parent 
     */
    explicit SplashScreenBridge(QObject* parent = nullptr);

    /**
     * Get progress value
     * @return Progress value in range [0, 100]
     */
    std::int32_t progress() const;

public slots:

    /**
     * Set progress value to \p progress
     * @param progress Progress value in range [0, 100]
     */
	void setProgress(std::int32_t progress);

    void requestInitial();

    // called by JS at load

signals:

    /**
     * Signals that progress changed to \p progress
     * @param progress New progress value in range [0, 100]
     */
    void progressChanged(std::int32_t progress);

private:
    std::int32_t    _progress = 0;      /** Progress value in range [0, 100] */
};

}