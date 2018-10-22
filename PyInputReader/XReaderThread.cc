#include "XReaderThread.hh"

XReaderThread::XReaderThread(HINSTANCE pInst) : 
	mWinThread(), 
	mLock(), 
	mWinHandler(0),
	mInst(pInst),
	mStopRequested(false),
	mJoyPoller(0),
	mStates()
{
	mWinThread = std::thread(threadFn, this);
}

XReaderThread::~XReaderThread()
{
	mLock.lock();
	mStopRequested = true;
	mLock.unlock();

	mWinThread.join();
	delete mWinHandler;
	delete mJoyPoller;
}

bool XReaderThread::isInitialized() const
{
	mLock.lock();
	bool initialized = mWinHandler->isInitialized();
	mLock.unlock();
	return initialized;
}

const std::vector<JoyState> XReaderThread::getStates() const
{
	mLock.lock();
	std::vector<JoyState> ret(mStates);
	mLock.unlock();

	return ret;
}

void XReaderThread::threadFn(XReaderThread* that)
{
	that->mWinHandler = new WindowsHandler(that->mInst);
	that->mJoyPoller = new JoyPoller(*that->mWinHandler);

	while (true)
	{
		that->mLock.lock();
		if (that->mStopRequested)
		{
			that->mLock.unlock();
			return;
		}

		bool needsToPoll = that->mWinHandler->processMessages();
		if (needsToPoll)
		{
			that->mStates = that->mJoyPoller->poll();
		}
		that->mLock.unlock();

		that->mWinHandler->waitMessages();
	}
}
