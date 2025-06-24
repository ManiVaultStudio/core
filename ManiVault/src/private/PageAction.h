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

/**
 * Page action
 *
 * Container class for page action.
 *
 * @author Thomas Kroes
 */
class PageAction final
{
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
    
    QIcon getIcon() const { return _icon; }
    void setIcon(const QIcon& icon) { _icon = icon; }

    QString getTitle() const { return _title.isEmpty() ? "NA" : _title; }
    void setTitle(const QString& title) { _title = title; }

    QString getDescription() const { return _description.isEmpty() ? "NA" : _description; }
    void setDescription(const QString& description) { _description = description; }

    QString getComments() const { return _comments.isEmpty() ? "NA" : _comments; }
    void setComments(const QString& comments) { _comments = comments; }

    QStringList getTags() const { return _tags; }
    void setTags(const QStringList& tags) { _tags = tags; }

    QString getSubtitle() const { return _subtitle.isEmpty() ? "NA" : _subtitle; }
    void setSubtitle(const QString& subtitle) { _subtitle = subtitle; }

    QString getMetaData() const { return _metaData; }
    void setMetaData(const QString& metaData) { _metaData = metaData; }

    QImage getPreviewImage() const { return _previewImage; }
    void setPreviewImage(const QImage& previewImage) { _previewImage = previewImage; }

    QString getTooltip() const { return _tooltip; }
    void setTooltip(const QString& tooltip) { _tooltip = tooltip; }

    QStringList getContributors() const { return _contributors; }
    void setContributors(const QStringList& contributors) { _contributors = contributors; }

    QStringList getDownloadUrls() const { return _downloadUrls; }
    void setDownloadUrls(const QStringList& downloadUrls) { _downloadUrls = downloadUrls; }

    ClickedCallback getClickedCallback() const { return _clickedCallback; }
    void setClickedCallback(const ClickedCallback& clickedCallback) { _clickedCallback = clickedCallback; }

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

protected:
    QIcon               _icon;              /** Action icon (shown on the left) */
    QString             _title;             /** Title is shown next to the icon */
    QString             _description;       /** Description is in the second row */
    QString             _comments;          /** Comments are show on the top right */
    QStringList         _tags;              /** Tags (might be empty) */
    QString             _subtitle;          /** Subtitle */
    QString             _metaData;          /** Metadata (might be empty) */
    QImage              _previewImage;      /** Preview image (might be empty) */
    QString             _tooltip;           /** Tooltip (might be empty) */
    QStringList         _contributors;      /** List of contributors */
    QStringList         _downloadUrls;      /** Action download URLs */
    ClickedCallback     _clickedCallback;   /** Callback function that is called when the action row is clicked */

    static bool compactView;
};

using PageActions = QList<PageAction>;