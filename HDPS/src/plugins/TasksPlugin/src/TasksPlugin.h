// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <ViewPlugin.h>

#include <actions/TasksAction.h>

#include <ModalTask.h>

using namespace hdps;
using namespace hdps::plugin;
using namespace hdps::gui;

/**
 * Tasks plugin
 *
 * This plugin provides a user interface for viewing/configuring tasks.
 *
 * @author Thomas Kroes
 */
class TasksPlugin : public ViewPlugin
{
    Q_OBJECT
    
public:

#ifdef _DEBUG

    enum class TestMode {
        ModalRunningIndeterminate,
        ModalSubtasks,
    };

    static QMap<TestMode, QString> testModeNames;

#endif

    /**
     * Construct with \p factory
     * @param factory Pointer to plugin factory which created this plugin
     */
    TasksPlugin(const PluginFactory* factory);
    
    void init() override;

private:
    
    
#ifdef _DEBUG

    /** Adds GUI for testing various actions (visible in debug mode only) */
    void addTestSuite();
#endif
    

private:
    TasksAction     _tasksAction;   /** Tasks action for displaying and interacting with the tasks in the system */
};

class TasksPluginFactory : public ViewPluginFactory
{
    Q_INTERFACES(hdps::plugin::ViewPluginFactory hdps::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "NL.ManiVault.TasksPlugin" FILE "TasksPlugin.json")
    
public:

    /** Constructor */
    TasksPluginFactory();

    /**
     * Get plugin icon
     * @param color Icon color for flat (font) icons
     * @return Icon
     */
    QIcon getIcon(const QColor& color = Qt::black) const override;
    
	/**
     * Produces the plugin
     * @return Pointer to the produced plugin
     */
    ViewPlugin* produce() override;
};

#ifdef _DEBUG

class AbstractTaskTester : public QObject
{
public:
    AbstractTaskTester(QObject* parent, const QString& name, const Task::Status& status = Task::Status::Idle) :
        _modalTask(parent, name, status)
    {
        
    }

    virtual void run() = 0;

    ModalTask& getModalTask() {
        return _modalTask;
    }

    QTimer& getTimer() {
        return _timer;
    }

private:
    ModalTask   _modalTask;
    QTimer      _timer;
};

class ModalRunningIndeterminateTester : public AbstractTaskTester
{
public:
    ModalRunningIndeterminateTester(QObject* parent, const QString& name, const Task::Status& status = Task::Status::Idle) :
        AbstractTaskTester(parent, name, status),
        _tasks({ "Task A", "Task B", "Task C", "Task D", "Task E" })
    {
        connect(&getModalTask(), &Task::aborted, this, [this]() -> void {
            qDebug() << getModalTask().getName() << "has aborted";

            getTimer().stop();
            deleteLater();
        });

        connect(&getModalTask(), &Task::finished, this, [this]() -> void {
            qDebug() << getModalTask().getName() << "has finished";

            deleteLater();
        });

        run();
    }

    void run() override {
        getModalTask().setRunningIndeterminate();

        connect(&getTimer(), &QTimer::timeout, this, [&]() -> void {
            getModalTask().setProgressDescription(_tasks.first());

            _tasks.removeFirst();

            if (_tasks.isEmpty())
                getModalTask().setFinished();
        });

        getTimer().setInterval(1000);
        getTimer().start();
    }

private:
    QStringList _tasks;
};

#endif
