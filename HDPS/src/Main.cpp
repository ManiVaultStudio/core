#include "MainWindow.h"

#include <QSurfaceFormat>
#include <QStyleFactory>

#include "Application.h"

#include <actions/ResponsiveToolbarAction.h>
#include <actions/IntegralAction.h>
#include <actions/DecimalAction.h>
#include <actions/StringAction.h>
#include <actions/ColorAction.h>
#include <actions/ColorMapAction.h>
#include <actions/DatasetPickerAction.h>
#include <actions/OptionAction.h>
#include <actions/ToggleAction.h>
#include <actions/WindowLevelAction.h>

class ToolbarDialog : public QDialog
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent widget
     * @param dataset Dataset root to apply grouping to
     */
    ToolbarDialog(QWidget* parent) :
        _toolbarAction(this),
        _integralAction(this, "Integral action"),
        _decimalAction(this, "Decimal action"),
        _stringAction(this, "String action"),
        _colorAction(this, "Color action"),
        _colorMapAction(this, "Color map action"),
        _datasetPickerAction(this, "Dataset picker action"),
        _optionAction(this, "Option action"),
        _toggleAction(this, "Toggle action"),
        _triggerAction(this, "Trigger action"),
        _windowLevelAction(this)
    {
        setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);

        //_widthAction.setDefaultWidgetFlags(IntegralAction::WidgetFlag::SpinBox);

        _toolbarAction.addAction(&_integralAction, 21);
        _toolbarAction.addAction(&_decimalAction, 7);
        _toolbarAction.addAction(&_stringAction, 11);
        _toolbarAction.addAction(&_colorAction, 3);
        _toolbarAction.addAction(&_colorMapAction, 1);
        _toolbarAction.addAction(&_datasetPickerAction, 8);
        _toolbarAction.addAction(&_optionAction, 5);
        _toolbarAction.addAction(&_toggleAction, 2);
        _toolbarAction.addAction(&_triggerAction, 4);
        _toolbarAction.addAction(&_windowLevelAction, 7);

        auto layout = new QVBoxLayout();

        layout->setMargin(0);

        setLayout(layout);

        layout->addWidget(_toolbarAction.createWidget(this));
        //layout->addWidget(new QPushButton("asdas"));
        //layout->addStretch(1);
    }

    /** Get preferred size */
    QSize sizeHint() const override {
        return QSize(850, 0);
    }

    /** Get minimum size hint*/
    QSize minimumSizeHint() const override {
        return sizeHint();
    }

protected:
    ToolbarAction           _toolbarAction;
    IntegralAction          _integralAction;
    DecimalAction           _decimalAction;
    StringAction            _stringAction;
    ColorAction             _colorAction;
    ColorMapAction          _colorMapAction;
    DatasetPickerAction     _datasetPickerAction;
    OptionAction            _optionAction;
    ToggleAction            _toggleAction;
    TriggerAction           _triggerAction;
    WindowLevelAction       _windowLevelAction;
};

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("BioVault");
    QCoreApplication::setOrganizationDomain("LUMC (LKEB) & TU Delft (CGV)");
    QCoreApplication::setApplicationName("HDPS");
    // Necessary to instantiate QWebEngine from a plugin
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts, true);

#ifdef __APPLE__
    // Ask for an OpenGL 3.3 Core Context as the default
    QSurfaceFormat defaultFormat;
    defaultFormat.setVersion(3, 3);
    defaultFormat.setProfile(QSurfaceFormat::CoreProfile);
    defaultFormat.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    QSurfaceFormat::setDefaultFormat(defaultFormat);
#endif

    hdps::Application hdpsApplication(argc, argv);

    // Retina display support for Mac OS and X11:
    // AA_UseHighDpiPixmaps attribute is off by default in Qt 5.1 but will most
    // likely be on by default in a future release of Qt.
    hdpsApplication.setAttribute(Qt::AA_UseHighDpiPixmaps);
    
    QFile styleSheetFile(":/styles/default.qss");

    styleSheetFile.open(QFile::ReadOnly);

    QString styleSheet = QLatin1String(styleSheetFile.readAll());

    qApp->setStyleSheet(styleSheet);

    QIcon appIcon;

    appIcon.addFile(":/Icons/AppIcon32");
    appIcon.addFile(":/Icons/AppIcon64");
    appIcon.addFile(":/Icons/AppIcon128");
    appIcon.addFile(":/Icons/AppIcon256");
    appIcon.addFile(":/Icons/AppIcon512");
    appIcon.addFile(":/Icons/AppIcon1024");

    hdpsApplication.setWindowIcon(appIcon);

    hdps::gui::MainWindow mainWindow;

    mainWindow.show();

    ToolbarDialog toolbarDialog(nullptr);

    toolbarDialog.show();

    return hdpsApplication.exec();
}
