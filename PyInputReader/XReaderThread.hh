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
	const std::vector<JoyState> getStates() const;
private:
	static void threadFn(XReaderThread* that);

	std::thread mWinThread;
	mutable std::mutex mLock;
	WindowsHandler* mWinHandler;
	HINSTANCE mInst;
	bool mStopRequested;
	JoyPoller* mJoyPoller;
	std::vector<JoyState> mStates;
};

#endif // !X_READER_THREAD_HH




