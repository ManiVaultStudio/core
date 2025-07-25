// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "View.h"

namespace mv::util
{

void TextElideDelegate::initStyleOption(QStyleOptionViewItem* option, const QModelIndex& index) const
{
	QStyledItemDelegate::initStyleOption(option, index);

	option->textElideMode = _textElideMode;
}

Qt::TextElideMode TextElideDelegate::getTextElideMode() const
{
	return _textElideMode;
}

void TextElideDelegate::setTextElideMode(const Qt::TextElideMode& textElideMode)
{
	_textElideMode = textElideMode;
}

}
