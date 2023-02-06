#pragma once

#include <QStandardItemModel>

namespace hdps {
    namespace gui {
        class WidgetAction;
    }
}

/**
 * Public actions model class
 *
 * Model class for storing public actions.
 *
 * @author Thomas Kroes
 */
class PublicActionsModel final : public QStandardItemModel
{
    Q_OBJECT

public:

    /** Public action model columns */
    enum class Column {
        Name,   /** Name of the public action */
        ID,     /** Globally unique identifier of the action */

        Count
    };

    /** Column name and tooltip */
    static QMap<Column, QPair<QString, QString>> columnInfo;

public:

    /**
     * Construct with \p parent object
     * @param parent Pointer to parent object
     */
    PublicActionsModel(QObject* parent = nullptr);

private:

    /**
     * Add public widget \p action to the model
     * @param action Pointer to public widget action to add
     */
    void addPublicAction(hdps::gui::WidgetAction* action);

    /**
     * Remove widget \p action from the model
     * @param action Pointer to widget action to remove
     */
    void removePublicAction(hdps::gui::WidgetAction* action);
};
