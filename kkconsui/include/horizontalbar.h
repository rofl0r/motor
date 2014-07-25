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
	INT item, ncolor, scolor, previtem;
	vector<string> items;

	horizontalbar(INT x, INT y, INT nc, INT sc, ...);
	horizontalbar(INT nc, INT sc, ...);
	~horizontalbar();

	void movebar(INT k);
	virtual void redraw();
	void update();
	void align(bamode m);
	void setcoords(INT x, INT y);
};

__KTOOL_END_NAMESPACE

#ifdef __KTOOL_USE_NAMESPACES

using ktool::bamode;
using ktool::baleft;
using ktool::horizontalbar;

#endif

#endif
