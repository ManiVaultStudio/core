// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <util/HardwareSpec.h>

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
     * Construct with \p index, \p icon, \p tooltipCallback and \p clickedCallback
     * @param icon Action icon
     * @param tooltipCallback TooltipCallback that is invoked to retrieve the sub-action tooltip
     * @param clickedCallback ClickedCallback that is called when the sub-action is clicked
     */
    PageSubAction(const QIcon& icon, const TooltipCallback& tooltipCallback = {}, const ClickedCallback& clickedCallback = {});

    /** Add default virtual destructor */
    virtual ~PageSubAction() = default;

    /**
     * Get icon
     * @return Icon
     */
    QIcon getIcon() const { return _icon; }

    /**
     * Set the icon to \p icon
     * @param icon Icon to set
     */
    void setIcon(const QIcon& icon) { _icon = icon; }

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
    ClickedCallback         _clickedCallback;   /** Callback function that is called when the sub-action is clicked */
    TooltipCallback         _tooltipCallback;   /** Callback function that is invoked to retrieve the sub-action tooltip */

    static bool compactView;
};

/** For displaying comments */
class CommentsPageSubAction : public PageSubAction
{
public:

	/**
     * Construct with \p comments
	 * @param comments Comments to display
	 */
	CommentsPageSubAction(const QString& comments);
};

/** For displaying tags */
class TagsPageSubAction : public PageSubAction
{
public:

    /**
     * Construct with \p tags
     * @param tags List of tags
     */
    TagsPageSubAction(const QStringList& tags);
};

/** For displaying download size */
class PageDownloadSubAction : public PageSubAction
{
public:

    /**
     * Construct with \p downloadSize
     * @param downloadSize Download size to display
     */
    PageDownloadSubAction(const QString& downloadSize);
};

/** For displaying hardware compatibility information */
class PageCompatibilitySubAction : public PageSubAction
{
public:

    /**
     * Construct with \p downloadSize
     * @param systemCompatibilityInfo System compatibility information to display
     */
    PageCompatibilitySubAction(const mv::util::HardwareSpec::SystemCompatibilityInfo& systemCompatibilityInfo);
};

/** For displaying the project preview */
class ProjectPreviewPageSubAction : public PageSubAction
{
public:

    /**
     * Construct with \p 
     */
    ProjectPreviewPageSubAction();
};

/** For displaying the project contributors */
class ContributorsPageSubAction : public PageSubAction
{
public:

    /**
     * Construct with \p contributors
     * @param contributors List of contributors to display
     */
    ContributorsPageSubAction(const QStringList& contributors);
};

using PageSubActionPtr  = std::shared_ptr<PageSubAction>;
using PageSubActionPtrs = std::vector<PageSubActionPtr>;

