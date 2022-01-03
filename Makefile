PROJECT = cobj

include mk/flags.mk

CPPFLAGS += -DCOBJ_BUILD -Isrc
CANYFLAGS += -fvisibility=hidden

HEADERS := $(wildcard include/*.h)
SOURCES := $(sort $(wildcard src/*.c src/*/*.c))
OBJS := $(SOURCES:.c=.o)
DOCDIR := docs/html

include mk/libs.mk
include mk/prerequisties.mk
