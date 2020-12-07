#pragma once

#include <memory>

#include <QWidget>

namespace Ui {
	class SelectionWidget;
}

/**
 * TODO
 *
 * @author Thomas Kroes
 */
class SelectionWidget : public QWidget
{
public: // Construction/destruction

    SelectionWidget(QWidget* parent);

private:
	std::unique_ptr<Ui::SelectionWidget>	_ui;        /** User interface as produced by Qt designer */
};