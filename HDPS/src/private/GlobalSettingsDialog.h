#pragma once

#include <QDialog>

/**
 * Global settings dialog class
 * 
 * Dialog class for modifying global settings.
 *
 * @author Thomas Kroes
 */
class GlobalSettingsDialog : public QDialog
{
public:

    /**
     * Construct a global settings dialog with \p with parent widget
     * @param parent Pointer to parent widget
     */
    GlobalSettingsDialog(QWidget* parent = nullptr);

    /** Get preferred size */
    QSize sizeHint() const override {
        return QSize(640, 320);
    }

    /** Get minimum size hint*/
    QSize minimumSizeHint() const override {
        return sizeHint();
    }

private:

};
