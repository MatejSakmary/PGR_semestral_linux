CC = g++
CXXFLAGS = -lglfw -lGL -ldl 
INCLUDES = -Ivendor/ -Ivendor/imgui -Ivendor/imgui/backends -Ivendor/glad -DIMGUI_IMPL_OPENGL_LOADER_GLAD 
OBJDIR = build/
IMGUI_DIR = vendor/imgui
EXE = semestral
SRC_DIR = src

SOURCES = src/main.cpp vendor/glad/glad.c src/shader.cpp src/camera.cpp
SOURCES += $(IMGUI_DIR)/imgui.cpp $(IMGUI_DIR)/imgui_demo.cpp $(IMGUI_DIR)/imgui_draw.cpp $(IMGUI_DIR)/imgui_tables.cpp $(IMGUI_DIR)/imgui_widgets.cpp
SOURCES += $(IMGUI_DIR)/backends/imgui_impl_glfw.cpp $(IMGUI_DIR)/backends/imgui_impl_opengl3.cpp

OBJS = $(addprefix build/, $(addsuffix .o, $(basename $(notdir $(SOURCES)))))

$(OBJDIR)$(EXE): $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

$(OBJDIR)%.o:$(SRC_DIR)/%.cpp
	# @echo $(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<

$(OBJDIR)%.o:$(IMGUI_DIR)/%.cpp
	# @echo $(CXX) $(CXXFLAGS) $(INCLUDES)-c -o $@ $<
	$(CXX) $(CXXFLAGS) $(INCLUDES)-c -o $@ $<

$(OBJDIR)%.o:$(IMGUI_DIR)/backends/%.cpp
	# @echo $(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<

$(OBJDIR)%.o:vendor/glad/%.c
	# @echo $(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

all: $(EXE)
	@echo Build complete for $(ECHO_MESSAGE)

clean:
	@echo $(OBJS)
	rm -f $(EXE) $(OBJS)