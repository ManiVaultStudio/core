#include "RecentFilePathsAction.h"
#include "Application.h"

#include <QHBoxLayout>
#include <QMenu>

namespace hdps::gui {

RecentFilePathsAction::RecentFilePathsAction(QObject* parent, const QString& settingsKey /*= ""*/, const QString& fileType /*= ""*/, const QIcon& icon /*= QIcon()*/) :
    WidgetAction(parent),
    _settingsKey(),
    _fileType(),
    _icon(),
    _model(this)
{
    initialize(settingsKey, fileType, icon);
}

RecentFilePathsAction::Widget::Widget(QWidget* parent, RecentFilePathsAction* recentFilePathsAction, const std::int32_t& widgetFlags) :
    WidgetActionWidget(parent, recentFilePathsAction, widgetFlags),
    _recentFilePathsAction(recentFilePathsAction)
{
    //connect(this, &QPushButton::clicked, this, [this, triggerAction]() {
    //    triggerAction->trigger();
    //});

    //const auto update = [this, triggerAction, widgetFlags]() -> void {
    //    QSignalBlocker blocker(this);

    //    setEnabled(triggerAction->isEnabled());

    //    if (widgetFlags & WidgetFlag::Text)
    //        setText(triggerAction->text());

    //    if (widgetFlags & WidgetFlag::Icon) {
    //        setIcon(triggerAction->icon());

    //        if ((widgetFlags & WidgetFlag::Text) == 0)
    //            setProperty("class", "square-button");
    //    }

    //    setToolTip(triggerAction->toolTip());
    //    setVisible(triggerAction->isVisible());
    //};

    //connect(triggerAction, &QAction::changed, this, [this, update]() {
    //    update();
    //});

    //update();
}

QString RecentFilePathsAction::getTypeString() const
{
    return "RecentFilePaths";
}

QMenu* RecentFilePathsAction::getMenu()
{
    auto menu = new QMenu();

    menu->setTitle("Recent...");
    menu->setToolTip(QString("Recently opened %1s").arg(_fileType.toLower()));
    menu->setIcon(Application::getIconFont("FontAwesome").getIcon("clock"));
    menu->setEnabled(_model.rowCount() >= 1);

    for (auto action : _model.getActions())
        menu->addAction(action);
    
    return menu;
}

QString RecentFilePathsAction::getSettingsKey() const
{
    return _settingsKey;
}

QString RecentFilePathsAction::getFileType() const
{
    return _fileType;
}

QIcon RecentFilePathsAction::getIcon() const
{
    return _icon;
}

void RecentFilePathsAction::addRecentFilePath(const QString& filePath)
{
    _model.addRecentFilePath(filePath);
}

void RecentFilePathsAction::initialize(const QString& settingsKey, const QString& fileType, const QIcon& icon)
{
    _settingsKey    = settingsKey;
    _fileType       = fileType;
    _icon           = icon;

    _model.loadFromSettings();
}

RecentFilePathsAction::Model::Model(RecentFilePathsAction* recentFilePathsAction) :
    QStandardItemModel(),
    _recentFilePathsAction(recentFilePathsAction),
    _actions()
{
    setHeaderData(0, Qt::Horizontal, "File path");
    setHeaderData(1, Qt::Horizontal, "Date/time");

    setColumnCount(2);
}

void RecentFilePathsAction::Model::loadFromSettings()
{
    clear();

    for (auto action : _actions)
        delete action;

    _actions.clear();

    const auto recentFilePaths = Application::current()->getSetting(_recentFilePathsAction->getSettingsKey(), QVariantList()).toList();

    for (const auto& recentFilePath : recentFilePaths) {
        const auto filePath = recentFilePath.toMap()["FilePath"].toString();
        const auto dateTime = recentFilePath.toMap()["DateTime"].toString();

        if (!QFileInfo(filePath).exists())
            continue;

        appendRow({ new QStandardItem(_recentFilePathsAction->getIcon(), filePath), new QStandardItem(dateTime) });

        auto recentFilePathAction = new TriggerAction(this, filePath);

        recentFilePathAction->setIcon(_recentFilePathsAction->getIcon());
        recentFilePathAction->setShortcut(QKeySequence(QString("Ctrl+Shift+%1").arg(QString::number(recentFilePaths.indexOf(recentFilePath) + 1))));

        auto mainWindow = Application::topLevelWidgets().first();

        mainWindow->addAction(recentFilePathAction);

        connect(recentFilePathAction, &TriggerAction::triggered, this, [this, filePath]() -> void {
            emit _recentFilePathsAction->triggered(filePath);
        });

        _actions << recentFilePathAction;
    }
}

void RecentFilePathsAction::Model::addRecentFilePath(const QString& filePath)
{
    auto recentFilePaths = Application::current()->getSetting(_recentFilePathsAction->getSettingsKey(), QVariantList()).toList();

    QVariantMap recentFilePath{
        { "FilePath", filePath },
        { "DateTime", QDateTime::currentDateTime() }
    };

    removeRecentFilePath(filePath);

    recentFilePaths.insert(0, recentFilePath);

    Application::current()->setSetting(_recentFilePathsAction->getSettingsKey(), recentFilePaths);

    loadFromSettings();
}

void RecentFilePathsAction::Model::removeRecentFilePath(const QString& filePath)
{
    auto recentFilePaths = Application::current()->getSetting(_recentFilePathsAction->getSettingsKey(), QVariantList()).toList();

    for (auto recentFilePath : recentFilePaths)
        if (recentFilePath.toMap()["FilePath"].toString() == filePath)
            recentFilePaths.removeOne(recentFilePath);

    Application::current()->setSetting(_recentFilePathsAction->getSettingsKey(), recentFilePaths);

    loadFromSettings();
}

QList<TriggerAction*> RecentFilePathsAction::Model::getActions()
{
    return _actions;
}

}