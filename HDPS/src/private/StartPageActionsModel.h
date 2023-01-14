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

    /** Model columns */
    enum class Column {
        Icon,           /** Action icon */
        Title,          /** Action title */
        Description,    /** Action description */
        Summary         /** Shows a delegate item with icon, title and description */
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
     */
    void add(const QIcon& icon, const QString& title, const QString& description);
};
