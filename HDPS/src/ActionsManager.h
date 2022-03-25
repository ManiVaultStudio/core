#pragma once

#include <QObject>
#include <QMap>
#include <QString>
#include <QDebug>
#include <QAbstractListModel>

namespace hdps
{

namespace gui {
    class WidgetAction;
}

using namespace gui;

class ActionsManager : public QObject
{
    Q_OBJECT

public:

    class PublicActionsModel : public QAbstractListModel
    {
    public:

        /** Public action columns */
        enum Column {
            Name,                   /** Name of the public action */
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

    //private:
    //    QIcon getDecorationRole(const LineType& lineType) const;

    protected:
        QVector<WidgetAction*>  _publicActions;        /** Shared pointers to public widget actions */

        friend class ActionsManager;
};

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     */
    ActionsManager(QObject* parent = nullptr);

    /**
     * Add public action
     * @param publicAction Pointer to public action
     */
    void addPublicAction(WidgetAction* publicAction);

    /**
     * Remove public action
     * @param publicAction Pointer to public action
     */
    void removePublicAction(WidgetAction* publicAction);

    /**
     * Get public actions model
     * @return List model for public actions
     */
    const PublicActionsModel& getPublicActionsModel() const;

    /**
     * Establish whether an action is public
     * @param action Action to test
     * @return Boolean indicating whether an action is public or not
     */
    bool isActionPublic(const WidgetAction* action) const;

    /**
     * Establish whether an action is published as a public action
     * @param action Action to test
     * @return Boolean indicating whether an action is published as a public action or not
     */
    bool isActionPublished(const WidgetAction* action) const;

    /**
     * Establish whether an action is connected to a public action
     * @param action Action to test
     * @return Boolean indicating whether an action is connected to a public action or not
     */
    bool isActionConnected(const WidgetAction* action) const;

private:
    PublicActionsModel      _publicActionsModel;        /** List model for public actions */
};

}
