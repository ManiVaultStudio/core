#pragma once

#include <QStandardItemModel>

//using namespace hdps;
//using namespace hdps::gui;

/**
 * Public actions model class
 *
 * Model class for storing public actions.
 *
 * @author Thomas Kroes
 */
class PublicActionsModel : public QStandardItemModel
{
    Q_OBJECT

public:

    /**
     * Construct with \p parent object
     * @param parent Pointer to parent object
     */
    PublicActionsModel(QObject* parent = nullptr);

private:
};
