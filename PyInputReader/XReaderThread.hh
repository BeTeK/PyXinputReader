#ifndef X_READER_THREAD_HH
#define X_READER_THREAD_HH

#include "WindowsHandler.hh"
#include "JoyPoller.hh"

#include <thread>
#include <mutex>
#include <vector>

class XReaderThread
{
public:
	XReaderThread() = delete;
	XReaderThread(HINSTANCE pInst);
	virtual ~XReaderThread();
	XReaderThread(const XReaderThread& pOne) = delete;
	XReaderThread& operator = (const XReaderThread& pOne) = delete;

	bool isInitialized() const;
	const JoyStates* getStates() const;
	void rescan();
	void setFreq(unsigned int pFreq);
private:
	static void threadFn(XReaderThread* that);

	std::thread mWinThread;
	mutable std::mutex mLock;
	HINSTANCE mInst;
	bool mStopRequested;
	std::vector<JoyState> mStates;
	int mFreq;
	mutable JoyStates mJoyStates;
	mutable std::vector<JoyState> mCallerSafeStates;
	bool mRequestRescan;
	JoyPoller* mJoyPoller;
	WindowsHandler* mWinHandler;
};

#endif // !X_READER_THREAD_HH




