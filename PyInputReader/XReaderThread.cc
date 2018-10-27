#include "XReaderThread.hh"

XReaderThread::XReaderThread(HINSTANCE pInst) : 
	mWinThread(), 
	mLock(), 
	mInst(pInst),
	mStopRequested(false),
	mStates(),
	mRequestRescan(false),
	mFreq(-1),
	mJoyStates(),
	mCallerSafeStates(),
	mJoyPoller(0),
	mWinHandler(0)
{
	mJoyStates.joyStates = 0;
	mWinThread = std::thread(threadFn, this);
}

XReaderThread::~XReaderThread()
{
	mLock.lock();
	mStopRequested = true;
	mLock.unlock();

	mWinThread.join();

	if (mJoyPoller)
	{
		delete mJoyPoller;
	}

	if (mWinHandler != 0)
	{
		delete mWinHandler;
	}
}

bool XReaderThread::isInitialized() const
{
	return true;
}

const JoyStates* XReaderThread::getStates() const
{
	mLock.lock();
	if (mJoyPoller != 0)
	{
		auto ret = mJoyPoller->poll();
		mCallerSafeStates.resize(ret.size());
		mCallerSafeStates.assign(ret.begin(), ret.end());
	}
	mLock.unlock();

	if (mCallerSafeStates.size() > 0)
	{
		mJoyStates.joyStates = &mCallerSafeStates.at(0);
	}
	else
	{
		mJoyStates.joyStates = 0;
	}

	mJoyStates.stateCount = static_cast<int>(mCallerSafeStates.size());

	return &mJoyStates;
}

void XReaderThread::rescan()
{
	mLock.lock();
	mRequestRescan = true;
	mLock.unlock();
}

void XReaderThread::setFreq(unsigned int pFreq)
{
	mLock.lock();
	if (pFreq > 0)
	{
		mFreq = pFreq;
	}
	mLock.unlock();
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

		if (that->mRequestRescan)
		{
			that->mRequestRescan = false;
			that->mJoyPoller->rescan();
		}
		
		if (that->mFreq >= 0)
		{
			that->mWinHandler->setTimerFreq(that->mFreq);
			that->mFreq = -1;
		}

		bool needsToPoll = that->mWinHandler->processMessages();
		if (needsToPoll)
		{
			// that->mStates = mJoyPoller.poll();
		}
		that->mLock.unlock();

		that->mWinHandler->waitMessages();
	}

	if (that->mJoyPoller)
	{
		delete that->mJoyPoller;
	}

	if (that->mWinHandler != 0)
	{
		delete that->mWinHandler;
	}
}
