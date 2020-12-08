#pragma once

#include <memory>

#include <QWidget>

class SelectionTool;

namespace Ui {
	class SelectionToolWidget;
}

/**
 * Selection tool widget class
 *
 * @author Thomas Kroes
 */
class SelectionToolWidget : public QWidget
{
public: // Construction/destruction

    SelectionToolWidget(QWidget* parent, SelectionTool& selectionTool);

private:
    SelectionTool&                              _selectionTool;     /** Selection tool */
	std::unique_ptr<Ui::SelectionToolWidget>	_ui;                /** User interface as produced by Qt designer */
};