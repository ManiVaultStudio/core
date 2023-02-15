#include "ActionsModel.h"
#include "AbstractActionsManager.h"

#include <actions/WidgetAction.h>

#include <Application.h>

using namespace hdps::gui;

#ifdef _DEBUG
    //#define ACTIONS_MODEL_VERBOSE
#endif

namespace hdps
{
    class ScopeItem : public QStandardItem {
    public:
        ScopeItem(WidgetAction* action) :
            _action(action)
        {
        }

        QVariant data(int role = Qt::UserRole + 1) const override {
            if (_action.isNull())
                return QVariant();

            return _action->isPublic() ? "Public" : "Private";
        }

    private:
        QPointer<WidgetAction> _action;
    };

ActionsModel::ActionsModel(QObject* parent /*= nullptr*/) :
    QStandardItemModel(parent)
{
    setColumnCount(static_cast<int>(Column::Count));

    setHeaderData(static_cast<int>(Column::Name), Qt::Horizontal, "Name");
    setHeaderData(static_cast<int>(Column::ID), Qt::Horizontal, "ID");
    setHeaderData(static_cast<int>(Column::Type), Qt::Horizontal, "Type");
    setHeaderData(static_cast<int>(Column::Scope), Qt::Horizontal, "Scope");

    setHeaderData(static_cast<int>(Column::Name), Qt::Horizontal, "Name of the parameter", Qt::ToolTipRole);
    setHeaderData(static_cast<int>(Column::ID), Qt::Horizontal, "Globally unique identifier of the parameter", Qt::ToolTipRole);
    setHeaderData(static_cast<int>(Column::Type), Qt::Horizontal, "Type of parameter", Qt::ToolTipRole);
    setHeaderData(static_cast<int>(Column::Scope), Qt::Horizontal, "Whether the parameter is public or private", Qt::ToolTipRole);
}

void ActionsModel::addAction(WidgetAction* action)
{
#ifdef ACTIONS_MODEL_VERBOSE
    qDebug() << __FUNCTION__ << action->text();
#endif

    connect(action, &WidgetAction::changed, this, [this, action]() -> void {
        const auto matches = match(index(0, static_cast<int>(Column::ID), QModelIndex()), Qt::DisplayRole, action->getId(), -1, Qt::MatchFlag::MatchRecursive);
            
        if (matches.count() != 1)
            return;

        setData(matches.first().siblingAtColumn(static_cast<int>(Column::Name)), QVariant::fromValue(action->text()), Qt::EditRole);
    });

    const auto createRow = [this, action]() -> QList<QStandardItem*> {
        const QList<QStandardItem*> standardItems {
            new QStandardItem(action->text()),
            new QStandardItem(action->getId()),
            new QStandardItem(action->getTypeString()),
            new ScopeItem(action)
        };

        for (auto standardItem : standardItems)
            standardItem->setData(QVariant::fromValue(action), Qt::UserRole + 1);

        return standardItems;
    };

    appendRow(createRow());
}

void ActionsModel::removeAction(WidgetAction* action)
{
#ifdef ACTIONS_MODEL_VERBOSE
    qDebug() << __FUNCTION__ << action->getId();
#endif

    const auto matches = match(index(0, static_cast<int>(Column::ID), QModelIndex()), Qt::DisplayRole, action->getId(), -1, Qt::MatchFlag::MatchRecursive);

    if (matches.isEmpty())
        return;

    //for (const auto& match : matches)
    //    removeRow(match.row(), match.parent());
}

}
