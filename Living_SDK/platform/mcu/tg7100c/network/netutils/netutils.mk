# Component Makefile
#
## These include paths would be exported to project level
NAME := tg7100c_netutils

$(NAME)_SOURCES += iperf/iperf.c \
				   #tcpclient/tcpclient.c \
				   #netstat/netstat.c \

$(NAME)_INCLUDES += include/netutils

 
