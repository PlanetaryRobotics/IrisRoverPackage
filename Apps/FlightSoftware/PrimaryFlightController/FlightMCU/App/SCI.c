#include <App/SCI.h>

#include "sys_vim.h"

void sciIntInit()
{
    // VIM is only accessible in privileged mode!
    vimChannelMap(SCI_LVL0_INT, SCI_LVL0_INT, sciHighLevelInterrupt);
    vimEnableInterrupt(SCI_LVL0_INT, SYS_IRQ);
}
