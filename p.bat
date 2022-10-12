@echo off
setlocal enabledelayedexpansion

    set stlink="C:\Program Files\STMicroelectronics\STM32 ST-LINK Utility\ST-LINK Utility\ST-LINK_CLI.exe"
if exist %stlink% (goto FILEOK)
    set stlink="C:\Program Files (x86)\STMicroelectronics\STM32 ST-LINK Utility\ST-LINK Utility\ST-LINK_CLI.exe"
if exist %stlink% (goto FILEOK)
    set stlink="D:\Program Files (x86)\STMicroelectronics\STM32 ST-LINK Utility\ST-LINK Utility\ST-LINK_CLI.exe"
if exist %stlink% (goto FILEOK)
    set stlink="C:\Program Files\STMicroelectronics\STM32 ST-LINK Utility\ST-LINK Utility\ST-LINK_CLI.exe"
if exist %stlink% (goto FILEOK)
goto BAT_END
rem -------------------------------------------------------
:FILEOK

if !ID!==0 (goto :ID_START)
if !ID!==1 (goto :ID_START)
if !ID!==2 (goto :ID_START)
if !ID!==3 (goto :ID_START)
if !ID!==4 (goto :ID_START)

    %stlink%            -P "firmware.hex" -Rst

goto BAT_END
rem -------------------------------------------------------
:ID_START
echo ID[%ID%]
%stlink% -C ID=%ID% -P "firmwear.hex" -Rst
rem -------------------------------------------------------

:BAT_END

