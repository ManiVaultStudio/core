// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

namespace mv::util
{

/**
 * Secure network access manager class
 *
 * For secure, HTTPS-based, network access management.
 *
 * @author Thomas Kroes
 */
class SecureNetworkAccessManager : public QNetworkAccessManager {

    Q_OBJECT

public:

    /** No need for custom constructor */
    using QNetworkAccessManager::QNetworkAccessManager;

protected:

    /**
     * Create a network request, but block non-HTTPS requests
     * @param op Operation to perform
     * @param request Network request to create
     * @param outgoingData Optional data to send with the request
     * @return Pointer to the created network reply, or nullptr if the request is blocked
     */
    QNetworkReply* createRequest(Operation op, const QNetworkRequest& request, QIODevice* outgoingData = nullptr) override;
};

}
