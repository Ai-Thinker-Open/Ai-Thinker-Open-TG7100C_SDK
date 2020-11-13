NAME := irot

GLOBAL_INCLUDES    += ../include/irot

ifeq ($(findstring linuxhost, $(BUILD_STRING)), linuxhost)
$(NAME)_COMPONENTS := irot.km
else ifeq ($(findstring mk3060, $(BUILD_STRING)), mk3060)
$(NAME)_COMPONENTS := irot.km
else ifeq ($(findstring mk3080, $(BUILD_STRING)), mk3080)
$(NAME)_COMPONENTS := irot.km
else ifeq ($(findstring mk5080, $(BUILD_STRING)), mk5080)
$(NAME)_COMPONENTS := irot.km
else ifeq ($(findstring cb2201, $(BUILD_STRING)), cb2201)
$(NAME)_COMPONENTS := irot.tee
else ifeq ($(findstring uno-91h, $(BUILD_STRING)), uno-91h)
$(NAME)_COMPONENTS := irot.km
else ifeq ($(findstring hf-lpt230, $(BUILD_STRING)), hf-lpt230)
$(NAME)_COMPONENTS := irot.km
else ifeq ($(findstring hf-lpb135, $(BUILD_STRING)), hf-lpb135)
$(NAME)_COMPONENTS := irot.km
else ifeq ($(findstring hf-lpt130, $(BUILD_STRING)), hf-lpt130)
$(NAME)_COMPONENTS := irot.km
else ifeq ($(findstring hf-lpb130, $(BUILD_STRING)), hf-lpb130)
$(NAME)_COMPONENTS := irot.km
else ifeq ($(findstring amebaz, $(BUILD_STRING)), amebaz)
$(NAME)_COMPONENTS := irot.km
endif

