// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ManiVaultException.h"

#include "util/Miscellaneous.h"

#include <QDebug>

using namespace mv::util;

namespace mv {

ManiVaultException::ManiVaultException(SeverityLevel severity, QString message, QString what, QString where, QVariantMap details /*= {}*/, std::source_location location /*= std::source_location::current()*/) :
	std::runtime_error(message.toStdString()),
	_severity(severity),
	_message(std::move(message)),
	_what(std::move(what)),
	_where(std::move(where)),
	_details(std::move(details)),
    _diagnosticId(QUuid::createUuid())
{
    _details["SourceLocation"] = QVariantMap{
        { "File", location.file_name() },
        { "Line", static_cast<int>(location.line()) },
        { "Function", location.function_name() }
    };

    _details["DiagnosticId"]        = _diagnosticId.toString(QUuid::WithoutBraces);
    _details["StackTraceOverview"]  = stackTraceFunctionList(mv::errors().getDebugStackTrace());
    _details["StackTrace"]          = stackTraceToVariantList(mv::errors().getDebugStackTrace());
}

ManiVaultException ManiVaultException::withAddedDetails(const QVariantMap& additionalDetails) const
{
    ManiVaultException copy = *this;

    for (auto it = additionalDetails.begin(); it != additionalDetails.end(); ++it)
        copy._details[it.key()] = it.value();

    return copy;
}

SeverityLevel ManiVaultException::getSeverity() const
{
    return _severity;
}

QString ManiVaultException::getMessage() const
{
    return _message;
}

QString ManiVaultException::getWhat() const
{
    return _what;
}

QString ManiVaultException::getWhere() const
{
    return _where;
}

QVariantMap ManiVaultException::getDetails() const
{
    return _details;
}

QUuid ManiVaultException::getDiagnosticId() const
{
    return _diagnosticId;
}

StackTrace ManiVaultException::getStackTrace() const
{
    return stackTraceFromVariantList(_details.value("StackTrace").toList());
}

}
