#ifndef __KONST_HORBAR_H_
#define __KONST_HORBAR_H_

#include <vector>

#include "conf.h"
#include "conscommon.h"
#include "abstractui.h"
#include "kkstrtext.h"

__KTOOL_BEGIN_NAMESPACE

enum bamode {
    baleft
};

class horizontalbar : public abstractuicontrol {
    protected:
    public:
	bool done;
	int item, ncolor, scolor, previtem;
	vector<string> items;

	horizontalbar(int x, int y, int nc, int sc, ...);
	horizontalbar(int nc, int sc, ...);
	~horizontalbar();

	void movebar(int k);
	virtual void redraw();
	void update();
	void align(bamode m);
	void setcoords(int x, int y);
};

__KTOOL_END_NAMESPACE

#ifdef __KTOOL_USE_NAMESPACES

using ktool::bamode;
using ktool::baleft;
using ktool::horizontalbar;

#endif

#endif
