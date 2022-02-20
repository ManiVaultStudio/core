#pragma once

#include <Application.h>

#include <QProgressDialog>

namespace hdps {

/**
 * HDPS application class
 * 
 * @author Thomas Kroes
 */
class HdpsApplication : public Application
{
public:

    /**
     * Task progress dialog class
     *
     * Class for reporting task progress
     *
     * @author Thomas Kroes
     */
    class TaskProgressDialog : public QProgressDialog
    {
        public:
            explicit TaskProgressDialog(QWidget* parent, const QStringList& tasks, const QString& title, const QIcon& icon);

            void addTasks(const QStringList& tasks);
            void setCurrentTask(const QString& taskName);
            void setTaskFinished(const QString& taskName);
            void setCurrentTaskName(const QString& taskName);

        protected:
            QStringList     _tasks;     /** String list of tasks that need to be performed */
    };
    
public: // Construction

    /**
     * Constructor
     * @param argc Number of command line arguments
     * @param argv Command line arguments
     */
    HdpsApplication(int& argc, char** argv);

public: // Serialization

    /** Load application from disk */
    void loadAnalysis() override final;

    /** Save application to disk */
    void saveAnalysis() override final;
};

}