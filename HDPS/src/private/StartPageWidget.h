#pragma once

#include "ProjectBarWidget.h"
#include "LogoWidget.h"

#include <QWidget>
#include <QHBoxLayout>

/**
 * Start page widget class
 *
 * Widget class for the start page
 *
 * @author Thomas Kroes
 */
class StartPageWidget : public QWidget
{
public:

    /** Start page modes */
    enum class Mode {
        ProjectBar,     /** Show user interface for project open, recent projects etc. */
        LogoOnly        /** Show logo only */
    };

public:

    /**
     * Constructor
     * @param parent Pointer to parent widget
     * @param mode Start page modus
     */
    StartPageWidget(QWidget* parent = nullptr, const Mode& mode = Mode::ProjectBar);

    /**
     * Set mode
     * @param mode Mode
     */
    void setMode(const Mode& mode);

    /**
     * Get mode
     * @return Mode
     */
    Mode getMode() const;

protected:
    Mode                _mode;                  /** Start page mode */
    QHBoxLayout         _layout;                /** Main layout */
    ProjectBarWidget    _projectBarWidget;      /** Project bar for project management */
    LogoWidget          _logoWidget;            /** Logo widget */
};
