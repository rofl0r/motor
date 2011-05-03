#ifndef __KKCONSUI_SCREENAREA_H__
#define __KKCONSUI_SCREENAREA_H__

#include <vector>
#include "conscommon.h"

class screenarea {
    protected:
	vector<chtype *> buffer;
	int x1, y1, x2, y2;

	void freebuffer();

    public:
	screenarea();
	screenarea(int fx1, int fy1, int fx2, int fy2);
	virtual ~screenarea();

	void save();
	void save(int fx1, int fy1, int fx2, int fy2);

	void restore();
	void restore(int fx1, int fy1, int fx2, int fy2);

	bool empty();
};

#endif
