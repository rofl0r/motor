#include "textinputline.h"

textinputline::textinputline() {
    beginpos = length = position = 0;
    idle = 0;
    otherkeys = 0;
    passwordchar = 0;
    selector = 0;
}

textinputline::~textinputline() {
}

void textinputline::setcoords(int x, int y, int len) {
    x1 = x;
    y1 = y2 = y;
    length = len;
    x2 = x1+length;
}

bool textinputline::keymove(int key) {
    bool r = true;

    switch(key) {
	case KEY_LEFT:
	case CTRL('b'):
	    if(--position < 0) position = 0;
	    break;
	case KEY_RIGHT:
	case CTRL('f'):
	    if(++position > value.size()) position = value.size();
	    break;
	case KEY_HOME:
	case CTRL('a'):
	    position = 0;
	    break;
	case KEY_END:
	case CTRL('e'):
	    position = value.size();
	    break;
	default:
	    r = false;
    }

    return r;
}

void textinputline::exechistory() {
    vector<string>::iterator i;
    static verticalmenu m;
    int wy1, wx2, n;

    if(!history.empty()) {
	m.clear();
	m.setcolor(histcolor, histcurcolor);

	for(i = history.begin(), wx2 = 0; i != history.end(); i++) {
	    m.additem(" " + *i);
	    if(wx2 < i->size()) wx2 = i->size();
	}

	wy1 = y1 < LINES/2 ? y1 : y1-history.size()-2;
	if(wy1 < 1) wy1 = 1;

	wx2 += x1+3;
	if(wx2 > COLS-1) wx2 = COLS-1;

	m.setwindow(textwindow(x1, wy1, wx2, 0, histcolor));
	m.scale();
	n = m.open();
	m.close();
    
	if(n) {
	    value = history[n-1];
	    position = 0;
	}
    }
}

void textinputline::redraw() {
    int displen;

    if(position > value.size()) {
	position = value.size();
    }

    if(position < beginpos) {
	beginpos = position;
    }

    while(position > beginpos+length-1) {
	beginpos++;
    }

    displen = value.length()-beginpos;
    if(displen > length) displen = length;

    attrset(color);
    kgotoxy(x1, y1);

    printstring((passwordchar ? string(displen, passwordchar) :
	value.substr(beginpos, displen)) +
	string(length-displen, ' '));

    kgotoxy(x1+position-beginpos, y1);
    refresh();
}

void textinputline::exec() {
    bool fin, go;
    vector<string> sel;
    vector<string>::iterator isel;

    position = 0;
    screenbuffer.save(x1, y1, x2, y2);
    firstiter = true;

    for(fin = false; !fin; ) {
	redraw();
	go = idle ? keypressed() : true;

	if(go) {
	    lastkey = getkey();

	    if(!keymove(lastkey))
	    switch(lastkey) {
		case '\r':
		    if(!passwordchar)
			historyadd(value);
		    fin = true;
		    break;
		case KEY_DC:
		    if(firstiter) {
			value = "";
		    } else {
			value.erase(position, 1);
		    }
		    break;
		case CTRL('k'):
		    value = "";
		    break;
		case CTRL('u'):
		    value.erase(0, position);
		    position = 0;
		    break;
		case CTRL('t'):
		    if(selector) {
			selector->exec();
			selector->close();

			if(strchr("\r ", selector->getlastkey())) {
			    sel = selector->getselected();

			    if(!sel.empty()) {
				position = 0;

				if(sel.size() == 1) {
				    value = sel[0];
				} else {
				    value = "";
				    for(isel = sel.begin();
				    isel != sel.end(); isel++) {
					if(!value.empty()) value += " ";
					value += "\"" + *isel + "\"";
				    }
				}
			    }
			}
		    }
		    break;
		case ALT('h'):
		    if(!passwordchar) exechistory();
		    break;
		case KEY_BACKSPACE:
		case CTRL('h'):
		case 127:
		    if(position) {
			value.erase(--position, 1);
		    }
		    break;
		case KEY_ESC:
		    fin = true;
		    break;
		default:
		    if((lastkey > 31) && (lastkey < 256)) {
			if(firstiter) {
			    value = string(1, (char) lastkey);
			    position = 1;
			} else {
			    value.insert(position++, string(1, (char) lastkey));
			}
		    }
	    }

	    firstiter = false;
	} else {
	    if(idle) {
		(*idle)(*this);
	    }
	}
    }
}

void textinputline::close() {
}

void textinputline::historyadd(const string &buf) {
    while(history.size() > TEXTINPUT_HIST_LIMIT)
	history.erase(history.begin());

    history.insert(history.begin(), buf);
}

void textinputline::setvalue(const string &buf) {
    value = buf;
}

string textinputline::getvalue() const {
    return value;
}

void textinputline::setpasswordchar(char npc) {
    passwordchar = npc;
}

void textinputline::setcolor(int acolor, int ahistcolor, int ahistcurcolor) {
    color = acolor;

    if(!(histcolor = ahistcolor)) histcolor = color;
    if(!(histcurcolor = ahistcurcolor)) histcurcolor = 0;
}

void textinputline::connectselector(fileselector &fsel) {
    selector = &fsel;
}

void textinputline::removeselector() {
    selector = 0;
}

int textinputline::getlastkey() const {
    return lastkey;
}
