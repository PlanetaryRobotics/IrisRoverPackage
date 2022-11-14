
set myPath=%cd%

rem If gmake is different folder, please make a change here
set makePath=c:\ti\ccs900\ccs\utils\bin

set debugPath=%myPath%\TMS570LC43x\lc43_can_boot\Debug
cd %debugPath%
call %makePath%\gmake -k all

set debugPath=%myPath%\RM57Lx\rm57_can_boot\Debug
cd %debugPath%
call %makePath%\gmake -k all


set debugPath=%myPath%\TMS570LS31x\lc31_can_boot\Debug
cd %debugPath%
call %makePath%\gmake -k all

set debugPath=%myPath%\RM48Lx\rm48_can_boot\Debug
cd %debugPath%
call %makePath%\gmake -k all


set debugPath=%myPath%\TMS570LS12x\lc12_can_boot\Debug
cd %debugPath%
call %makePath%\gmake -k all

set debugPath=%myPath%\RM46Lx\rm46_can_boot\Debug
cd %debugPath%
call %makePath%\gmake -k all


set debugPath=%myPath%\TMS570LS07x\ls07_can_boot\Debug
cd %debugPath%
call %makePath%\gmake -k all

set debugPath=%myPath%\RM44Lx\rm44_can_boot\Debug
cd %debugPath%
call %makePath%\gmake -k all


set debugPath=%myPath%\TMS570LS04x\ls04_can_boot\Debug
cd %debugPath%
call %makePath%\gmake -k all

set debugPath=%myPath%\RM42Lx\rm42_can_boot\Debug
cd %debugPath%
call %makePath%\gmake -k all

cd %myPath%