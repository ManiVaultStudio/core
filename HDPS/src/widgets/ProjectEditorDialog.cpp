#include "ViewPluginEditorDialog.h"
#include "ViewPlugin.h"

#include <Application.h>

#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>

using namespace hdps;
using namespace hdps::plugin;

ViewPluginEditorDialog::ViewPluginEditorDialog(QWidget* parent, ViewPlugin* viewPlugin) :
    QDialog(parent),
    _viewPlugin(viewPlugin),
    _actionHierarchyWidget(this, viewPlugin)
{
    setWindowIcon(Application::getIconFont("FontAwesome").getIcon("cog"));
    setModal(true);
    setWindowTitle(QString("Edit view (%1)").arg(_viewPlugin->text()));
    setMinimumSize(QSize(320, 240));

    auto layout = new QVBoxLayout();

    layout->addWidget(&_actionHierarchyWidget);
    layout->addWidget(_viewPlugin->getMayCloseAction().createWidget(this));
    layout->addWidget(_viewPlugin->getMayFloatAction().createWidget(this));
    layout->addWidget(_viewPlugin->getGuiNameAction().createWidget(this));

    setLayout(layout);

    auto dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok);

    layout->addWidget(dialogButtonBox);

    connect(dialogButtonBox, &QDialogButtonBox::accepted, this, &ViewPluginEditorDialog::accept);
}
