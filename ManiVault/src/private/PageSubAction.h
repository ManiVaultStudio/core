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
class PageSubAction final
{
public:

    using ClickedCallback = std::function<void()>;    /** Callback function that is called when the sub-action is clicked */

public:

    /**
     * Construct with \p index and \p icon
     * @param index Model index
     * @param icon Action icon
     */
    PageSubAction(const QModelIndex& index, const QIcon& icon);

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

protected:
    QIcon               _icon;              /** Action icon (shown on the left) */
    const QModelIndex   _index;             /** Model index of the parent action */
    ClickedCallback     _clickedCallback;   /** Callback function that is called when the sub-action is clicked */

    static bool compactView;
};

using PageSubActionPtr  = std::shared_ptr<PageSubAction>;
using PageSubActionPtrs = std::vector<PageSubActionPtr>;
