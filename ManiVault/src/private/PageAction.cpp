// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PageAction.h"

using namespace mv::util;

bool PageAction::compactView = false;

PageAction::PageAction(const QIcon& icon, const QString& title, const QString& subtitle, const QString& tooltip, const ClickedCallback& clickedCallback) :
    _icon(icon),
    _title(title),
    _subtitle(subtitle),
    _tooltip(tooltip),
	_clickedCallback(clickedCallback)
{
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

	emit iconChanged(_icon);
}

QString PageAction::getTitle() const
{
	return _title.isEmpty() ? "NA" : _title;
}

void PageAction::setTitle(const QString& title)
{
    if (title != _title) {
	    _title = title;

    	emit titleChanged(_title);
    }
}

QString PageAction::getSubtitle() const
{
    return _subtitle.isEmpty() ? "NA" : _subtitle;
}

void PageAction::setSubtitle(const QString& subtitle)
{
    if (subtitle != _subtitle) {
	    _subtitle = subtitle;

    	emit subtitleChanged(_subtitle);
    }
}

QString PageAction::getTooltip() const
{
    return _tooltip;
}

void PageAction::setTooltip(const QString& tooltip)
{
    if (tooltip != _tooltip) {
    	_tooltip = tooltip;

    	emit tooltipChanged(_tooltip);
    }
}

PageAction::ClickedCallback PageAction::getClickedCallback() const
{
    return _clickedCallback;
}

void PageAction::setClickedCallback(const ClickedCallback& clickedCallback)
{
    _clickedCallback = clickedCallback;
}

QString PageAction::getMetaData() const
{
	return _metaData;
}

void PageAction::setMetaData(const QString& metaData)
{
    if (metaData != _metaData) {
	    _metaData = metaData;

    	emit metadataChanged(_metaData);
    }
}

QString PageAction::getParentTitle() const
{
    return _parentTitle;
}

void PageAction::setParentTitle(const QString& parentTitle)
{
    if (parentTitle != _parentTitle) {
	    _parentTitle = parentTitle;

    	emit parentTitleChanged(_parentTitle);
    }
}

void PageAction::removeSubAction(const PageSubActionPtr& subAction)
{
    _subActions.erase(std::ranges::remove(_subActions, subAction).begin(), _subActions.end());

    emit subActionsChanged(_subActions);
}

void PageAction::clearSubActions()
{
	_subActions.clear();

    emit subActionsChanged(_subActions);
}

void PageAction::setExpanded(bool expanded)
{
    emit expandedChanged(expanded);
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
