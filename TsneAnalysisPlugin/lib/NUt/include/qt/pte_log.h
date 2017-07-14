#ifndef PTE_LOG_H
#define PTE_LOG_H

#include <log/abstract_log.h>
#include <helpers/initialization_helper.h>
#include <QPlainTextEdit>

namespace nut{

	//! shows log on a plain text edit
	class PTELog: public AbstractLog{
	public:
		PTELog(QPlainTextEdit* ptr = nullptr):_plain_text_edit_ptr(ptr){}
	    virtual ~PTELog(){}
		//! clear the plain text edit
	    virtual void Clear();
		//! display some text on the logger
	    virtual void Display(const std::string& d);

		//! set the plain text edit pointer
		void set_plain_text_edit_ptr(QPlainTextEdit* ptr){_plain_text_edit_ptr = ptr;}

	private:
		QPlainTextEdit* _plain_text_edit_ptr;
	};

	//! shows log on a plain text edit
	class AsyncPTELog: public QObject, public AbstractLog{
		Q_OBJECT
	public:
		AsyncPTELog():_plain_text_edit_ptr(nullptr),_receiver(nullptr){}
	    virtual ~AsyncPTELog(){}
		//! clear the plain text edit
	    virtual void Clear();
		//! display some text on the logger
	    virtual void Display(const std::string& d);
		//! initialize the async pte logger
		void Initialize(QPlainTextEdit* ptr);
		//! uninitialize the async pte logger
		void Unitialize();

	Q_SIGNALS:
		void asyncClear();
		void asyncDisplay(QString str);


	private:
		QPlainTextEdit* 		_plain_text_edit_ptr;
		QObject* 				_receiver;
		InitializationHelper	_initialization_helper;
	};
	
}

//Implementation
#include "pte_log-inl.h"

#endif // COUT_LOG_H
