set(BUILD_SYSTEM_NAME   aarch64)
set(AARCH_64_C  /opt/gcc-linaro-7.5.0-2019.12-x86_64_aarch64-linux-gnu/bin)

set(CMAKE_C_COMPILER     ${AARCH_64_C}/aarch64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER     ${AARCH_64_C}/aarch64-linux-gnu-g++)
set(LINKER ${AARCH_64_C}/aarch64-linux-gnu-ld)
set(AS     ${AARCH_64_C}/aarch64-linux-gnu-as)
set(AR     ${AARCH_64_C}/aarch64-linux-gnu-ar)
set(OBJCOPY     ${AARCH_64_C}/aarch64-linux-gnu-objcopy)
set(OBJDUMP     ${AARCH_64_C}/aarch64-linux-gnu-objdump)
set(SIZE     ${AARCH_64_C}/aarch64-linux-gnu-size)

include_directories(${AARCH_64_C}/../include)
link_directories(${AARCH_64_C}/../lib)