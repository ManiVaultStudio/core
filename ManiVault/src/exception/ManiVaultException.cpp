// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ManiVaultException.h"

#include <QDebug>

namespace mv {

ManiVaultException::ManiVaultException(SeverityLevel severity, QString message, QString what, QString where, QVariantMap details) :
	std::runtime_error(message.toStdString()),
	_severity(severity),
	_message(std::move(message)),
	_what(std::move(what)),
	_where(std::move(where)),
	_details(std::move(details))
{
}

}
