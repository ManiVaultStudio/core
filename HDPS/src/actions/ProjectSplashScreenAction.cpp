#include "ProjectSplashScreenAction.h"
#include "Project.h"

#include <QToolButton>
#include <QTimer>

namespace hdps::gui {

ProjectSplashScreenAction::ProjectSplashScreenAction(QObject* parent, const Project& project) :
    HorizontalGroupAction(parent, "Splash Screen"),
    _project(project),
    _enabledAction(this, "Show splash screen at startup"),
    _durationAction(this, "Duration", 1000, 10000, 3000),
    _backgroundColorAction(this, "Background Color", QColor(200, 200, 200)),
    _editAction(this, "Edit"),
    _showSplashScreenAction(this, "Show"),
    _splashScreenDialog(*this)
{
    setSerializationName("Splash Screen");
    setConfigurationFlag(WidgetAction::ConfigurationFlag::NoLabelInGroup);

    _enabledAction.setStretch(1);
    _enabledAction.setToolTip("Show splash screen at startup");
    _enabledAction.setSerializationName("Enabled");

    _durationAction.setToolTip("Duration of the splash screen in milliseconds");
    _durationAction.setSuffix("ms");
    _durationAction.setSerializationName("Duration");

    auto& fontAwesome = Application::getIconFont("FontAwesome");

    _editAction.setDefaultWidgetFlags(TriggerAction::Icon);
    _editAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::AlwaysCollapsed);
    _editAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::NoLabelInGroup);
    _editAction.setIcon(fontAwesome.getIcon("cog"));
    _editAction.setToolTip("Edit the splash screen settings");
    _editAction.setPopupSizeHint(QSize(300, 0));

    _editAction << _durationAction;
    _editAction << _backgroundColorAction;

    _showSplashScreenAction.setDefaultWidgetFlags(TriggerAction::Icon);
    _showSplashScreenAction.setIcon(fontAwesome.getIcon("eye"));
    _showSplashScreenAction.setToolTip("Preview the splash screen");

    addAction(_enabledAction);
    addAction(_editAction);
    addAction(_showSplashScreenAction);

    connect(&_showSplashScreenAction, &TriggerAction::triggered, this, [this]() -> void {
        _splashScreenDialog.open();
    });
}

QString ProjectSplashScreenAction::getTypeString() const
{
    return "ProjectSplashScreen";
}

const Project& ProjectSplashScreenAction::getProject() const
{
    return _project;
}

void ProjectSplashScreenAction::fromVariantMap(const QVariantMap& variantMap)
{
    Serializable::fromVariantMap(variantMap);

    _enabledAction.fromParentVariantMap(variantMap);
    _durationAction.fromParentVariantMap(variantMap);
    _backgroundColorAction.fromParentVariantMap(variantMap);
}

QVariantMap ProjectSplashScreenAction::toVariantMap() const
{
    auto variantMap = Serializable::toVariantMap();

    _enabledAction.insertIntoVariantMap(variantMap);
    _durationAction.insertIntoVariantMap(variantMap);
    _backgroundColorAction.insertIntoVariantMap(variantMap);

    return variantMap;
}

ProjectSplashScreenAction::Dialog::Dialog(const ProjectSplashScreenAction& projectSplashScreenAction, QWidget* parent /*= nullptr*/) :
    QDialog(parent),
    _projectSplashScreenAction(projectSplashScreenAction),
    _opacityAnimation(this, "windowOpacity", this),
    _positionAnimation(this, "pos", this)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setWindowFlags(Qt::Window | Qt::Popup);// | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint); //  | Qt::Popup
    setAttribute(Qt::WA_StyledBackground);

    const auto updateStyleSheet = [this]() -> void {
        setStyleSheet(QString("QDialog { \
            background-color: %1; \
            border: 1px solid darkgray; \
         }").arg(_projectSplashScreenAction.getBackgroundColorAction().getColor().name(QColor::HexArgb)));
    };
    
    updateStyleSheet();

    connect(&_projectSplashScreenAction.getBackgroundColorAction(), &ColorAction::colorChanged, this, updateStyleSheet);

    _opacityAnimation.setDuration(animationDuration);
    _positionAnimation.setDuration(animationDuration);

    _opacityAnimation.setEasingCurve(QEasingCurve::OutQuad);
    _positionAnimation.setEasingCurve(QEasingCurve::OutQuad);

    connect(&_opacityAnimation, &QPropertyAnimation::finished, [this] {
        if (_opacityAnimation.endValue().toInt() == 0)
            QDialog::accept();
    });

    auto& project = const_cast<Project&>(_projectSplashScreenAction.getProject());

    auto mainLayout = new QVBoxLayout();

    {
        auto toolbarLayout      = new QHBoxLayout();
        auto closeToolButton    = new QToolButton();

        closeToolButton->setIcon(Application::getIconFont("FontAwesome").getIcon("times"));
        closeToolButton->setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonIconOnly);
        closeToolButton->setAutoRaise(true);

        connect(closeToolButton, &QToolButton::clicked, this, &Dialog::accept);

        toolbarLayout->addStretch(1);
        toolbarLayout->addWidget(closeToolButton);

        mainLayout->addLayout(toolbarLayout);
    }

    {
        auto primaryLayout = new QHBoxLayout();

        {
            auto imageLayout = new QVBoxLayout();

            auto imageLabel = new QLabel();

            imageLabel->setFixedSize(200, 200);

            imageLayout->addWidget(imageLabel);

            primaryLayout->addLayout(imageLayout);
        }

        {
            auto textLayout = new QVBoxLayout();

            textLayout->addWidget(project.getTitleAction().createWidget(this, StringAction::Label));
            textLayout->addWidget(project.getProjectVersionAction().getVersionStringAction().createWidget(this, StringAction::Label));
            textLayout->addWidget(project.getDescriptionAction().createWidget(this, StringAction::Label));
            textLayout->addStretch(1);

            primaryLayout->addLayout(textLayout);
        }

        mainLayout->addLayout(primaryLayout);
    }

    setLayout(mainLayout);
}

void ProjectSplashScreenAction::Dialog::open()
{
    QTimer::singleShot(_projectSplashScreenAction.getDurationAction().getValue() - animationDuration, this, [this]() -> void {
        animate(true);
    });

    animate(false);

    QDialog::open();
}

void ProjectSplashScreenAction::Dialog::animate(bool reverse)
{
    if (_opacityAnimation.state() == QPropertyAnimation::Running)
        _opacityAnimation.stop();
    
    _opacityAnimation.setStartValue(reverse ? 1.0 : 0.0);
    _opacityAnimation.setEndValue(reverse ? 0.0 : 1.0);
    _opacityAnimation.start();
}

QSize ProjectSplashScreenAction::Dialog::sizeHint() const
{
    return QSize(640, 480);
}

void ProjectSplashScreenAction::Dialog::accept()
{
    animate(true);
}

}