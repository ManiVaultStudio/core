#pragma once

#include <QDialog>

namespace hdps::gui {

/**
 * Settings manager dialog class
 * 
 * Dialog class for modifying settings.
 *
 * @author Thomas Kroes
 */
class SettingsManagerDialog : public QDialog
{
public:

    /**
     * Construct a dialog for the settings manager with \p with parent widget
     * @param parent Pointer to parent widget
     */
    SettingsManagerDialog(QWidget* parent = nullptr);

    /** Get preferred size */
    QSize sizeHint() const override {
        return QSize(640, 320);
    }

    /** Get minimum size hint*/
    QSize minimumSizeHint() const override {
        return sizeHint();
    }
};

}