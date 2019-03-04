################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
%.obj: ../%.asm $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"/home/mishko/ti/ccsv8/tools/compiler/ti-cgt-c2000_18.1.4.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla0 --float_support=fpu32 --vcu_support=vcu0 -O2 --include_path="/home/mishko/ti/ccsv8/tools/compiler/ti-cgt-c2000_18.1.4.LTS/include" --include_path="C:/Users/Mishko Lytvyn/Dropbox/WORK/CCSworkspace/motorsPro/head" --define=FLASH --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

clk.obj: /home/mishko/Dropbox/WORK/CCSworkspace/1MotorsGitBetaC2/src/clk.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"/home/mishko/ti/ccsv8/tools/compiler/ti-cgt-c2000_18.1.4.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla0 --float_support=fpu32 --vcu_support=vcu0 -O2 --include_path="/home/mishko/ti/ccsv8/tools/compiler/ti-cgt-c2000_18.1.4.LTS/include" --include_path="C:/Users/Mishko Lytvyn/Dropbox/WORK/CCSworkspace/motorsPro/head" --define=FLASH --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

gpio.obj: /home/mishko/Dropbox/WORK/CCSworkspace/1MotorsGitBetaC2/src/gpio.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"/home/mishko/ti/ccsv8/tools/compiler/ti-cgt-c2000_18.1.4.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla0 --float_support=fpu32 --vcu_support=vcu0 -O2 --include_path="/home/mishko/ti/ccsv8/tools/compiler/ti-cgt-c2000_18.1.4.LTS/include" --include_path="C:/Users/Mishko Lytvyn/Dropbox/WORK/CCSworkspace/motorsPro/head" --define=FLASH --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

%.obj: ../%.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"/home/mishko/ti/ccsv8/tools/compiler/ti-cgt-c2000_18.1.4.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla0 --float_support=fpu32 --vcu_support=vcu0 -O2 --include_path="/home/mishko/ti/ccsv8/tools/compiler/ti-cgt-c2000_18.1.4.LTS/include" --include_path="C:/Users/Mishko Lytvyn/Dropbox/WORK/CCSworkspace/motorsPro/head" --define=FLASH --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

memCopy.obj: /home/mishko/Dropbox/WORK/CCSworkspace/1MotorsGitBetaC2/src/memCopy.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"/home/mishko/ti/ccsv8/tools/compiler/ti-cgt-c2000_18.1.4.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla0 --float_support=fpu32 --vcu_support=vcu0 -O2 --include_path="/home/mishko/ti/ccsv8/tools/compiler/ti-cgt-c2000_18.1.4.LTS/include" --include_path="C:/Users/Mishko Lytvyn/Dropbox/WORK/CCSworkspace/motorsPro/head" --define=FLASH --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '


