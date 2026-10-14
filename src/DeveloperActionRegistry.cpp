#include "DeveloperActionRegistry.h"

#include <QHash>
#include <QMutex>
#include <QMutexLocker>

namespace mv::developer
{
namespace
{
QHash<QString, MenuPopulator>& populators()
{
    static QHash<QString, MenuPopulator> value;

    return value;
}

QMutex& mutex()
{
    static QMutex value;

    return value;
}
}

void registerMenuPopulator(const QString& id, MenuPopulator populator)
{
    if (id.isEmpty() || !populator)
        return;

    QMutexLocker locker(&mutex());

    populators().insert(id, std::move(populator));
}

void populateMenu(QMenu& menu, QWidget* owner)
{
    QHash<QString, MenuPopulator> copy;
    {
        QMutexLocker locker(&mutex());
        copy = populators();
    }

    for (const auto& populator : copy)
    {
        populator(menu, owner);
    }
}
}
