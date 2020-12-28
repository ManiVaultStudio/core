#pragma once

#include <QWidget>
#include <QIcon>

namespace hdps
{
namespace gui
{

/**
 * Settings widget class
 *
 * @author Thomas Kroes
 */
class SettingsWidget : public QWidget
{
    Q_OBJECT

public: // Dynamic property wrapper functions

    /**
     * Set title
     * @param title Title
     */
    void setTitle(const QString& title);

    /** Returns the title */
    QString getTitle() const;

    /**
     * Set subtitle
     * @param subtitle Subtitle
     */
    void setSubtitle(const QString& subtitle);

    /** Returns the subtitle */
    QString getSubtitle() const;

    /**
     * Set icon
     * @param icon Icon
     */
    void setIcon(const QIcon& icon);

    /** Returns the icon */
    QIcon getIcon() const;
};

}
}