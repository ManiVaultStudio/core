#include "GlobalSettingsDialog.h"

#include "HdpsApplication.h"

#include <QVBoxLayout>

using namespace hdps;

#ifdef _DEBUG
    #define GLOBAL_SETTINGS_DIALOG_VERBOSE
#endif

GlobalSettingsDialog::GlobalSettingsDialog(QWidget* parent /*= nullptr*/) :
    QDialog(parent)
{
    setWindowIcon(Application::getIconFont("FontAwesome").getIcon("cogs"));
    setModal(true);
    setWindowTitle("Global settings");

    auto layout = new QVBoxLayout();

    layout->addWidget(HdpsApplication::getGlobalSettingsAction().createWidget(this));

    setLayout(layout);
}
