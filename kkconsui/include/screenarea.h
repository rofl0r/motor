#ifndef __KKCONSUI_SCREENAREA_H__
#define __KKCONSUI_SCREENAREA_H__

#include <vector>
#include "conscommon.h"

class screenarea {
    protected:
	vector<chtype *> buffer;
	INT x1, y1, x2, y2;

	void freebuffer();

    public:
	screenarea();
	screenarea(INT fx1, INT fy1, INT fx2, INT fy2);
	virtual ~screenarea();

	void save();
	void save(INT fx1, INT fy1, INT fx2, INT fy2);

	void restore();
	void restore(INT fx1, INT fy1, INT fx2, INT fy2);

	bool empty();
};

#endif
