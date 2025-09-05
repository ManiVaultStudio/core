// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Exception.h"

namespace mv::util {

BaseException::BaseException(const QString& message, const QIcon& icon):
	_message(message),
	_messageUtf8(message.toUtf8()),
	_icon(icon)
{
}

const char* BaseException::what() const noexcept
{
	return _messageUtf8.constData();
}

void BaseException::raise() const
{
	throw* this;
}

BaseException* BaseException::clone() const
{
	return new BaseException(*this);
}

QString BaseException::getMessage() const
{
	return _message;
}

QIcon BaseException::getIcon() const
{
	return _icon;
}

}
