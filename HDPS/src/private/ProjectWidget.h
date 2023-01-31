#pragma once

#include <QWidget>

/**
 * Project widget class
 *
 * Widget class for showing the workspace of the current project.
 *
 * @author Thomas Kroes
 */
class ProjectWidget : public QWidget
{
public:

    /**
     * Construct project widget from \p parent widget
     * @param parent Pointer to parent widget
     */
    ProjectWidget(QWidget* parent = nullptr);
};
