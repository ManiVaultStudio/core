#include "GlobalSettingsGroupAction.h"

namespace hdps
{

using namespace gui;

GlobalSettingsGroupAction::GlobalSettingsGroupAction(QObject* parent, const QString& title, bool expanded /*= true*/) :
    GroupAction(parent, title, expanded)
{
}

QString GlobalSettingsGroupAction::getSettingsPrefix() const
{
    return QString("GlobalSettings/%1/").arg(text());
}

}
