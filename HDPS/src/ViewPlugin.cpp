#include "ViewPlugin.h"

#include <QWidget>

namespace hdps
{

namespace plugin
{

ViewPlugin::ViewPlugin(const PluginFactory* factory) :
    Plugin(factory),
    _widget(),
    _editActionsAction(&_widget, "Edit view plugin actions"),
    _mayCloseAction(this, "May close", true, true)
{
    setText(getGuiName());

    _widget.addAction(&_editActionsAction);

    _editActionsAction.setShortcut(tr("F12"));
    _editActionsAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);

    _mayCloseAction.setToolTip("Determines whether this view plugin may be closed or not");
    _mayCloseAction.setConnectionPermissions(WidgetAction::None);

    connect(&_editActionsAction, &TriggerAction::triggered, this, [this]() -> void {
        ViewPluginEditorDialog viewPluginEditorDialog(nullptr, this);
        viewPluginEditorDialog.exec();
    });
}

void ViewPlugin::setObjectName(const QString& name)
{
    QObject::setObjectName("Plugins/View/" + name);
}

void ViewPlugin::loadData(const Datasets& datasets)
{
    qDebug() << "Load function not implemented in view plugin implementation";
}

ViewPluginFactory::ViewPluginFactory() :
    PluginFactory(Type::VIEW)
{

}

QIcon ViewPluginFactory::getIcon(const QColor& color /*= Qt::black*/) const
{
    return Application::getIconFont("FontAwesome").getIcon("eye", color);
}

}

}
