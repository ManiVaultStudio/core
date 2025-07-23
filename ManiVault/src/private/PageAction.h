// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QIcon>
#include <QImage>
#include <QList>
#include <QString>
#include <QModelIndex>

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

    using ClickedCallback = std::function<void()>;    /** Callback function that is called when the action row is clicked */

public:

    /**
     * Construct with \p icon, \p title, \p description and \p clickedCallback
     * @param icon Action icon
     * @param title Action title
     * @param subtitle Action subtitle
     * @param description Action description
     * @param tooltip Action tooltip
     * @param clickedCallback Callback function that is called when the action row is clicked
     */
    PageAction(const QIcon& icon, const QString& title, const QString& subtitle, const QString& description, const QString& tooltip, const ClickedCallback& clickedCallback);

    /**
     * Construct from model \p index
     * @param index Model index to set editor data from
     */
    PageAction(const QModelIndex& index);

public: // Getters and setters
    
    QIcon getIcon() const;

    void  setIcon(const QIcon& icon);

    void setExpanded(bool expanded);

    QString getTitle() const;

    void    setTitle(const QString& title);

    QString getParentTitle() const;

    void    setParentTitle(const QString& parentTitle);

    QString getComments() const { return _comments.isEmpty() ? "NA" : _comments; }
    void setComments(const QString& comments) { _comments = comments; }

    QString getSubtitle() const { return _subtitle.isEmpty() ? "NA" : _subtitle; }
    void setSubtitle(const QString& subtitle) { _subtitle = subtitle; }

    QString getMetaData() const;

    void setMetaData(const QString& metaData);

    QString getTooltip() const { return _tooltip; }
    void setTooltip(const QString& tooltip) { _tooltip = tooltip; }

    QStringList getDownloadUrls() const { return _downloadUrls; }
    void setDownloadUrls(const QStringList& downloadUrls) { _downloadUrls = downloadUrls; }

    ClickedCallback getClickedCallback() const { return _clickedCallback; }
    void setClickedCallback(const ClickedCallback& clickedCallback) { _clickedCallback = clickedCallback; }

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

        emit subActionsChanged();

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

    void iconChanged();
    void metadataChanged();
    void subActionsChanged();

private:
    QIcon                               _icon;              /** Action icon (shown on the left) */
    QString                             _title;             /** Title is shown next to the icon */
    QString                             _description;       /** Description is in the second row */
    QString                             _parentTitle;       /** Parent action title (if any) */
    QString                             _comments;          /** Comments are show on the top right */
    QStringList                         _tags;              /** Tags (might be empty) */
    QString                             _subtitle;          /** Subtitle */
    QString                             _metaData;          /** Metadata (might be empty) */
    QImage                              _previewImage;      /** Preview image (might be empty) */
    QString                             _tooltip;           /** Tooltip (might be empty) */
    QStringList                         _contributors;      /** List of contributors */
    QStringList                         _downloadUrls;      /** Action download URLs */
    ClickedCallback                     _clickedCallback;   /** Callback function that is called when the action row is clicked */
    PageSubActionPtrs                   _subActions;        /** Sub-actions that are shown when hovering the action */

    static bool compactView;

    friend class PageActionDelegateEditorWidget;  /** Friend class for accessing private members */
};

using PageActionSharedPtr     = std::shared_ptr<PageAction>;
using PageActionSharedPtrs    = std::vector<PageActionSharedPtr>;