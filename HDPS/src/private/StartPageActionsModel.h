#pragma once

#include <QStandardItemModel>
#include <QImage>

/**
 * Start page actions model class
 *
 * Model class which contains actions for the start page.
 *
 * @author Thomas Kroes
 */
class StartPageActionsModel final : public QStandardItemModel
{
public:

    using ClickedCB = std::function<void()>;    /** Callback function that is called when the action row is clicked */

    /** Model columns */
    enum class Column {
        Icon,               /** Action icon */
        Title,              /** Action title */
        Description,        /** Action description */
        Comments,           /** Action comments */
        Tags,               /** Action tags */
        PreviewImage,       /** Action preview image */
        Tooltip,            /** Action tooltip */
        ClickedCallback,    /** Callback which is called when the action is clicked */
        SummaryDelegate     /** Delegate item with title and subtitle */
    };

    /** Column name and tooltip */
    static QMap<Column, QPair<QString, QString>> columns;

public:

    /**
     * Construct start page actions model with \p parent object
     * @param parent Pointer to parent object
     */
    StartPageActionsModel(QObject* parent = nullptr);

    /**
     * Add action with \p icon, \p name and \p title
     * @param icon Action icon
     * @param title Action title
     * @param description Action description
     * @param comments Action comments
     * @param tags Action tags
     * @param previewImage Action preview image
     * @param tooltip Action tooltip
     * @param clickedCallback Callback function that is called when the action row is clicked
     * @param tooltipCallback Callback function that is called when a tooltip is required
     */
    void add(const QIcon& icon, const QString& title, const QString& description, const QString& comments, const QStringList& tags, const QImage& previewImage, const QString& tooltip, const ClickedCB& clickedCallback);

    /** Resets the rows and notifies others */
    void reset();

    /**
     * Get whether the model has any comments
     * @return Boolean determining whether the model has any comments
     */
    bool hasComments() const;
};
