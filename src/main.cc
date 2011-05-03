#include "motorconfiguration.h"
#include "motorproject.h"
#include "motormanager.h"
#include "motorexecutor.h"
#include "motordebugger.h"
#include "motorgrepper.h"
#include "motorvcs.h"
#include "motortagbrowser.h"
#include "ncursesui.h"

#ifdef ENABLE_NLS

#include <locale.h>
#include <libintl.h>

#endif

motorproject project;
motorconfiguration conf;
motormanager manager;
motorexecutor executor;
motordebugger debugger;
motorgrepper grepper;
motorvcs vcs;
motortagbrowser tagbrowser;

motorui &ui = thisui;

int main(int argc, char **argv) {
    /*
    * i18n stuff is gonna be done for any kind of ui, since
    * motor has to be multi-lingual by default.
    */
#ifdef ENABLE_NLS
    setlocale(LC_ALL, "");
    bindtextdomain(PACKAGE, LOCALE_DIR);
    textdomain(PACKAGE);
#endif

    conf.load();

    ui.execute(argc, argv);
    conf.save();
}
