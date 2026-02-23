#include <App/SCILIN.h>

#include "sys_vim.h"

void scilinIntInit()
{
    // VIM is only accessible in privileged mode!
    vimChannelMap(SCILIN_LVL0_INT, SCILIN_LVL0_INT, linHighLevelInterrupt);
    vimEnableInterrupt(SCILIN_LVL0_INT, SYS_IRQ);
}
