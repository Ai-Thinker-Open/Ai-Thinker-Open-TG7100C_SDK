NAME := xtensa

$(NAME)_SOURCES := \
                   port.c            \
                   portasm.S         \
                   xtensa_vectors.S  \
                   xtensa_context.S  \
                   xtensa_intr_asm.S \
                   xtensa_intr.c     \
                   xtensa_init.c

$(NAME)_INCLUDES := \
                    ../../mcu/esp32/espidf/components/freertos/include  \
                    ../../mcu/esp32/espidf/components/soc/esp32/include \
                    ../../mcu/esp32/espidf/components/log/include       \
                    ../../mcu/esp32/espidf/components/driver/include

GLOBAL_INCLUDES += . ./include    \
                   ./include/frxt \
                   ../../mcu/esp32/espidf/components/esp32/include

