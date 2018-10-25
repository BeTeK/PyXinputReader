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
	mCallerSafeStates()
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
}

bool XReaderThread::isInitialized() const
{
	return true;
}

const JoyStates* XReaderThread::getStates() const
{
	mLock.lock();
	mCallerSafeStates.resize(mStates.size());
	mCallerSafeStates.assign(mStates.begin(), mStates.end());
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
	WindowsHandler mWinHandler(that->mInst);
	JoyPoller mJoyPoller(mWinHandler);

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
			mJoyPoller.rescan();
		}
		
		if (that->mFreq >= 0)
		{
			mWinHandler.setTimerFreq(that->mFreq);
			that->mFreq = -1;
		}

		bool needsToPoll = mWinHandler.processMessages();
		if (needsToPoll)
		{
			that->mStates = mJoyPoller.poll();
		}
		that->mLock.unlock();

		mWinHandler.waitMessages();
	}
}
