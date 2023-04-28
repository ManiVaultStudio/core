#pragma once

#include "ActionsModel.h"

#include <QStandardItemModel>

namespace hdps
{

/**
 * Actions list model class
 *
 * Standard item model class which contains a list of actions
 *
 * @author Thomas Kroes
 */
class ActionsListModel final : public ActionsModel
{
    Q_OBJECT

public:

    /**
     * Construct with \p parent object
     * @param parent Pointer to parent object
     */
    ActionsListModel(QObject* parent = nullptr);
};

}
