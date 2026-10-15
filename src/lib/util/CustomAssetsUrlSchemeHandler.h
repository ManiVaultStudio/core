// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include <QWebEngineUrlSchemeHandler>
#include <QMimeDatabase>

namespace mv::util
{

/**
 * Custom assets URL scheme handler class
 *
 * Gives restricted access to custom assets located in the application directory
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT CustomAssetsUrlSchemeHandler : public QWebEngineUrlSchemeHandler
{
    Q_OBJECT

public:

    /**
     * Construct with optional parent
     * @param parent 
     */
    explicit CustomAssetsUrlSchemeHandler(QObject* parent = nullptr);

    /**
     * Invoked when a request is started
     * @param job Request job
     */
    void requestStarted(QWebEngineUrlRequestJob* job) override;

private:
    const QString _rootDir;     /** Root directory for custom assets */
};


}
