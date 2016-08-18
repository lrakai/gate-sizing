CXXFLAGS =	-O0 -g -Wall -fmessage-length=0

SRCDIR = src
OBJDIR = obj

CPPS = $(wildcard $(SRCDIR)/*.cpp)
OBJS = $(addprefix $(OBJDIR)/,$(notdir $(CPPS:.cpp=.o)))


TARGET   = gateSizing


$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

$(TARGET):	$(OBJS)
	$(CXX) -o $@ $^

$(OBJS): | $(OBJDIR)

$(OBJDIR):
	mkdir -p $(OBJDIR)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
