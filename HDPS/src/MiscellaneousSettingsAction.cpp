#include "MiscellaneousSettingsAction.h"

namespace hdps
{

MiscellaneousSettingsAction::MiscellaneousSettingsAction(QObject* parent) :
    GlobalSettingsGroupAction(parent, "Miscellaneous"),
    _ignoreLoadingErrorsAction(this, "Ignore loading errors")
{
    setShowLabels(false);

    _ignoreLoadingErrorsAction.setSettingsPrefix(getSettingsPrefix() + "IgnoreLoadingErrors");
}

}
