// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "HttpsErrorReply.h"

#include <QNetworkAccessManager>
#include <QTimer>

namespace mv::util
{

HttpsErrorReply::HttpsErrorReply(const QNetworkRequest& request, QObject* parent) :
    QNetworkReply(parent)
{
    setRequest(request);
    setUrl(request.url());
    setOperation(QNetworkAccessManager::GetOperation);
    setError(QNetworkReply::ProtocolUnknownError, "Only HTTPS is allowed");

    open(QIODevice::ReadOnly);

    QTimer::singleShot(0, this, &HttpsErrorReply::emitError);
}

void HttpsErrorReply::abort()
{
}

qint64 HttpsErrorReply::readData(char* data, qint64 maxlen)
{
    Q_UNUSED(data)
    Q_UNUSED(maxlen)

    return -1;
}

void HttpsErrorReply::emitError()
{
    emit errorOccurred(error());
    emit finished();
}

}
