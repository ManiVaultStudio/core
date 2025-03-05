// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "WidgetAction.h"

#include "util/StyledIcon.h"

#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QToolButton>

class QWidget;
class QCompleter;
class QEvent;

namespace mv::gui {

/**
 * String widget action class
 *
 * Stores a string creates widgets to interact with it
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT StringAction : public WidgetAction
{
    Q_OBJECT

public:

    /** Describes the widget configurations */
    enum WidgetFlag {
        Label       = 0x00001,      /** Widget includes a label */
        LineEdit    = 0x00002,      /** Widget includes a line edit */
        TextEdit    = 0x00004,      /** Widget includes a text edit */

        Default = LineEdit,
    };

    /** Actions shown at leading and/or trailing position */
    class InlineAction : public QWidgetAction
    {
    private:

        /** Create hover effect for tool button */
        class StyledToolButton : public QToolButton
        {
        public:

            /**
             * Construct with \p styledIcon and \p parent
             * @param inlineAction Reference to inline action
             * @param parent Pointer to parent widget
             */
            StyledToolButton(InlineAction& inlineAction, QWidget* parent = nullptr);

            /**
             * Respond to \p watched events
             * @param watched Object of which an event occurred
             * @param event The event that took place
             * @return Whether the event was handled
             */
            bool eventFilter(QObject* watched, QEvent* event) override;

        private:

            /** Add glow effect to tool button */
            void addGlowEffect();

            /** Remove glow effect from tool button */
            void removeGlowEffect();

            /** Synchronize with inline action */
            void synchronizeWithInlineAction();

        private:
            InlineAction& _inlineAction; /** Reference to inline action */
        };

    public:

        /**
         * Construct with pointer to parent widget
         * @param parent Pointer to parent widget
         */
        InlineAction(QWidget* parent = nullptr);

        /**
         * Create widget
         * @return Pointer to created widget
         */
        QWidget* createWidget(QWidget* parent) override;

        /**
         * Set icon by name
         * @param iconName Name of the icon
         */
        void setIconByName(const QString& iconName);

    protected:

        /** Only allow use of our own set icon function */
        using QWidgetAction::setIcon;

    private:
        util::StyledIcon _styledIcon;   /** Styled icon */
    };

public:

    /** Label widget class for string action */
    class CORE_EXPORT LabelWidget : public QLabel
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param stringAction Pointer to string action
         */
        LabelWidget(QWidget* parent, StringAction* stringAction);

        /**
         * Respond to \p target events
         * @param target Object of which an event occurred
         * @param event The event that took place
         */
        bool eventFilter(QObject* target, QEvent* event) override;

    private:

        /** Fetches text from the string action and computes the resultant text (taking into account text elidation) */
        void updateText();

    private:
        StringAction* _stringAction;    /** Pointer to string action */

        friend class StringAction;
    };

    /** Line edit widget class for string action */
    class CORE_EXPORT LineEditWidget : public QLineEdit
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param stringAction Pointer to string action
         */
        LineEditWidget(QWidget* parent, StringAction* stringAction);

    private:
        StringAction* _stringAction;    /** Pointer to string action */

        friend class StringAction;
    };

    /** Text edit widget class for multi-line strings */
    class CORE_EXPORT TextEditWidget : public QTextEdit
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param stringAction Pointer to string action
         */
        TextEditWidget(QWidget* parent, StringAction* stringAction);

        friend class StringAction;
    };

protected:

    /**
     * Get widget representation of the string action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override;

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     * @param string String
     * @param defaultString Default string
     */
    Q_INVOKABLE explicit StringAction(QObject* parent, const QString& title, const QString& string = "");

    /** Gets the current string */
    QString getString() const;

    /**
     * Sets the current string
     * @param string Current string
     */
    void setString(const QString& string);

    /** Get placeholder text */
    QString getPlaceholderString() const;

    /**
     * Set placeholder text
     * @param placeholderString Placeholder text
     */
    void setPlaceHolderString(const QString& placeholderString);

    /**
     * Get action at leading position
     * @return Reference to action at leading position
     */
    QAction& getLeadingAction();

    /**
     * Get action at trailing position
     * @return Reference to action at trailing position
     */
    QAction& getTrailingAction();

    /**
     * Get completer
     * @return Pointer to completer
     */
    QCompleter* getCompleter() const;

    /**
     * Set completer
     * @param completer Pointer to completer
     */
    void setCompleter(QCompleter* completer);

    /**
     * Get search mode (if on, there will be a leading action with a search icon and a trailing action to clear the string)
     * @return Search mode
     */
    bool getSearchMode() const;

    /**
     * Set search mode (if on, there will be a leading action with a search icon and a trailing action to clear the string)
     * @param searchMode Whether search mode is on/off
     */
    void setSearchMode(bool searchMode);

    /**
     * Get whether the string can be cleared by clicking the trailing action
     * @return Boolean indicating whether the string can be cleared
     */
    bool isClearable() const;

    /**
     * Set whether the string can be cleared by clicking the trailing action
     * @param clearable Boolean indicating whether the string can be cleared
     */
    void setClearable(bool clearable);

    /**
     * Get text elide mode
     * @return Text elide mode
     */
    Qt::TextElideMode getTextElideMode() const;

    /**
     * Set text elide mode
     * @param textElideMode Text elide mode
     */
    void setTextElideMode(const Qt::TextElideMode& textElideMode);

protected: // Linking

    /**
     * Connect this action to a public action
     * @param publicAction Pointer to public action to connect to
     * @param recursive Whether to also connect descendant child actions
     */
    void connectToPublicAction(WidgetAction* publicAction, bool recursive) override;

    /**
     * Disconnect this action from its public action
     * @param recursive Whether to also disconnect descendant child actions
     */
    void disconnectFromPublicAction(bool recursive) override;

public: // Serialization

    /**
     * Load widget action from variant
     * @param Variant representation of the widget action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save widget action to variant
     * @return Variant representation of the widget action
     */
    QVariantMap toVariantMap() const override;

signals:

    /**
     * Signals that the current string changed to \p string
     * @param string Current string that changed
     */
    void stringChanged(const QString& string);

    /**
     * Signals that the placeholder string changed to \p placeholderString
     * @param placeholderString Placeholder string that changed
     */
    void placeholderStringChanged(const QString& placeholderString);

    /**
     * Signals that the completer changed to \p completer
     * @param completer Pointer to completer
     */
    void completerChanged(QCompleter* completer);

    /**
     * Signals that the text elide mode changed to \p textElideMode
     * @param textElideMode Text elide mode
     */
    void textElideModeChanged(const Qt::TextElideMode& textElideMode);
    
protected:
    QString             _string;                /** Current string */
    QString             _placeholderString;     /** Place holder string */
    InlineAction        _leadingAction;         /** Inline action at the leading position */
    InlineAction        _trailingAction;        /** Inline action at the trailing position */
    QCompleter*         _completer;             /** Pointer to completer */
    bool                _searchMode;            /** Whether the string action is in search mode */
    bool                _clearable;             /** Whether the string can be cleared by clicking the trailing action */
    Qt::TextElideMode   _textElideMode;         /** Text elide mode */

    friend class AbstractActionsManager;
};

}

Q_DECLARE_METATYPE(mv::gui::StringAction)

inline const auto stringActionMetaTypeId = qRegisterMetaType<mv::gui::StringAction*>("mv::gui::StringAction");
