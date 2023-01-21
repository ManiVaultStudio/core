#pragma once

#include <QWidget>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QGridLayout>

/**
 * Start page action delegate editor widget class
 *
 * Delegate editor widget class for custom start page action display in a view.
 *
 * @author Thomas Kroes
 */
class StartPageActionDelegateEditorWidget : public QWidget
{
    Q_OBJECT
public:
    
    /**
     * Construct delegate editor widget with \p parent widget
     * @param parent Pointer to parent widget
     */
    StartPageActionDelegateEditorWidget(QWidget* parent = nullptr);

    /**
     * Set editor data from model \p index
     * @param index Model index to fetch data from
     */
    void setEditorData(const QModelIndex& index);

private:
    QHBoxLayout     _mainLayout;            /** Main editor layout */
    QVBoxLayout     _iconLayout;            /** Left icon layout */
    QLabel          _iconLabel;             /** Left icon label */
    QGridLayout     _textLayout;            /** Right text layout */
    QLabel          _titleLabel;            /** Title label */
    QLabel          _descriptionLabel;      /** Description label */
    QLabel          _commentsLabel;         /** Comments label */
    QHBoxLayout     _infoLayout;            /** Layout for preview, tags and meta info popups */
};
