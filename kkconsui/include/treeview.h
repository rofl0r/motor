#ifndef __KONST_UI_TREE_H_
#define __KONST_UI_TREE_H_

#include <vector>
#include <algorithm>

#include "conf.h"
#include "conscommon.h"
#include "abstractui.h"
#include "cmenus.h"

__KTOOL_BEGIN_NAMESPACE

class treeview : public abstractuicontrol {
    private:
	struct treeviewnode {
	    INT id, parentid, color;
	    bool isnode, isopen;
	    string text;
	    void *ref;

	    bool operator == (const INT aid);
	    bool operator == (const void *aref);
	    bool operator != (const INT aid);
	    bool operator != (const void *aref);
	};

	INT bgcolor, selectcolor, nodecolor, leafcolor, idseq;

	vector<treeviewnode> items;
	vector<treeviewnode> nestlevel;
	vector<treeviewnode> refdeps;

	bool islast(INT id);
	void drawnest(INT y);
	void genmenu(INT parent);
	void init();

    public:
	INT curelem;
	bool collapsable;
	verticalmenu menu;

	treeview(INT nx1, INT ny1, INT nx2, INT ny2, INT nbgcolor, INT nselectcolor, INT nnodecolor, INT nleafcolor);
	treeview(INT nbgcolor, INT nselectcolor, INT nnodecolor, INT nleafcolor);
	treeview();
	~treeview();

	bool empty();

	INT addnode(INT parent, INT color, void *ref, const string &text);
	INT addnode(INT parent, INT color, INT ref, const string &text);
	INT addnode(const string &text);

	INT addnodef(INT parent, INT color, void *ref, const char *fmt, ...);
	INT addnodef(INT parent, INT color, INT ref, const char *fmt, ...);
	INT addnodef(const char *fmt, ...);

	INT addleaf(INT parent, INT color, void *ref, const string &text);
	INT addleaf(INT parent, INT color, INT ref, const string &text);
	INT addleaf(const string &text);

	INT addleaff(INT parent, INT color, void *ref, const char *fmt, ...);
	INT addleaff(INT parent, INT color, INT ref, const char *fmt, ...);
	INT addleaff(const char *fmt, ...);

	void opennode(INT mpos);
	void closenode(INT mpos);

	INT getcount();
	INT getid(INT mpos);
	INT getid(void *ref);
	bool isnode(INT id);
	bool isnodeopen(INT id);

	INT getparent(INT id);
	void *getref(INT id);

	void clear();

	void redraw();
	void *open(INT *n);

	void setcur(INT id);
	void setcoords(INT nx1, INT ny1, INT nx2, INT ny2);
};

__KTOOL_END_NAMESPACE

#ifdef __KTOOL_USE_NAMESPACES

using ktool::treeview;

#endif

#endif
