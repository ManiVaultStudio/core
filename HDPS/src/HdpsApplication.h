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

            /**
             * Constructor
             * @param parent Pointer to parent widget
             * @param tasks List of task names
             * @param title Main task title
             * @param icon Dialog icon
             */
            explicit TaskProgressDialog(QWidget* parent, const QStringList& tasks, const QString& title, const QIcon& icon);

            /**
             * Add a list of tasks
             * @param tasks List of tasks to add
             */
            void addTasks(const QStringList& tasks);

            /**
             * Set the name of the current task
             * @param taskName Name of the current task
             */
            void setCurrentTask(const QString& taskName);

            /**
             * Flag task as finished
             * @param taskName Name of the task that finished
             */
            void setTaskFinished(const QString& taskName);

            /**
             * Set the current tasks text (visible in the label on the progress dialog)
             * @param taskText Task text
             */
            void setCurrentTaskText(const QString& taskText);

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

public: // Project IO

    /**
     * Load project from disk
     * @param projectFilePath File path of the project (if empty, the user will  select a file location by hand)
     */
    void loadProject(QString projectFilePath = "") override final;

    /**
     * Save project to disk
     * @param projectFilePath File path of the project (if empty, the user will  select a file location by hand)
     */
    void saveProject(QString projectFilePath = "") override final;

protected:
    bool            _enableCompression;     /** Whether projects will be saved with compression or not */
    std::uint32_t   _compressionLevel;      /** Level of compression for project files */
};

}