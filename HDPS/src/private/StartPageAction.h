#pragma once

#include <QIcon>
#include <QString>
#include <QStringList>
#include <QImage>
#include <QModelIndex>

/**
 * Start page action
 *
 * Container class for start page action.
 *
 * @author Thomas Kroes
 */
class StartPageAction final
{
public:

    using ClickedCallback = std::function<void()>;    /** Callback function that is called when the action row is clicked */

public:

    /**
     * Construct with \p icon, \p title, \p description and \p clickedCallback
     * @param icon Action icon
     * @param title Action title
     * @param description Action description
     * @param tooltip Action tooltip
     * @param clickedCallback Callback function that is called when the action row is clicked
     */
    StartPageAction(const QIcon& icon, const QString& title, const QString& description, const QString& tooltip, const ClickedCallback& clickedCallback);

    /**
     * Construct from model \p index
     * @param index Model index to set editor data from
     */
    StartPageAction(const QModelIndex& index);

public: // Getters and setters
    
    QIcon getIcon() const { return _icon; }
    void setIcon(const QIcon& icon) { _icon = icon; }

    QString getTitle() const { return _title; }
    void setTitle(const QString& title) { _title = title; }

    QString getDescription() const { return _description; }
    void setDescription(const QString& description) { _description = description; }

    QString getComments() const { return _comments; }
    void setComments(const QString& comments) { _comments = comments; }

    QStringList getTags() const { return _tags; }
    void setTags(const QStringList& tags) { _tags = tags; }

    QImage getPreviewImage() const { return _previewImage; }
    void setPreviewImage(const QImage& previewImage) { _previewImage = previewImage; }

    QString getTooltip() const { return _tooltip; }
    void setTooltip(const QString& tooltip) { _tooltip = tooltip; }

    ClickedCallback getClickedCallback() const { return _clickedCallback; }
    void setClickedCallback(const ClickedCallback& clickedCallback) { _clickedCallback = clickedCallback; }

private:
    
    /**
     * Set editor data from model \p index
     * @param index Model index to set editor data from
     */
    void setEditorData(const QModelIndex& index);

protected:
    QIcon               _icon;              /** Action icon (shown on the left) */
    QString             _title;             /** Title is shown next to the icon */
    QString             _description;       /** Description is in the second row */
    QString             _comments;          /** Comments are show on the top right */
    QStringList         _tags;              /** Tags (might be empty) */
    QImage              _previewImage;      /** Preview image (might be empty) */
    QString             _tooltip;           /** Tooltip (might be empty) */
    ClickedCallback     _clickedCallback;   /** Callback function that is called when the action row is clicked */
};
