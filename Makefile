CC = gcc
CFLAGS = -std=gnu99 -Wextra -Wall # -Wno-unused-parameter -Werror -fsanitize=address

TARGET = router

SRCS = conversion.c network.c check.c
OBJS = conversion.o network.o check.o

.PHONY: all clean distclean

all = $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

clean:
		$(RM) $(OBJS) 

distclean:
		$(RM) $(OBJS) $(TARGET)