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

    Q_PROPERTY(int progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(QString progressDescription READ progressDescription NOTIFY progressDescriptionChanged)

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
    int progress() const;

    /**
     * Get progress description
     * @return Progress description
     */
    QString progressDescription() const;

public slots:

    /**
     * Set progress value to \p progress
     * @param progress Progress value in range [0, 100]
     */
	void setProgress(int progress);

    /**
     * Set progress description to \p description
     * @param progressDescription Progress description
     */
    void setProgressDescription(const QString& progressDescription);

    /** called by JS at load */
    void requestInitial();

signals:

    /**
     * Signals that progress changed to \p progress
     * @param progress New progress value in range [0, 100]
     */
    void progressChanged(int progress);

    /**
     * Signals that progress description changed to \p description
     * @param description New progress description
     */
    void progressDescriptionChanged(const QString& description);

private:
    int     _progress = 0;          /** Progress value in range [0, 100] */
    QString _progressDescription;   /** Progress description */
};

}