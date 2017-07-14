#ifndef PTE_LOG_INL
#define PTE_LOG_INL

#include <iostream>
#include <errors\invariants.h>
#include <QCoreApplication>

namespace nut{

	inline void PTELog::Clear(){
		PRECONDITION(_plain_text_edit_ptr != nullptr);
		_plain_text_edit_ptr->clear();
		QCoreApplication::processEvents();
	}
	inline void PTELog::Display(const std::string& d){
		PRECONDITION(_plain_text_edit_ptr != nullptr);
		_plain_text_edit_ptr->appendPlainText(QString::fromStdString(d));
		QCoreApplication::processEvents();
	}


	inline void AsyncPTELog::Clear(){
		_initialization_helper.CheckInitialized();
		emit asyncClear();
	}
	inline void AsyncPTELog::Display(const std::string& d){
		_initialization_helper.CheckInitialized();
		emit asyncDisplay(QString::fromStdString(d));
	}
	inline void AsyncPTELog::Initialize(QPlainTextEdit* ptr){
		PRECONDITION(ptr != nullptr);
		_plain_text_edit_ptr = ptr;
		QObject::connect(this, SIGNAL(asyncClear(void)),		_plain_text_edit_ptr,SLOT(clear(void)), Qt::QueuedConnection);
		QObject::connect(this, SIGNAL(asyncDisplay(QString)),	_plain_text_edit_ptr,SLOT(appendPlainText(QString)), Qt::QueuedConnection);
		_initialization_helper.Initialize();	
	}
	inline void AsyncPTELog::Unitialize(){
		disconnect();
		_plain_text_edit_ptr = nullptr;
		_initialization_helper.Uninitialize();
	}
	
}

#endif // COUT_LOG_INL
