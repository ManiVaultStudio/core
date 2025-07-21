// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PageSubAction.h"

PageSubAction::PageSubAction(const QModelIndex& index, const QIcon& icon) :
    _icon(icon),
    _index(index)
{
}

PageSubAction::PageSubAction(const QModelIndex& index, const QIcon& icon, const ClickedCallback& clickedCallback, const TooltipCallback& tooltipCallback) :
    _icon(icon),
    _index(index),
    _clickedCallback(clickedCallback),
	_tooltipCallback(tooltipCallback)
{
}

QString PageSubAction::getTooltip() const
{
    if (_tooltipCallback)
        return _tooltipCallback();

    return {};
}

PageCommentsSubAction::PageCommentsSubAction(const QModelIndex& index) :
	PageSubAction(index, mv::util::StyledIcon("scroll"))
{
}

