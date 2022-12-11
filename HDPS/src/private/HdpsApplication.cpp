#include "HdpsApplication.h"
#include "Archiver.h"
#include "DataHierarchyManager.h"

#include <CoreInterface.h>

#include <QDebug>
#include <QTemporaryDir>
#include <QFileDialog>
#include <QDir>
#include <QGridLayout>
#include <QCheckBox>
#include <QLabel>
#include <QSpinBox>
#include <QLineEdit>

#define _VERBOSE

using namespace hdps::util;

namespace hdps {

#if (__cplusplus < 201703L)   // definition needed for pre C++17 gcc and clang
    constexpr bool HdpsApplication::DEFAULT_ENABLE_COMPRESSION;
    constexpr std::uint32_t HdpsApplication::DEFAULT_COMPRESSION_LEVEL;
#endif

HdpsApplication::HdpsApplication(int& argc, char** argv) :
    Application(argc, argv),
    _projectManager(),
    _globalSettingsAction(this)
{
}

GlobalSettingsAction& HdpsApplication::getGlobalSettingsAction()
{
    return dynamic_cast<HdpsApplication*>(HdpsApplication::current())->_globalSettingsAction;
}

void HdpsApplication::loadProject(QString projectFilePath /*= ""*/)
{
    
}

void HdpsApplication::saveProject(QString projectFilePath /*= ""*/)
{
    
}

HdpsApplication::TaskProgressDialog::TaskProgressDialog(QWidget* parent, const QStringList& tasks, const QString& title, const QIcon& icon) :
    QProgressDialog(parent),
    _tasks(tasks)
{
    setWindowIcon(icon);
    setWindowTitle(title);
    setWindowModality(Qt::WindowModal);
    setMinimumDuration(0);
    setFixedWidth(600);
    setMinimum(0);
    setMaximum(_tasks.count());
    setValue(0);
    setAutoClose(false);
    setAutoReset(false);
}

void HdpsApplication::TaskProgressDialog::addTasks(const QStringList& tasks)
{
    _tasks << tasks;

    setMaximum(_tasks.count());
}

void HdpsApplication::TaskProgressDialog::setCurrentTask(const QString& taskName)
{
    // Update the label text
    setLabelText(taskName);

    // Ensure the progress dialog gets updated
    QCoreApplication::processEvents();
}

void HdpsApplication::TaskProgressDialog::setTaskFinished(const QString& taskName)
{
    // Remove the task from the list
    _tasks.removeOne(taskName);

    // Update the progress value
    setValue(maximum() - _tasks.count());

    // Ensure the progress dialog gets updated
    QCoreApplication::processEvents();
}

void HdpsApplication::TaskProgressDialog::setCurrentTaskText(const QString& taskText)
{
    // Update label text
    setLabelText(taskText);

    // Ensure the progress dialog gets updated
    QCoreApplication::processEvents();
}

}
