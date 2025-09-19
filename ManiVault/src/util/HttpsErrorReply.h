// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include <QNetworkRequest>
#include <QNetworkReply>

namespace mv::util
{

/**
 * HTTPS error reply class
 *
 * Custom reply class for non-HTTPS requests.
 *
 * @author Thomas Kroes
 */
class HttpsErrorReply : public QNetworkReply {

    Q_OBJECT

public:

    /**
     *
     * Construct with a network \p request and an optional \p parent.
     * @param request The network request that caused the error
     * @param parent The parent object (optional)
     */
    HttpsErrorReply(const QNetworkRequest& request, QObject* parent = nullptr);

    /** Abort the reply (this method is called when the reply is aborted). */
    void abort() override;

    /**
     * Read data from the reply.
     * @param data Pointer to the data buffer where the read data will be stored
     * @param maxlen Maximum length of data to read
     * @return The number of bytes read, or -1 if no data is available
     */
    qint64 readData(char* data, qint64 maxlen) override;

private slots:

    /** Emit an error signal after a short delay (this is necessary to ensure that the reply is fully initialized before emitting the error) */
    void emitError();
};


}
