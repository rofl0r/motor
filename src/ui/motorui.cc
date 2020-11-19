#include "motorui.h"

motorui::motorui(): outputblocked(false) {
}

motorui::~motorui() {
}

void motorui::execute(int argc, char **argv) {
}

void motorui::loadeditfile(const editfile ef, int options) {
}

void motorui::reloadeditfiles() {
}

void motorui::redraw() {
}

motorui::askresult motorui::ask(const string &answersallowed, const string &text) {
    return cancel;
}

void motorui::log(const string &text) {
}

void motorui::logf(const char *fmt, ...) {
    char buf[512];

    va_list ap;
    va_start(ap, fmt);
    vsprintf(buf, fmt, ap);
    va_end(ap);

    log(buf);
}

motorui::askresult motorui::askf(const string &answersallowed, const char *fmt, ...) {
    char buf[512];

    va_list ap;
    va_start(ap, fmt);
    vsprintf(buf, fmt, ap);
    va_end(ap);

    return ask(answersallowed, buf);
}

vector<motorui::editfile> motorui::geteditfiles() {
    return vector<motorui::editfile>();
}

vector<string> motorui::geteditfile(const string &fname) {
    return vector<string>();
}

bool motorui::editcloseall() {
    return true;
}

motorui::askresult motorui::input(motorui::inputkind kind, string &result,
const string &prompt) {
    return cancel;
}

void motorui::writeoutput(const string &text) {
}

void motorui::doneoutput() {
}

bool motorui::autosave() {
    return false;
}

void motorui::executordone(const string &target, int nerr, int nwarn, int ninfo) {
}

void motorui::setdebugcurrentline(const editfile ef) {
}

void motorui::externalexec(int options, const string &cmd) {
}

void motorui::externalexecf(int options, const char *fmt, ...) {
    char buf[512];

    va_list ap;
    va_start(ap, fmt);
    vsprintf(buf, fmt, ap);
    va_end(ap);

    externalexec(options, buf);
}

void motorui::setdesktop(const vector< pair<string, string> > &asettings) {
}

vector< pair<string, string > > motorui::getdesktop() const {
    return vector< pair<string, string > >();
}

void motorui::updatewatches() {
}

bool motorui::editmodified() const {
    return false;
}

void motorui::setoutputblockmode(bool fblock) {
    outputblocked = fblock;
}

motorui::askresult motorui::notemplate(string &templname) const {
    return motorui::no;
}

// ----------------------------------------------------------------------------

motorui::editfile::editfile(): x(0), y(0) {
}

motorui::editfile::editfile(const string &afname, int ax, int ay):
fname(afname), x(ax), y(ay) {
}

bool motorui::editfile::operator == (const string &afname) {
    return fname == afname;
}

bool motorui::editfile::operator != (const string &afname) {
    return !(*this == afname);
}
