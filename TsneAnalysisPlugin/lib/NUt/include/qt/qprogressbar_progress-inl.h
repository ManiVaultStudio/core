#ifndef QPROGRESSBAR_PROGRESS_INL
#define QPROGRESSBAR_PROGRESS_INL

namespace nut{

	inline QProgressBarProgress::QProgressBarProgress(QProgressBar* progress_bar):
		_progress_bar(progress_bar)
	{}
	inline void QProgressBarProgress::Reset(){
		PRECONDITION(_progress_bar != nullptr);
		_progress_bar->reset();
	}
	inline void QProgressBarProgress::SetMaximum(int maximum){
		PRECONDITION(_progress_bar != nullptr);
		_progress_bar->setMaximum(maximum);
	}
	inline int QProgressBarProgress::GetMaximum() const{
		PRECONDITION(_progress_bar != nullptr);
		return _progress_bar->maximum();
	}
	inline void QProgressBarProgress::SetMinimum(int minimum){
		PRECONDITION(_progress_bar != nullptr);
		_progress_bar->setMinimum(minimum);
	}
	inline int QProgressBarProgress::GetMinimum() const{
		PRECONDITION(_progress_bar != nullptr);
		return _progress_bar->minimum();
	}
	inline void QProgressBarProgress::SetCurrentPosition(int pos){
		PRECONDITION(_progress_bar != nullptr);
		_progress_bar->setValue(pos);
	}
	inline int QProgressBarProgress::GetCurrentPosition() const{
		PRECONDITION(_progress_bar != nullptr);
		return _progress_bar->value();
	}
	inline void QProgressBarProgress::MakeAStep(){
		PRECONDITION(_progress_bar != nullptr);
		_progress_bar->setValue(_progress_bar->value()+1);
	}
	inline void QProgressBarProgress::MakeNSteps(unsigned int steps){
		PRECONDITION(_progress_bar != nullptr);
		_progress_bar->setValue(_progress_bar->value()+steps);
	}
	inline bool QProgressBarProgress::IsCanceled()const{
		PRECONDITION(_progress_bar != nullptr);
		return false;
	}


	inline AsyncQProgressBarProgress::AsyncQProgressBarProgress():
		_progress_bar(nullptr)
	{}
	inline void AsyncQProgressBarProgress::Reset(){
		_initialization_helper.CheckInitialized();
		emit asyncReset();
	}
	inline void AsyncQProgressBarProgress::SetMaximum(int maximum){
		_initialization_helper.CheckInitialized();
		emit asyncSetMaximum(maximum);
	}
	inline int AsyncQProgressBarProgress::GetMaximum() const{
		_initialization_helper.CheckInitialized();
		return _progress_bar->maximum();
	}
	inline void AsyncQProgressBarProgress::SetMinimum(int minimum){
		_initialization_helper.CheckInitialized();
		emit asyncSetMinimum(minimum);
	}
	inline int AsyncQProgressBarProgress::GetMinimum() const{
		_initialization_helper.CheckInitialized();
		return _progress_bar->minimum();
	}
	inline void AsyncQProgressBarProgress::SetCurrentPosition(int pos){
		_initialization_helper.CheckInitialized();
		emit asyncSetValue(pos);
	}
	inline int AsyncQProgressBarProgress::GetCurrentPosition() const{
		_initialization_helper.CheckInitialized();
		return _progress_bar->value();
	}
	inline void AsyncQProgressBarProgress::MakeAStep(){
		_initialization_helper.CheckInitialized();
		emit asyncSetValue(_progress_bar->value()+1);
	}
	inline void AsyncQProgressBarProgress::MakeNSteps(unsigned int steps){
		_initialization_helper.CheckInitialized();
		emit asyncSetValue(_progress_bar->value()+steps);
	}
	inline bool AsyncQProgressBarProgress::IsCanceled()const{
		_initialization_helper.CheckInitialized();
		return false;
	}

	inline void AsyncQProgressBarProgress::Initialize(QProgressBar* ptr){
		PRECONDITION(ptr != nullptr);
		_progress_bar = ptr;
		QObject::connect(this, SIGNAL(asyncReset(void)),		_progress_bar,SLOT(reset(void)), Qt::QueuedConnection);
		QObject::connect(this, SIGNAL(asyncSetMaximum(int)),	_progress_bar,SLOT(setMaximum(int)), Qt::QueuedConnection);
		QObject::connect(this, SIGNAL(asyncSetMinimum(int)),	_progress_bar,SLOT(setMinimum(int)), Qt::QueuedConnection);
		QObject::connect(this, SIGNAL(asyncSetValue(int)),		_progress_bar,SLOT(setValue(int)), Qt::QueuedConnection);
		_initialization_helper.Initialize();
	}
	inline void AsyncQProgressBarProgress::Uninitialize(){
		disconnect();
		_progress_bar = nullptr;
		_initialization_helper.Uninitialize();
		POSTCONDITION(_progress_bar == nullptr);
	}
	
}

#endif