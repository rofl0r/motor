#ifndef __KONST_UI_SKEL_H_
#define __KONST_UI_SKEL_H_

#include "conf.h"
#include "conscommon.h"
#include "screenarea.h"

__KTOOL_BEGIN_NAMESPACE

class abstractuicontrol {
    protected:
	screenarea screenbuffer;
	bool fisopen, finished, aborted;

    public:
	bool emacs;
	int x1, x2, y1, y2;

	abstractuicontrol();
	abstractuicontrol(const abstractuicontrol &a);
	~abstractuicontrol();

	virtual bool empty() const;
	bool getaborted() const;
	bool isopen() const;

	virtual void redraw();
	virtual void close();
	virtual void abort();
};

__KTOOL_END_NAMESPACE

#ifdef __KTOOL_USE_NAMESPACES

using ktool::abstractuicontrol;

#endif

#endif
