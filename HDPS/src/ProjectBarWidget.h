#pragma once

#include <QWidget>
#include <QGridLayout>
#include <QLabel>

/**
 * Project bar widget class
 *
 * Widget class for the project bar
 *
 * @author Thomas Kroes
 */
class ProjectBarWidget : public QWidget
{
public:

    /**
     * Header widget class
     *
     * Widget class for header with logo
     *
     * @author Thomas Kroes
     */
    class HeaderWidget : public QWidget
    {
    public:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         */
        HeaderWidget(QWidget* parent = nullptr);

    protected:
        QVBoxLayout     _layout;        /** Main layout */
        QLabel          _headerLabel;   /** Header label */
    };

    /**
     * File action widget class
     *
     * Widget class for file action
     *
     * @author Thomas Kroes
     */
    class FileActionWidget : public QWidget
    {
    public:

        /** Called when the file action widget is pressed */
        using ActionCallBack = std::function<void()>;

    public:

        /**
         * Constructor
         * @param icon Icon
         * @param title Title of the file action
         * @param description Description of the file action
         * @param parent Pointer to parent widget
         */
        FileActionWidget(const QIcon& icon, const QString& title, const QString& description, const ActionCallBack& actionCallback, QWidget* parent = nullptr);

        /**
         * Invoked when the mouse starts to hover over the widget
         * @param event Pointer to event
         */
        void enterEvent(QEvent* event);

        /**
         * Invoked when the mouse leaves the widget
         * @param event Pointer to event
         */
        void leaveEvent(QEvent* event);

        /**
         * Invoked when the mouse presses inside the widget
         * @param mouseEvent Pointer to mouse event
         */
        void mousePressEvent(QMouseEvent* mouseEvent);

    protected:
        QHBoxLayout         _layout;                /** Main layout */
        QVBoxLayout         _fileLayout;            /** File layout */
        QLabel              _iconLabel;             /** Icon label */
        QLabel              _titleLabel;            /** Title label */
        QLabel              _descriptionLabel;      /** Description label */
        ActionCallBack      _actionCallback;        /** Called when the file action widget is pressed */
    };

    /**
     * Projects widget class
     *
     * Widget class for opening projects
     *
     * @author Thomas Kroes
     */
    class ProjectsWidget : public QWidget
    {
    public:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         */
        ProjectsWidget(QWidget* parent = nullptr);

    protected:

        /** Create left column actions */
        void createLeftColumn();

        /** Create right column actions */
        void createRightColumn();


        /**
         * Create header label
         * @param title Title of the header
         * @param tooltip Tooltip of the header
         * @return Pointer to newly created header label
         */
        QLabel* createHeaderLabel(const QString& title, const QString& tooltip);

    protected:
        QHBoxLayout     _layout;                /** Main layout */
        QVBoxLayout     _leftColumnLayout;      /** Layout for left column */
        QVBoxLayout     _rightColumnLayout;     /** Layout for right column */
    };

    /**
     * Recent projects widget class
     *
     * Widget class for recent projects
     *
     * @author Thomas Kroes
     */
    class RecentProjectsWidget : public QWidget
    {
    public:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         */
        RecentProjectsWidget(QWidget* parent = nullptr);

    protected:
        QVBoxLayout     _layout;    /** Main layout */
    };

    /**
     * Import data widget class
     *
     * Widget class for importing data
     *
     * @author Thomas Kroes
     */
    class ImportDataWidget : public QWidget
    {
    public:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         */
        ImportDataWidget(QWidget* parent = nullptr);

    protected:
        QVBoxLayout     _layout;    /** Main layout */
    };

public:

    /**
     * Constructor
     * @param parent Pointer to parent widget
     */
    ProjectBarWidget(QWidget* parent = nullptr);

protected:

    /**
     * Set the widget background color role
     * @param widget Pointer to widget to apply the background color to
     * @param colorRole Background color role
     */
    static void setWidgetBackgroundColorRole(QWidget* widget, const QPalette::ColorRole& colorRole);

protected:
    QHBoxLayout     _layout;        /** Main layout */
    QVBoxLayout     _barLayout;     /** Bar layout */
};
