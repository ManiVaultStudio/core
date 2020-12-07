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
class SelectionLayerWidget : public QWidget
{
public: // Construction/destruction

	SelectionLayerWidget(QWidget* parent);

private:
	std::unique_ptr<Ui::SelectionWidget>	_ui;        /** User interface as produced by Qt designer */
};