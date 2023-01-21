#pragma once

#include "StartPageAction.h"

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
     * Add start page action
     * @param startPageAction Start page action
     */
    void add(const StartPageAction& startPageAction);

    /** Resets the rows and notifies others */
    void reset();

    /**
     * Get whether the model has any comments
     * @return Boolean determining whether the model has any comments
     */
    bool hasComments() const;
};
