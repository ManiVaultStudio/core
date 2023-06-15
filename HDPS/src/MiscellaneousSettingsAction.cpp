#include "MiscellaneousSettingsAction.h"
#include "Application.h"

namespace hdps
{

MiscellaneousSettingsAction::MiscellaneousSettingsAction(QObject* parent) :
    GlobalSettingsGroupAction(parent, "Miscellaneous"),
    _ignoreLoadingErrorsAction(this, "Ignore loading errors")
{
    setShowLabels(false);

    addAction(&_ignoreLoadingErrorsAction);

    _ignoreLoadingErrorsAction.setSettingsPrefix(getSettingsPrefix() + "IgnoreLoadingErrors");
}

}
