// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ManiVaultException.h"

#include <QDebug>

namespace mv::util {
ManiVaultException::ManiVaultException(SeverityLevel severity, QString message, QString code, QString scope, QVariantMap details) :
	std::runtime_error(message.toStdString()),
	_severity(severity),
	_message(std::move(message)),
	_code(std::move(code)),
	_scope(std::move(scope)),
	_details(std::move(details))
{
}

}
