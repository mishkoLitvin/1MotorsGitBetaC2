################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
F2806x_CodeStartBranch.obj: ../F2806x_CodeStartBranch.asm $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"/opt/ti/ccsv7/tools/compiler/ti-cgt-c2000_6.2.7/bin/cl2000" -v28 -ml -mt --cla_support=cla0 --float_support=fpu32 --vcu_support=vcu0 -O4 --opt_for_speed=2 --include_path="/opt/ti/ccsv7/tools/compiler/ti-cgt-c2000_6.2.7/include" --include_path="/home/mishko/Dropbox/WORK/CCSworkspace/1MotorsGitBetaC2/head/" --include_path="C:/Users/Mishko Lytvyn/Dropbox/WORK/CCSworkspace/1MotorsGitBetaC2/head" --include_path="C:/Users/user/Dropbox/WORK/CCSworkspace/1MotorsGitBetaC2/head" -g --define=FLASH --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="F2806x_CodeStartBranch.d" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

clk.obj: /home/mishko/Dropbox/WORK/CCSworkspace/1MotorsGitBetaC2/src/clk.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"/opt/ti/ccsv7/tools/compiler/ti-cgt-c2000_6.2.7/bin/cl2000" -v28 -ml -mt --cla_support=cla0 --float_support=fpu32 --vcu_support=vcu0 -O4 --opt_for_speed=2 --include_path="/opt/ti/ccsv7/tools/compiler/ti-cgt-c2000_6.2.7/include" --include_path="/home/mishko/Dropbox/WORK/CCSworkspace/1MotorsGitBetaC2/head/" --include_path="C:/Users/Mishko Lytvyn/Dropbox/WORK/CCSworkspace/1MotorsGitBetaC2/head" --include_path="C:/Users/user/Dropbox/WORK/CCSworkspace/1MotorsGitBetaC2/head" -g --define=FLASH --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="clk.d" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

gpio.obj: /home/mishko/Dropbox/WORK/CCSworkspace/1MotorsGitBetaC2/src/gpio.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"/opt/ti/ccsv7/tools/compiler/ti-cgt-c2000_6.2.7/bin/cl2000" -v28 -ml -mt --cla_support=cla0 --float_support=fpu32 --vcu_support=vcu0 -O4 --opt_for_speed=2 --include_path="/opt/ti/ccsv7/tools/compiler/ti-cgt-c2000_6.2.7/include" --include_path="/home/mishko/Dropbox/WORK/CCSworkspace/1MotorsGitBetaC2/head/" --include_path="C:/Users/Mishko Lytvyn/Dropbox/WORK/CCSworkspace/1MotorsGitBetaC2/head" --include_path="C:/Users/user/Dropbox/WORK/CCSworkspace/1MotorsGitBetaC2/head" -g --define=FLASH --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="gpio.d" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

main.obj: ../main.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"/opt/ti/ccsv7/tools/compiler/ti-cgt-c2000_6.2.7/bin/cl2000" -v28 -ml -mt --cla_support=cla0 --float_support=fpu32 --vcu_support=vcu0 -O4 --opt_for_speed=2 --include_path="/opt/ti/ccsv7/tools/compiler/ti-cgt-c2000_6.2.7/include" --include_path="/home/mishko/Dropbox/WORK/CCSworkspace/1MotorsGitBetaC2/head/" --include_path="C:/Users/Mishko Lytvyn/Dropbox/WORK/CCSworkspace/1MotorsGitBetaC2/head" --include_path="C:/Users/user/Dropbox/WORK/CCSworkspace/1MotorsGitBetaC2/head" -g --define=FLASH --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="main.d" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

memCopy.obj: /home/mishko/Dropbox/WORK/CCSworkspace/1MotorsGitBetaC2/src/memCopy.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"/opt/ti/ccsv7/tools/compiler/ti-cgt-c2000_6.2.7/bin/cl2000" -v28 -ml -mt --cla_support=cla0 --float_support=fpu32 --vcu_support=vcu0 -O4 --opt_for_speed=2 --include_path="/opt/ti/ccsv7/tools/compiler/ti-cgt-c2000_6.2.7/include" --include_path="/home/mishko/Dropbox/WORK/CCSworkspace/1MotorsGitBetaC2/head/" --include_path="C:/Users/Mishko Lytvyn/Dropbox/WORK/CCSworkspace/1MotorsGitBetaC2/head" --include_path="C:/Users/user/Dropbox/WORK/CCSworkspace/1MotorsGitBetaC2/head" -g --define=FLASH --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="memCopy.d" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '


