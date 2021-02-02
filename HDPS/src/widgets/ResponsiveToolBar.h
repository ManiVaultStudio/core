#pragma once

#include "PopupPushButton.h"
#include "ResponsiveSectionWidget.h"

#include <QWidget>
#include <QEvent>
#include <QDebug>
#include <QHBoxLayout>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>

class QFrame;

namespace hdps {

namespace gui {

class ResponsiveToolBar : public QWidget
{
    Q_OBJECT

public:
    class Spacer : public QWidget {
    public:
        enum class Type {
            Divider,
            Spacer
        };

    public:
        Spacer(const Type& type = Type::Divider);

        static Type getType(const ResponsiveSectionWidget::State& stateBefore, const ResponsiveSectionWidget::State& stateAfter);
        static Type getType(const ResponsiveSectionWidget* sectionBefore, const ResponsiveSectionWidget* sectionAfter);
        void setType(const Type& type);
        static std::int32_t getWidth(const Type& type);
        

    protected:
        Type            _type;
        QHBoxLayout*    _layout;
        QFrame*         _verticalLine;
    };

    class SectionsWidget : public QWidget {
    public:
        SectionsWidget(QWidget* parent = nullptr);

        void addSectionWidget(QWidget* widget);

    protected:
        QHBoxLayout*    _layout;
    };

public:
    ResponsiveToolBar(QWidget* parent = nullptr);

    bool eventFilter(QObject* object, QEvent* event);

    template<typename StatefulWidgetType>
    void addWidget(ResponsiveSectionWidget::InitializeWidgetFn initializeWidgetFn, const QString& name, const QIcon& icon = QIcon(), const std::int32_t& priority = 0)
    {
        const auto getWidgetState = [](const ResponsiveSectionWidget::State& state) -> QWidget* {
            return new StatefulWidgetType(state);
        };

        auto sectionWidget = new ResponsiveSectionWidget(getWidgetState, name, icon, priority);

        sectionWidget->setInitializeWidgetFunction(initializeWidgetFn);
        sectionWidget->installEventFilter(this);

        _sections << sectionWidget;

        if (_sections.count() >= 2) {
            auto spacer = new Spacer();

            _spacers << spacer;

            _sectionsWidget->layout()->addWidget(spacer);
        }

        _sectionsWidget->layout()->addWidget(sectionWidget);

        computeLayout();
    }

private:
    void computeLayout();

private:
    QHBoxLayout*                        _layout;
    QWidget*                            _sectionsWidget;
    QVector<ResponsiveSectionWidget*>   _sections;
    QVector<Spacer*>                    _spacers;
    bool                                _dirty;

public:
    static const std::int32_t LAYOUT_MARGIN;
    static const std::int32_t LAYOUT_SPACING;
};

}
}