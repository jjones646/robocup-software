# set the arm toolchain and include it so we can use it
set(CMAKE_TOOLCHAIN_FILE ${ARM_TOOLCHAIN_FILE})
include(${ARM_TOOLCHAIN_FILE})

# Set compiler and linker flags
set(CMAKE_CXX_FLAGS         ${MBED_CMAKE_CXX_FLAGS}         )
set(CMAKE_C_FLAGS           ${MBED_CMAKE_C_FLAGS}           )
set(CMAKE_EXE_LINKER_FLAGS  ${MBED_CMAKE_EXE_LINKER_FLAGS}  )

get_filename_component(CMSIS_DAP_DIR "${PROJECT_SOURCE_DIR}/external/CMSIS-DAP/" ABSOLUTE)

set(MBED_REPO_DIR "${PROJECT_SOURCE_DIR}/external/mbed_lib_build_tools")
set(CMSIS_DAP_INCLUDES "${MBED_REPO_DIR}/libraries/rtos/rtx/TARGET_CORTEX_M/")

list(APPEND CMSIS_DAP_INCLUDES "${CMSIS_DAP_DIR}/shared/rtos/")
list(APPEND CMSIS_DAP_INCLUDES "${CMSIS_DAP_DIR}/shared/USBStack/INC/")
list(APPEND CMSIS_DAP_INCLUDES "${CMSIS_DAP_DIR}/shared/cmsis/TARGET_NXP/TARGET_LPC11UXX/")
list(APPEND CMSIS_DAP_INCLUDES "${CMSIS_DAP_DIR}/bootloader/Common/inc/")
list(APPEND CMSIS_DAP_INCLUDES "${CMSIS_DAP_DIR}/bootloader/hal/TARGET_NXP/TARGET_LPC11U35/")
list(APPEND CMSIS_DAP_INCLUDES "${CMSIS_DAP_DIR}/interface/Common/inc/")
list(APPEND CMSIS_DAP_INCLUDES "${CMSIS_DAP_DIR}/interface/interface/hal/TARGET_NXP/TARGET_LPC11U35/")

file(GLOB CMSIS_DAP_SRC_RTX "${MBED_REPO_DIR}/libraries/rtos/rtx/TARGET_CORTEX_M/*.c")
file(GLOB CMSIS_DAP_SRC_ASM "${MBED_REPO_DIR}/libraries/rtos/rtx/TARGET_CORTEX_M/TARGET_M0/TOOLCHAIN_GCC/*.S")

set(CMSIS_DAP_SRC ${CMSIS_DAP_SRC_RTX} ${CMSIS_DAP_SRC_ASM})
message(STATUS ${CMSIS_DAP_SRC})
list(APPEND CMSIS_DAP_SRC "${CMSIS_DAP_DIR}/shared/USBStack/SRC/usbd_cdc_acm.c")
list(APPEND CMSIS_DAP_SRC "${CMSIS_DAP_DIR}/shared/USBStack/SRC/usbd_core.c")
list(APPEND CMSIS_DAP_SRC "${CMSIS_DAP_DIR}/shared/USBStack/SRC/usbd_core_cdc.c")
list(APPEND CMSIS_DAP_SRC "${CMSIS_DAP_DIR}/shared/USBStack/SRC/usbd_core_hid.c")
list(APPEND CMSIS_DAP_SRC "${CMSIS_DAP_DIR}/shared/USBStack/SRC/usbd_core_msc.c")
list(APPEND CMSIS_DAP_SRC "${CMSIS_DAP_DIR}/shared/USBStack/SRC/usbd_hid.c")
list(APPEND CMSIS_DAP_SRC "${CMSIS_DAP_DIR}/shared/USBStack/SRC/usbd_msc.c")
list(APPEND CMSIS_DAP_SRC "${CMSIS_DAP_DIR}/shared/USBStack/INC/usb_lib.c")
list(APPEND CMSIS_DAP_SRC "${CMSIS_DAP_DIR}/bootloader/Common/src/RTX_Config.c")
# list(APPEND CMSIS_DAP_SRC "${CMSIS_DAP_DIR}/bootloader/Common/src/SVC_Table.s")
list(APPEND CMSIS_DAP_SRC "${CMSIS_DAP_DIR}/bootloader/Common/src/flash_svc.c")
list(APPEND CMSIS_DAP_SRC "${CMSIS_DAP_DIR}/bootloader/Common/src/usb_config.c")
list(APPEND CMSIS_DAP_SRC "${CMSIS_DAP_DIR}/bootloader/Common/src/usbd_user_msc.c")
list(APPEND CMSIS_DAP_SRC "${CMSIS_DAP_DIR}/bootloader/Common/src/version.c")
list(APPEND CMSIS_DAP_SRC "${CMSIS_DAP_DIR}/bootloader/hal/TARGET_NXP/TARGET_LPC11U35/flash_hal.c")
list(APPEND CMSIS_DAP_SRC "${CMSIS_DAP_DIR}/bootloader/hal/TARGET_NXP/TARGET_LPC11U35/gpio.c")
list(APPEND CMSIS_DAP_SRC "${CMSIS_DAP_DIR}/bootloader/hal/TARGET_NXP/TARGET_LPC11U35/read_uid.c")
list(APPEND CMSIS_DAP_SRC "${CMSIS_DAP_DIR}/bootloader/hal/TARGET_NXP/TARGET_LPC11U35/usbd_LPC11Uxx.c")
list(APPEND CMSIS_DAP_SRC "${CMSIS_DAP_DIR}/bootloader/hal/TARGET_NXP/TARGET_LPC11U35/vector_table.c")
list(APPEND CMSIS_DAP_SRC "${CMSIS_DAP_DIR}/interface/Common/src/board.c")
list(APPEND CMSIS_DAP_SRC "${CMSIS_DAP_DIR}/interface/Common/src/DAP.c")
list(APPEND CMSIS_DAP_SRC "${CMSIS_DAP_DIR}/interface/Common/src/dap_vendor_command.c")
list(APPEND CMSIS_DAP_SRC "${CMSIS_DAP_DIR}/interface/Common/src/JTAG_DP.c")
list(APPEND CMSIS_DAP_SRC "${CMSIS_DAP_DIR}/interface/Common/src/main.c") # main.c
list(APPEND CMSIS_DAP_SRC "${CMSIS_DAP_DIR}/interface/Common/src/RTX_Config.c")
list(APPEND CMSIS_DAP_SRC "${CMSIS_DAP_DIR}/interface/Common/src/semihost.c")
list(APPEND CMSIS_DAP_SRC "${CMSIS_DAP_DIR}/interface/Common/src/serial.c")
# list(APPEND CMSIS_DAP_SRC "${CMSIS_DAP_DIR}/interface/Common/src/SVC_Table.s")
list(APPEND CMSIS_DAP_SRC "${CMSIS_DAP_DIR}/interface/Common/src/swd_host.c")
list(APPEND CMSIS_DAP_SRC "${CMSIS_DAP_DIR}/interface/Common/src/SW_DP.c")
list(APPEND CMSIS_DAP_SRC "${CMSIS_DAP_DIR}/interface/Common/src/usb_config.c")
list(APPEND CMSIS_DAP_SRC "${CMSIS_DAP_DIR}/interface/Common/src/usbd_user_cdc_acm.c")
list(APPEND CMSIS_DAP_SRC "${CMSIS_DAP_DIR}/interface/Common/src/usbd_user_hid.c")
list(APPEND CMSIS_DAP_SRC "${CMSIS_DAP_DIR}/interface/Common/src/usbd_user_msc.c")
list(APPEND CMSIS_DAP_SRC "${CMSIS_DAP_DIR}/interface/Common/src/version.c")
list(APPEND CMSIS_DAP_SRC "${CMSIS_DAP_DIR}/interface/interface/hal/TARGET_NXP/TARGET_LPC11U35/gpio.c")
list(APPEND CMSIS_DAP_SRC "${CMSIS_DAP_DIR}/interface/interface/hal/TARGET_NXP/TARGET_LPC11U35/read_uid.c")
list(APPEND CMSIS_DAP_SRC "${CMSIS_DAP_DIR}/interface/interface/hal/TARGET_NXP/TARGET_LPC11U35/uart.c")
list(APPEND CMSIS_DAP_SRC "${CMSIS_DAP_DIR}/interface/interface/hal/TARGET_NXP/TARGET_LPC11U35/usbd_LPC11Uxx.c")
list(APPEND CMSIS_DAP_SRC "${CMSIS_DAP_DIR}/interface/interface/hal/TARGET_NXP/TARGET_LPC11U35/usbd_LPC11Uxx.c")

set(MBED_CMAKE_EXE_LINKER_FLAGS "-Wl,--gc-sections -Wl,--wrap,main --specs=nosys.specs  -u _printf_float -u _scanf_float")
set(CMAKE_EXE_LINKER_FLAGS "${MBED_CMAKE_EXE_LINKER_FLAGS} -T '${CMSIS_DAP_DIR}/shared/cmsis/TARGET_NXP/TARGET_LPC11UXX/TOOLCHAIN_ARM_STD/startup_LPC11Uxx.s.ld' -static")

set(COMMON_FLAGS " -Wall -Wextra -fno-builtin -MMD -fno-delete-null-pointer-checks")
set(COMMON_FLAGS "${COMMON_FLAGS} -mcpu=cortex-m0 -O2 -mthumb -fno-exceptions -msoft-float -ffunction-sections -fdata-sections -g -fno-common -fmessage-length=0")
set(MBED_TARGET_ISA "CORTEX_M0")
set(COMMON_DEFINES "-DTARGET_${MBED_PLATFORM_UPPERC} -DTARGET_${MBED_TARGET_ISA} -DTARGET_${MBED_TARGET_VENDOR} -DTOOLCHAIN_${MBED_TOOLCHAIN} -D__CORE_CM0")
SET(CMAKE_C_FLAGS      "${COMMON_FLAGS} ${CMAKE_CXX_FLAGS} ${COMMON_DEFINES} -std=gnu99")
SET(CMAKE_CXX_FLAGS    "${COMMON_FLAGS} ${CMAKE_CXX_FLAGS} ${COMMON_DEFINES}")

foreach(ff ${CMSIS_DAP_SRC})
    get_filename_component(file ${ff} NAME)
    message(STATUS ${ff})
endforeach()

message(STATUS ${MBED_REPO_DIR})

# add the target for making the elf file
add_executable(mbed_interface_elf ${CMSIS_DAP_SRC})

# depends on the mbed libraries and all of the accessory libraries
add_dependencies(mbed_interface_elf mbed_libraries)

target_link_libraries(mbed_interface_elf ${MBED_LIBRARY})

# include directories
target_include_directories(mbed_interface_elf PUBLIC ${CMSIS_DAP_INCLUDES})

# the final product is the .bin file, not the elf one.  We hide this away in the build dir
set_target_properties(mbed_interface_elf PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

# only build robot firmware if specifically instructed
set_target_properties(mbed_interface_elf PROPERTIES EXCLUDE_FROM_ALL TRUE)

# custom target for creating a .bin file from an elf binary
add_custom_target(mbed_interface
    ${ARM_OBJCOPY} -O binary mbed_interface_elf rj-mbed_if.bin
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/rj-mbed_if.bin ${PROJECT_SOURCE_DIR}/run/
    DEPENDS mbed_interface_elf
    COMMENT "objcopying to make mbed-compatible executable"
)
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES ${PROJECT_SOURCE_DIR}/run/rj-mbed_if.bin)

# ./interface/target/hal/DBG_NXP/DBG_LPC1114
# ├── ./interface/target/hal/DBG_NXP/DBG_LPC1114/target_flash.h
# └── ./interface/target/hal/DBG_NXP/DBG_LPC1114/target_reset.c
# ./interface/target/hal/DBG_NXP/DBG_LPC11U68
# ├── ./interface/target/hal/DBG_NXP/DBG_LPC11U68/target_flash.h
# └── ./interface/target/hal/DBG_NXP/DBG_LPC11U68/target_reset_LPC11U68.c
# ./interface/target/hal/DBG_NXP/DBG_LPC1768
# ├── ./interface/target/hal/DBG_NXP/DBG_LPC1768/target_flash.h
# └── ./interface/target/hal/DBG_NXP/DBG_LPC1768/target_reset.c

# │   │   └── ./shared/cmsis/TARGET_NXP
# │   │       ├── ./shared/cmsis/TARGET_NXP/TARGET_LPC11UXX
# │   │       │   ├── ./shared/cmsis/TARGET_NXP/TARGET_LPC11UXX/LPC11Uxx.h
# │   │       │   ├── ./shared/cmsis/TARGET_NXP/TARGET_LPC11UXX/system_LPC11Uxx.c
# │   │       │   ├── ./shared/cmsis/TARGET_NXP/TARGET_LPC11UXX/system_LPC11Uxx.h
# │   │       │   └── ./shared/cmsis/TARGET_NXP/TARGET_LPC11UXX/TOOLCHAIN_ARM_STD
# │   │       │       ├── ./shared/cmsis/TARGET_NXP/TARGET_LPC11UXX/TOOLCHAIN_ARM_STD/LPC11U35_bootloader.sct
# │   │       │       ├── ./shared/cmsis/TARGET_NXP/TARGET_LPC11UXX/TOOLCHAIN_ARM_STD/LPC11U35_LPC4330_bootloader.sct
# │   │       │       ├── ./shared/cmsis/TARGET_NXP/TARGET_LPC11UXX/TOOLCHAIN_ARM_STD/LPC11U35_LPC4330.sct
# │   │       │       ├── ./shared/cmsis/TARGET_NXP/TARGET_LPC11UXX/TOOLCHAIN_ARM_STD/LPC11U35_mbed_bootloader.sct
# │   │       │       ├── ./shared/cmsis/TARGET_NXP/TARGET_LPC11UXX/TOOLCHAIN_ARM_STD/LPC11U35.sct
# │   │       │       └── ./


# message(FATAL_ERROR ${CMSIS_DAP_DIR})


# "LPC11U35/401"
# "NXP (founded by Philips)""
# IRAM(0x10000000-0x10001FFF)
# IRAM2(0x20004000-0x200047FF)
# IROM(0-0xFFFF)
# CLOCK(12000000)
# CPUTYPE("Cortex-M0")
# "CMSIS-DAP/shared/cmsis/TARGET_NXP/TARGET_LPC11UXX/TOOLCHAIN_ARM_STD/startup_LPC11Uxx.s"
# "LPC11Uxx.h"

# <BeforeMake>
# <RunUserProg1>1</RunUserProg1>
# <UserProg1Name>python ..\..\..\tools\pre_build_script.py</UserProg1Name>
# </BeforeMake>

# <AfterMake>
# <RunUserProg1>1</RunUserProg1>
# <RunUserProg2>1</RunUserProg2>
# <UserProg1Name>$KARM\BIN\ElfDwT.exe #L</UserProg1Name>
# <UserProg2Name>fromelf --bin -o "$L@L.bin" "#L"</UserProg2Name>
# </AfterMake>
