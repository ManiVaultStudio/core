#include "Accordion.h"
#include "../Application.h"

namespace hdps
{

namespace gui
{

Accordion::Accordion(QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _mainLayout(),
    _toolbar(),
    _toolbarLayout(),
    _expandAllPushButton(),
    _collapseAllPushButton(),
    _toSectionComboBox(),
    _toSectionLineEdit(),
    _sectionsScrollArea(),
    _sectionsWidget(),
    _sectionsLayout(),
    _sections()
{
    setMinimumWidth(300);
    setMaximumWidth(600);
    
    resize(QSize(600, 400));

    _mainLayout.setMargin(5);
    _mainLayout.setSpacing(2);
    _mainLayout.setAlignment(Qt::AlignTop);
    _mainLayout.addWidget(&_toolbar);
    //_mainLayout.addLayout(&_sectionsLayout);
    _mainLayout.addWidget(&_sectionsScrollArea);

    _toolbar.setLayout(&_toolbarLayout);

    _toolbarLayout.setMargin(0);
    _toolbarLayout.setSpacing(4);
    _toolbarLayout.addWidget(&_collapseAllPushButton);
    _toolbarLayout.addWidget(&_expandAllPushButton);
    _toolbarLayout.addWidget(&_toSectionComboBox);
    //_toolbarLayout.addWidget(&_toSectionLineEdit);
    //_toolbarLayout.addStretch(1);

    const auto iconSize = QSize(12, 12);

    _expandAllPushButton.setFixedSize(22, 22);
    _expandAllPushButton.setIcon(Application::getIconFont("FontAwesome").getIcon("angle-double-down", iconSize));

    QObject::connect(&_expandAllPushButton, &QPushButton::clicked, [this]() {
        expandAll();
    });

    _collapseAllPushButton.setFixedSize(22, 22);
    _collapseAllPushButton.setIcon(Application::getIconFont("FontAwesome").getIcon("angle-double-up", iconSize));

    QObject::connect(&_collapseAllPushButton, &QPushButton::clicked, [this]() {
        collapseAll();
    });

    _sectionsScrollArea.setLayout(&_sectionsLayout);
    _sectionsScrollArea.setStyleSheet("QScrollArea { border: none; }");
    _sectionsScrollArea.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    //_sectionsScrollArea.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _sectionsScrollArea.setWidgetResizable(true);
    _sectionsScrollArea.setWidget(&_sectionsWidget);

    _sectionsWidget.setLayout(&_sectionsLayout);

    _sectionsLayout.setAlignment(Qt::AlignTop);
    _sectionsLayout.setMargin(0);
    _sectionsLayout.setSpacing(0);

    setLayout(&_mainLayout);

    updateExpansionButtons();
}

void Accordion::addSection(QWidget* widget, const QString& title, const QIcon& icon /*= QIcon()*/)
{
    Q_ASSERT(widget != nullptr);
    Q_ASSERT(!title.isEmpty());

    auto accordionSection = new AccordionSection(title);
    
    accordionSection->setWidget(widget);
    accordionSection->setIcon(icon);

    _sectionsLayout.addWidget(accordionSection);
    
    _sections.insert(widget, accordionSection);

    updateExpansionButtons();

    QObject::connect(accordionSection, &AccordionSection::expandedChanged, [this]() {
        updateExpansionButtons();
    });
}

void Accordion::removeSection(QWidget* widget)
{
    Q_ASSERT(widget != nullptr);

    _sectionsLayout.removeWidget(widget);

    delete _sections[widget];

    _sections.remove(widget);
}

bool Accordion::canExpandAll() const
{
    if (_sections.isEmpty())
        return false;

    return getNumExpandedSections() < _sections.count();
}

bool Accordion::canCollapseAll() const
{
    if (_sections.isEmpty())
        return false;

    return getNumExpandedSections() > 0;
}

void Accordion::expandAll()
{
    for (const auto section : _sections.values())
        section->expand();
}

void Accordion::collapseAll()
{
    for (const auto section : _sections.values())
        section->collapse();
}

void Accordion::showToolbar(const bool& show)
{
    _toolbar.setVisible(show);
}

std::uint32_t Accordion::getNumExpandedSections() const
{
    auto numExpandedSections = 0;

    for (const auto section : _sections.values()) {
        if (section->isExpanded())
            numExpandedSections++;
    }

    return numExpandedSections;
}

void Accordion::updateExpansionButtons()
{
    _expandAllPushButton.setEnabled(canExpandAll());
    _collapseAllPushButton.setEnabled(canCollapseAll());
}

}

}