// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <util/HardwareSpec.h>

#include <widgets/IconLabel.h>
#include <models/ProjectsModelProject.h>

#include <QIcon>
#include <QImage>

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
    virtual ~PageSubAction();

    /**
     * Set the icon to \p icon
     * @param icon Icon to set
     */
    void setIcon(const QIcon& icon);

    /**
     * Get the sub-action tooltip
     * @return Tooltip
     */
    QString getTooltip() const;

    /**
     * Set the sub-action visibility to \p visible
     * @param visible Sub-action visibility
     */
    void setVisible(bool visible);

    /**
     * Get the sub-action icon label
     * @return Sub-action icon label
     */
    mv::gui::IconLabel* getIconLabel();

public: // Callbacks

    /**
     * Get the clicked callback
     * @return ClickedCallback that is called when the sub-action is clicked
     */
    ClickedCallback getClickedCallback() const;

    /**
     * Set the clicked callback to \p clickedCallback
     * @param clickedCallback ClickedCallback that is called when the sub-action is clicked
     */
    void setClickedCallback(const ClickedCallback& clickedCallback);

    /**
     * Get the tooltip callback
     * @return TooltipCallback that is called when the sub-action is hovered
     */
    TooltipCallback getTooltipCallback() const;

    /**
     * Set the tooltip callback to \p tooltipCallback
     * @param tooltipCallback Callback function that is invoked to retrieve the sub-action tooltip
     */
    void setTooltipCallback(const TooltipCallback& tooltipCallback);

protected:
    QPointer<mv::gui::IconLabel>    _iconLabel;         /** Icon label representing the sub-action */

    static bool compactView;
};

/** For purging a downloaded project */
class ProjectPurgePageSubAction : public PageSubAction
{
public:

    /**
     * Construct shared pointer to the \p project to purge
     * @param project Shared pointer to the project to purge
     */
    ProjectPurgePageSubAction(mv::util::ProjectsModelProjectSharedPtr project);

private:
    mv::util::ProjectsModelProjectSharedPtr     _project;   /** Shared pointer to the project to purge */
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
class ProjectCompatibilityPageSubAction : public PageSubAction
{
public:

    /**
     * Construct with \p downloadSize
     * @param systemCompatibilityInfo System compatibility information to display
     */
    ProjectCompatibilityPageSubAction(const mv::util::HardwareSpec::SystemCompatibilityInfo& systemCompatibilityInfo);
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

/** For displaying the project JSON URL */
class ProjectsJsonUrlPageSubAction : public PageSubAction
{
public:

    /**
     * Construct with \p projectJsonUrl
     * @param projectJsonUrl Project JSON URL to display
     */
    ProjectsJsonUrlPageSubAction(const QUrl& projectJsonUrl);
};

/** For displaying the project plugins */
class ProjectPluginsPageSubAction : public PageSubAction
{
public:

    /**
     * Construct with \p plugins
     * @param plugins Project plugins to display
     */
    ProjectPluginsPageSubAction(const QStringList& plugins);
};

/** For displaying when the project was last updated on the server */
class ProjectLastUpdatedPageSubAction : public PageSubAction
{
public:

    /**
     * Construct with \p projectLastUpdated
     * @param projectLastUpdated When the project was last updated on the server
     */
    ProjectLastUpdatedPageSubAction(const QDateTime& projectLastUpdated = QDateTime());

    /**
     * Set the project last updated to \p projectLastUpdated
     * @param projectLastUpdated When the project was last updated on the server
     */
    void setProjectLastUpdated(const QDateTime& projectLastUpdated);

private:
    QDateTime   _projectLastUpdated;    /** When the project was last updated on the server */
};

/** For displaying project preview */
class ProjectPreviewPageSubAction : public PageSubAction
{
public:

    /**
     * Construct with \p previewImage
     * @param previewImage Project preview image to display
     */
    ProjectPreviewPageSubAction(const QImage& previewImage);

    /**
     * Set the project preview image to \p previewImage
     * @param previewImage Project preview image to display
     */
    void setPreviewImage(const QImage& previewImage);

private:
    QImage   _previewImage;    /** Preview image */
};

/** For displaying project info through proxy */
class ProxyPageSubAction : public PageSubAction
{
public:

    /**
     * Construct with \p tooltipCallback
     * @param icon Icon for the sub-action
     * @param tooltipCallback Callback function that is invoked to retrieve the sub-action tooltip
     */
    ProxyPageSubAction(const QIcon& icon, const TooltipCallback& tooltipCallback);
};

using PageSubActionPtr  = std::shared_ptr<PageSubAction>;
using PageSubActionPtrs = std::vector<PageSubActionPtr>;

