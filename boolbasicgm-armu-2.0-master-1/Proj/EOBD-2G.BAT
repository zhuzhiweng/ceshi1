SET PATH=C:\Keil_v5\ARM\ARMCC\Bin;C:\Program Files (x86)\Common Files\Oracle\Java\javapath;C:\windows\system32;C:\windows;C:\windows\System32\Wbem;C:\windows\System32\WindowsPowerShell\v1.0\;C:\Program Files (x86)\Yozosoft\Yozo_Office\Lib\xuggler\bin;C:\Program Files\dotnet\;C:\Program Files\Git\cmd;C:\Program Files\PuTTY\;C:\Users\Administrator\AppData\Local\Microsoft\WindowsApps;C:\Program Files\Microsoft VS Code\bin
SET CPU_TYPE=STM32F103CB
SET CPU_VENDOR=STMicroelectronics
SET UV2_TARGET=EOBD-2G
SET CPU_CLOCK=0x044AA200
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\stm32f10x_it.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\main.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\fl032pif.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\sim868.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\spi_flash.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\gb17691_can_gps.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\gb17691a.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\gps.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\gps_calculation.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\delay.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\bsp.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\can.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\spi.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\uart.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\rtc.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\dma.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\timer.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\iwdg.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\wkup.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\stm32f10x_usart.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\misc.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\stm32f10x_can.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\stm32f10x_gpio.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\stm32f10x_iwdg.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\stm32f10x_rcc.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\stm32f10x_tim.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\stm32f10x_spi.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\stm32f10x_rtc.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\stm32f10x_bkp.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\stm32f10x_pwr.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\stm32f10x_dma.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\stm32f10x_exti.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\core_cm3.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\system_stm32f10x.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmAsm" --Via ".\output\startup_stm32f10x_md._ia"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\rtos_wrapper_cmsis_rtos_rtx.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\rtx_conf_cm.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmLink" --Via ".\Output\EOBD.lnp"
"C:\Keil_v5\ARM\ARMCC\Bin\fromelf.exe" ".\Output\EOBD.axf" --i32combined --output ".\Output\EOBD.hex"
