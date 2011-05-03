#ifndef __UIWATCHER_H__
#define __UIWATCHER_H__

#include "motorcommon.h"
#include "ncursesui.h"

__MOTOR_BEGIN_NAMESPACE

class uiwatcher {
    private:
	bool fvisible;
	verticalmenu m;
	int startpos, curpos;

	static int keyhandler(verticalmenu &m, int k);

    public:
	uiwatcher();
	~uiwatcher();

	bool visible();
	void activate();
	void close();

	void update();
	void add();
	void exec();
	void modify(const string &expr, const string &val);
};

__MOTOR_END_NAMESPACE

#ifdef __MOTOR_USE_NAMESPACES

using motor::uiwatcher;

#endif

extern uiwatcher watcher;

#endif
