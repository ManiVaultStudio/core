#include "MainWindow.h"

#include <QSurfaceFormat>
#include <QStyleFactory>

#include "Application.h"

#include <actions/ToolbarAction.h>
#include <actions/IntegralAction.h>

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
        _widthAction(this, "Width"),
        _heightAction(this, "Height"),
        _depthAction(this, "Depth")
    {
        setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);

        _toolbarAction.addAction(&_widthAction, 1);
        _toolbarAction.addAction(&_heightAction, 1);
        _toolbarAction.addAction(&_depthAction, 1);

        auto layout = new QVBoxLayout();

        setLayout(layout);

        layout->addWidget(_toolbarAction.createWidget(this));
    }

    /** Get preferred size */
    QSize sizeHint() const override {
        return QSize(0, 0);
    }

    /** Get minimum size hint*/
    QSize minimumSizeHint() const override {
        return sizeHint();
    }

protected:
    ToolbarAction   _toolbarAction;
    IntegralAction  _widthAction;
    IntegralAction  _heightAction;
    IntegralAction  _depthAction;
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
