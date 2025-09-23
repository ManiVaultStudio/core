// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "SecureNetworkAccessManager.h"
#include "CoreInterface.h"
#include "HttpsErrorReply.h"
#include "StyledIcon.h"

namespace mv::util
{

QNetworkReply* SecureNetworkAccessManager::createRequest(Operation op, const QNetworkRequest& request, QIODevice* outgoingData)
{
    const auto url = request.url();

    if (url.scheme() != "https") {
        mv::help().addNotification("Network error", QString("For your safety, the network access manager blocked a non-HTTPS request: %1").arg(url.toString()), StyledIcon("shield-halved"));

        return new HttpsErrorReply(request, this);
    }

    return QNetworkAccessManager::createRequest(op, request, outgoingData);
}

}
