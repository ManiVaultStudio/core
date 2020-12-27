#include "Accordion.h"
#include "../Application.h"

#include <QScrollBar>
#include <QResizeEvent>

namespace hdps
{

namespace gui
{

const QString Accordion::JUMP_TO_SECTION_TEXT = "Go to...";

Accordion::Accordion(QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _mainLayout(),
    _toolbar(),
    _toolbarLayout(),
    _expandAllPushButton(),
    _collapseAllPushButton(),
    _toSectionComboBox(),
    _sectionsScrollArea(),
    _sectionsWidget(),
    _sectionsLayout(),
    _sections(),
    _toSection()
{
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

    _mainLayout.setMargin(5);
    _mainLayout.setSpacing(4);
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

    QObject::connect(&_toSectionComboBox, &QComboBox::currentTextChanged, [this](const QString& currentText) {
        goToSection(currentText);
    });

    _sectionsScrollArea.setLayout(&_sectionsLayout);
    _sectionsScrollArea.setStyleSheet("QScrollArea { border: none; }");
    _sectionsScrollArea.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    _sectionsScrollArea.setWidgetResizable(true);
    _sectionsScrollArea.setWidget(&_sectionsWidget);

    _sectionsWidget.setLayout(&_sectionsLayout);

    _sectionsLayout.setAlignment(Qt::AlignTop);
    _sectionsLayout.setMargin(0);
    _sectionsLayout.setSpacing(2);

    setLayout(&_mainLayout);

    updateExpansionUI();
    updateToSectionUI();

    QObject::connect(_sectionsScrollArea.verticalScrollBar(), &QScrollBar::rangeChanged, [this](int min, int max) {
        if (_toSection.isEmpty())
            return;

        goToSection(_toSection);

        _toSection = "";
    });

    _sectionsScrollArea.verticalScrollBar()->setStyleSheet(QString::fromUtf8("QScrollBar:vertical {"
        "    background: rgb(190, 190, 190);"
        "    width: 9px;"
        "    margin: 1px 0px 0px 2px;"
        "}"
        "QScrollBar::handle:vertical {"
        "    background: rgb(150, 150, 150);"
        "    min-height: 0px;"
        "}"
        "QScrollBar::add-line:vertical {"
        "    height: 0px;"
        "}"
        "QScrollBar::sub-line:vertical {"
        "    height: 0px;"
        "}"
    ));

    _sectionsScrollArea.verticalScrollBar()->installEventFilter(this);
}

void Accordion::addSection(QWidget* widget)
{
    Q_ASSERT(widget != nullptr);

    auto accordionSection = new AccordionSection(this);
    
    accordionSection->setWidget(widget);

    _sectionsLayout.addWidget(accordionSection);
    
    _sections << accordionSection;

    updateExpansionUI();
    updateToSectionUI();

    QObject::connect(accordionSection, &AccordionSection::expandedChanged, [this]() {
        updateExpansionUI();
    });

    _toSection = widget->objectName();
}

void Accordion::removeSection(QWidget* widget)
{
    Q_ASSERT(widget != nullptr);

    _sectionsLayout.removeWidget(widget);

    for (const auto section : _sections) {
        if (section->getWidget() == widget) {
            _sections.removeOne(section);
            delete section;
        }
    }
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
    for (const auto section : _sections)
        section->expand();
}

void Accordion::collapseAll()
{
    for (const auto section : _sections)
        section->collapse();
}

void Accordion::goToSection(const QString& name)
{
    Q_ASSERT(!name.isEmpty());

    for (const auto section : _sections) {
        if (section->getName() == name) {
            const auto pointRelativeToScrollArea = section->mapTo(&_sectionsScrollArea, QPoint(0, 0));

            _sectionsScrollArea.verticalScrollBar()->setValue(pointRelativeToScrollArea.y() + _sectionsScrollArea.verticalScrollBar()->value());
        }
    }

    QSignalBlocker toSectionComboBoxBlocker(&_toSectionComboBox);

    _toSectionComboBox.setCurrentText(JUMP_TO_SECTION_TEXT);
}

void Accordion::showToolbar(const bool& show)
{
    _toolbar.setVisible(show);
}

bool Accordion::eventFilter(QObject* target, QEvent* event)
{
    switch (event->type())
    {
        case QEvent::Show:
        case QEvent::Hide:
            updateToSectionUI();
            break;

        default:
            break;
    }

    return QObject::eventFilter(target, event);
}

std::uint32_t Accordion::getNumExpandedSections() const
{
    auto numExpandedSections = 0;

    for (const auto section : _sections) {
        if (section->isExpanded())
            numExpandedSections++;
    }

    return numExpandedSections;
}

void Accordion::updateExpansionUI()
{
    _expandAllPushButton.setEnabled(canExpandAll());
    _collapseAllPushButton.setEnabled(canCollapseAll());
}

void Accordion::updateToSectionUI()
{
    QSignalBlocker toSectionComboBoxBlocker(&_toSectionComboBox);

    _toSectionComboBox.clear();
    _toSectionComboBox.addItem(JUMP_TO_SECTION_TEXT);

    for (const auto section : _sections)
        _toSectionComboBox.addItem(section->getName());

    _toSectionComboBox.setEnabled(_sectionsScrollArea.verticalScrollBar()->isVisible());
}

}
}