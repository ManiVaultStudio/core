// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QIcon>

/**
 * Page sub-action class
 *
 * Action related to its parent action.
 *
 * @author Thomas Kroes
 */
class PageSubAction
{
public:

    using ClickedCallback = std::function<void()>;      /** Callback function that is called when the sub-action is clicked */
    using TooltipCallback = std::function<QString()>;   /** Invoked to retrieve the sub-action tooltip */

    /**
     * Construct with \p index and \p icon
     * @param index Model index
     * @param icon Action icon
     */
    PageSubAction(const QModelIndex& index, const QIcon& icon);

    /**
     * Construct with \p index, \p icon, \p clickedCallback and \p tooltipCallback
     * @param index Model index
     * @param icon Action icon
     * @param clickedCallback ClickedCallback that is called when the sub-action is clicked
     * @param tooltipCallback TooltipCallback that is invoked to retrieve the sub-action tooltip
     */
    PageSubAction(const QModelIndex& index, const QIcon& icon, const ClickedCallback& clickedCallback, const TooltipCallback& tooltipCallback);

    /** Add default virtual destructor */
    virtual ~PageSubAction() = default;

	/**
     * Get the sub-action tooltip
     * @return Tooltip
     */
    QString getTooltip() const;

public: // Callbacks

	/**
     * Get the clicked callback
     * @return ClickedCallback that is called when the sub-action is clicked
     */
    ClickedCallback getClickedCallback() const { return _clickedCallback; }

    /**
     * Set the clicked callback to \p clickedCallback
     * @param clickedCallback ClickedCallback that is called when the sub-action is clicked
     */
	void setClickedCallback(const ClickedCallback& clickedCallback) { _clickedCallback = clickedCallback; }

    /**
     * Get the tooltip callback
     * @return TooltipCallback that is called when the sub-action is hovered
     */
    TooltipCallback getTooltipCallback() const { return _tooltipCallback; }

    /**
     * Set the tooltip callback to \p tooltipCallback
     * @param tooltipCallback Callback function that is invoked to retrieve the sub-action tooltip
     */
    void setTooltipCallback(const TooltipCallback& tooltipCallback) { _tooltipCallback = tooltipCallback; }

protected:
    QIcon                   _icon;              /** Action icon (shown on the left) */
    QPersistentModelIndex   _index;             /** Model index of the parent action */
    ClickedCallback         _clickedCallback;   /** Callback function that is called when the sub-action is clicked */
    TooltipCallback         _tooltipCallback;   /** Callback function that is invoked to retrieve the sub-action tooltip */

    static bool compactView;
};

/** For displaying comments */
class PageCommentsSubAction : public PageSubAction
{
public:

	/**
     * Construct with \p index
	 * @param index 
	 */
	PageCommentsSubAction(const QModelIndex& index);
};

using PageSubActionPtr  = std::shared_ptr<PageSubAction>;
using PageSubActionPtrs = std::vector<PageSubActionPtr>;

