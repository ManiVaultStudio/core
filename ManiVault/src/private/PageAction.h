// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QIcon>
#include <QImage>
#include <QList>
#include <QString>

#include <functional>

#include "PageSubAction.h"

/**
 * Page action
 *
 * Container class for page action.
 *
 * @author Thomas Kroes
 */
class PageAction final : public QObject
{
    Q_OBJECT

public:

    /** Callback function that is called when the action row is clicked */
    using ClickedCallback = std::function<void()>;

    /**
     * Construct with \p icon, \p title, \p description and \p clickedCallback
     * @param icon Action icon
     * @param title Action title
     * @param subtitle Action subtitle
     * @param comments Action description
     * @param tooltip Action tooltip
     * @param clickedCallback Callback function that is called when the action row is clicked
     */
    PageAction(const QIcon& icon, const QString& title, const QString& subtitle, const QString& comments, const QString& tooltip, const ClickedCallback& clickedCallback);

    /**
     * Construct from model \p index
     * @param index Model index to set editor data from
     */
    PageAction(const QModelIndex& index);

public: // Getters and setters

    /**
     * Get action icon
     * @return Action icon
     */
    QIcon getIcon() const;

    /**
     * Set action icon to \p icon
     * @param icon Action icon
     */
    void setIcon(const QIcon& icon);

    /**
     * Get action title
     * @return Action title
     */
    QString getTitle() const;

    /**
     * Set action title to \p title
     * @param title Action title
     */
    void setTitle(const QString& title);

    /**
     * Get subtitle
     * @return Action subtitle
     */
    QString getSubtitle() const;

    /**
     * Set action subtitle to \p subtitle
     * @param subtitle Action subtitle
     */
    void setSubtitle(const QString& subtitle);

    /**
     * Get comments
     * @return Action comments
     */
    QString getComments() const;

    /**
     * Set action comments to \p comments
     * @param comments Action comments
     */
    void setComments(const QString& comments);

    /**
     * Get action tooltip
     * @return Action tooltip
     */
    QString getTooltip() const;

    /**
     * Set action tooltip to \p tooltip
     * @param tooltip Action tooltip
     */
    void setTooltip(const QString& tooltip);

    /**
     * Get action clicked callback
     * @return Action clicked callback
     */
    ClickedCallback getClickedCallback() const;

    /**
     * Set action clicked callback to \p clickedCallback
     * @param clickedCallback Action clicked callback
     */
    void setClickedCallback(const ClickedCallback& clickedCallback);

    /**
     * Get action metadata
     * @return Action metadata
     */
    QString getMetaData() const;

    /**
     * Set action metadata to \p metaData
     * @param metaData Action metadata
     */
    void setMetaData(const QString& metaData);

    /**
     * Get parent action title
     * @return Parent action title
     */
    QString getParentTitle() const;

    /**
     * Set parent action title to \p parentTitle
     * @param parentTitle Parent action title
     */
    void setParentTitle(const QString& parentTitle);

    /**
     * Set whether the action row is expanded
     * @param expanded Boolean determining whether the action row is expanded
     */
    void setExpanded(bool expanded);

public: // Sub-actions

    /**
     * Create subAction of \p SubActionType with arguments \p args and add it to the sub-actions list
     * @param args Arguments to pass to the sub-action constructor
     */
    template<typename SubActionType, typename... Args>
    std::shared_ptr<SubActionType> createSubAction(Args&&... args)
    {
        auto sharedSubAction = std::make_shared<SubActionType>(std::forward<Args>(args)...);

        _subActions.push_back(sharedSubAction);

        emit subActionsChanged(_subActions);

        return sharedSubAction;
    }

    /**
     * Remove \p subAction
     * @param subAction Shared pointer to sub-action
     */
    void removeSubAction(const PageSubActionPtr& subAction);

    /** Remove all sub-actions */
    void clearSubActions();

protected: // Sub-actions

    /**
     * Get sub-actions
     * @return List of sub-actions
     */
    const PageSubActionPtrs& getSubActions() const { return _subActions; }

	/**
     * Get sub-actions
     * @return List of sub-actions
     */
    PageSubActionPtrs& getSubActions() { return _subActions; }

private:
    
    /**
     * Set editor data from model \p index
     * @param index Model index to set editor data from
     */
    void setEditorData(const QModelIndex& index);

public:

    /**
     * Get whether compact view is enabled
     * @return Boolean determining whether to show actions in compact view mode
     */
    static bool isCompactView();

    /**
     * Toggles compact view
     * @param compactView Boolean determining whether to show actions in compact view mode
     */
    static void setCompactView(bool compactView);

signals:

    /**
     * Signals that the icon changed to \p icon
     * @param icon New action icon
     */
    void iconChanged(const QIcon& icon);

    /**
     * Signals that the title changed to \p title
     * @param title New action title
     */
    void titleChanged(const QString& title);

    /**
     * Signals that the subtitle changed to \p subtitle
     * @param subtitle New action subtitle
     */
    void subtitleChanged(const QString& subtitle);

    /**
     * Signals that the comments changed to \p comments
     * @param comments New action comments
     */
    void commentsChanged(const QString& comments);

    /**
     * Signals that the tooltip changed to \p tooltip
     * @param tooltip New action tooltip
     */
    void tooltipChanged(const QString& tooltip);

    /**
     * Signals that the metadata changed to \p metadata
     * @param metadata New action metadata
     */
    void metadataChanged(const QString& metadata);

    /**
     * Signals that the parentTitle changed to \p parentTitle
     * @param parentTitle New action parentTitle
     */
    void parentTitleChanged(const QString& parentTitle);

    /**
     * Signals that the sub-actions list changed to \p subActions
     * @param subActions New list of sub-actions
     */
    void subActionsChanged(const PageSubActionPtrs& subActions);

    /**
     * Signal that is emitted when the action row is clicked
     * @param expanded Boolean determining whether the action row is expanded
     */
    void expandedChanged(bool expanded);

private:
    QIcon               _icon;              /** Action icon (shown on the left) */
    QString             _title;             /** Title is shown next to the icon */
    QString             _subtitle;          /** Subtitle */
    QString             _comments;          /** Comments are show on the top right */
    QString             _tooltip;           /** Tooltip (might be empty) */
    ClickedCallback     _clickedCallback;   /** Callback function that is called when the action row is clicked */
    QStringList         _tags;              /** Tags (might be empty) */
    QString             _metaData;          /** Metadata (might be empty) */
    QImage              _previewImage;      /** Preview image (might be empty) */
    QStringList         _contributors;      /** List of contributors */
    QStringList         _downloadUrls;      /** Action download URLs */
    PageSubActionPtrs   _subActions;        /** Sub-actions that are shown when hovering the action */
    QString             _parentTitle;       /** Parent action title (if any) */

    static bool compactView;

    friend class PageActionDelegateEditorWidget;  /** Friend class for accessing private members */
};

using PageActionSharedPtr     = std::shared_ptr<PageAction>;
using PageActionSharedPtrs    = std::vector<PageActionSharedPtr>;