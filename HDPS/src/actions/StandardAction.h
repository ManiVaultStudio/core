#pragma once

#include <QWidgetAction>
#include <QPushButton>
#include <QCheckBox>

class QWidget;

namespace hdps {

namespace gui {

/**
 * Standard widget action class
 *
 * Provides interaction with a standard QT action
 *
 * @author Thomas Kroes
 */
class StandardAction : public QWidgetAction
{
    Q_OBJECT

public:
    class PushButton : public QPushButton {
    public:
    public:
        enum Configuration {
            Text    = 0x0001,
            Icon    = 0x0002,

            TextIcon = Text | Icon
        };

    public:
        PushButton(QWidget* parent, QAction* action, const Configuration& configuration = Configuration::Text);
    };

    class CheckBox : public QCheckBox {
    public:
        CheckBox(QWidget* parent, QAction* action);
    };

public:
    StandardAction(QObject* parent, const QString& title = "");
};

}
}