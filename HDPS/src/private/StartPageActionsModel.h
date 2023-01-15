#pragma once

#include <QStandardItemModel>

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

    using ClickedCB = std::function<void()>;

    /** Model columns */
    enum class Column {
        Icon,               /** Action icon */
        Title,              /** Action title */
        Description,        /** Action description */
        Comments,           /** Action comments */
        Callback,           /** Callback which is called when the action is clicked */
        SummaryDelegate,    /** Delegate item with title and subtitle */
        CommentsDelegate    /** Delegate item with title */
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
     * @param tooltip Action tooltip
     * @param clickedCallback Callback which is called when the action is clicked
     */
    void add(const QIcon& icon, const QString& title, const QString& description, const QString& comments, const QString& tooltip, const ClickedCB& clickedCallback);

    /**
     * Get whether the model has any comments
     * @return Boolean determining whether the model has any comments
     */
    bool hasComments() const;
};
