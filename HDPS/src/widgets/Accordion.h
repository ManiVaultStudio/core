#pragma once

#include "AccordionSection.h"

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
#include <QScrollArea>
#include <QMap>

namespace hdps
{

namespace gui
{

class Accordion : public QWidget
{
    Q_OBJECT

public:
    
    explicit Accordion(QWidget* parent = nullptr);

public:

    void addSection(QWidget* widget, const QString& title, const QIcon& icon = QIcon());
    void removeSection(QWidget* widget);

public: // Expand/collapse

    bool canExpandAll() const;
    bool canCollapseAll() const;
    void expandAll();
    void collapseAll();
    void goToSection(const QString& title);

public: // Toolbar

    void showToolbar(const bool& show);

private:

    std::uint32_t getNumExpandedSections() const;

    void updateExpansionUI();
    void updateToSectionUI();

public:
    QVBoxLayout                         _mainLayout;
    QWidget                             _toolbar;
    QHBoxLayout                         _toolbarLayout;
    QPushButton                         _expandAllPushButton;
    QPushButton                         _collapseAllPushButton;
    QComboBox                           _toSectionComboBox;
    QLineEdit                           _toSectionLineEdit;
    QScrollArea                         _sectionsScrollArea;
    QWidget                             _sectionsWidget;
    QVBoxLayout                         _sectionsLayout;
    QMap<QWidget*, AccordionSection*>   _sections;

    static const QString JUMP_TO_SECTION_TEXT;
};

}
}