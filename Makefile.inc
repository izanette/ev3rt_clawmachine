APPL_COBJS += 

APPL_CXXOBJS += cli_main.o cli_menu.o utils.o motor.o bignumber.o seconds_counter.o

SRCLANG := c++

ifdef CONFIG_EV3RT_APPLICATION

# Include libraries
include $(EV3RT_SDK_LIB_DIR)/libcpp-test/Makefile
include $(EV3RT_SDK_LIB_DIR)/lib2/Makefile

endif
