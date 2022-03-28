#pragma once

#include <QAbstractListModel>

namespace hdps
{

namespace gui {
    class WidgetAction;
}

using namespace gui;

class ActionsModel : public QAbstractListModel
{
public:

    /** Public action columns */
    enum Column {
        Name,                   /** Name of the public action */
        Type,                   /** Public action type string */
        IsPublic,               /** Whether the action is public or private */
        NumberOfConnections     /** Number of private actions connected to the public action */
    };

public:

    /**
     * Returns the number of rows in the model given the parent model index
     * @param parent Parent model index
     * @return Number of rows in the model given the parent model index
     */
    int rowCount(const QModelIndex& parent = QModelIndex()) const;

    /**
     * Returns the number of columns in the model given the parent model index
     * @param parent Parent model index
     * @return Number of columns in the model given the parent model index
     */
    int columnCount(const QModelIndex& parent = QModelIndex()) const;

    /**
     * Returns the model index for the given row, column and parent model index
     * @param row Row
     * @param column Column
     * @param parent Parent model index
     * @return Model index
     */
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;

    /**
     * Returns the data for the given model index and data role
     * @param index Model index
     * @param role Data role
     * @return Data in variant form
     */
    QVariant data(const QModelIndex& index, int role) const;

    /**
     * Get header data
     * @param section Section
     * @param orientation Orientation
     * @param role Data role
     * @return Header
     */
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    /**
     * Add public action to the model
     * @param publicAction Pointer to public action
     */
    void addPublicAction(WidgetAction* publicAction);

    /**
     * Remove public action
     * @param publicAction Pointer to public action
     */
    void removePublicAction(WidgetAction* publicAction);

protected:
    QVector<WidgetAction*>  _publicActions;        /** Shared pointers to public widget actions */

    friend class ActionsManager;
};

}
