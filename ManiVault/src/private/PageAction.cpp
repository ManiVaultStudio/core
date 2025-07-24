// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PageAction.h"

#include <util/StyledIcon.h>

using namespace mv::util;

bool PageAction::compactView = false;

PageAction::PageAction(const QIcon& icon, const QString& title, const QString& subtitle, const QString& description, const QString& tooltip, const ClickedCallback& clickedCallback) :
    _icon(icon),
    _title(title),
    _subtitle(subtitle),
    _description(description),
    _tooltip(tooltip),
    _clickedCallback(clickedCallback)
{
    if (!description.isEmpty())
		createSubAction<CommentsPageSubAction>(description);
}

PageAction::PageAction(const QModelIndex& index)
{
    setEditorData(index);
}

QIcon PageAction::getIcon() const
{
	return _icon;
}

void PageAction::setIcon(const QIcon& icon)
{
	_icon = icon;

	emit iconChanged();
}

void PageAction::setExpanded(bool expanded)
{
    emit expandedChanged(expanded);
}

QString PageAction::getTitle() const
{
	return _title.isEmpty() ? "NA" : _title;
}

void PageAction::setTitle(const QString& title)
{
	_title = title;
}

QString PageAction::getParentTitle() const
{
	return _parentTitle;
}

void PageAction::setParentTitle(const QString& parentTitle)
{
	_parentTitle = parentTitle;
}

QString PageAction::getMetaData() const
{
	return _metaData;
}

void PageAction::setMetaData(const QString& metaData)
{
	_metaData = metaData;

	emit metadataChanged();
}

void PageAction::removeSubAction(const PageSubActionPtr& subAction)
{
    _subActions.erase(std::ranges::remove(_subActions, subAction).begin(), _subActions.end());

    emit subActionsChanged();
}

void PageAction::clearSubActions()
{
	_subActions.clear();

    emit subActionsChanged();
}

void PageAction::setEditorData(const QModelIndex& index)
{
}

bool PageAction::isCompactView()
{
    return PageAction::compactView;
}

void PageAction::setCompactView(bool compactView)
{
    PageAction::compactView = compactView;
}
