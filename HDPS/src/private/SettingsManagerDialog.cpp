#include "SettingsManagerDialog.h"

#include <Application.h>

#include <QVBoxLayout>

using namespace hdps;

#ifdef _DEBUG
    #define SETTINGS_MANAGER_DIALOG_VERBOSE
#endif

namespace hdps::gui {

SettingsManagerDialog::SettingsManagerDialog(QWidget* parent /*= nullptr*/) :
    QDialog(parent)
{
    setWindowIcon(Application::getIconFont("FontAwesome").getIcon("cogs"));
    setModal(true);
    setWindowTitle("Settings");

    auto layout = new QVBoxLayout();

    setLayout(layout);
}

}
