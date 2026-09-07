# --------------------------------
OUTPUT = resource.o
# --------------------------------
ifeq ($(OS),Windows_NT)
    RM = del /Q /F 2>nul
else
    RM = rm -f
endif
# --------------------------------
all: $(OUTPUT)

%.o: %.rc
	windres -O coff $< -o $@
	
clean:
	$(RM) $(OUTPUT)
# --------------------------------