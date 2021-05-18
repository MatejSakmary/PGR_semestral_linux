CC = g++
CXXFLAGS = -lglfw -lGL -ldl -lassimp 
INCLUDES = -Ivendor/ -Ivendor/imgui -Ivendor/imgui/backends -Ivendor/glad -Ivendor/stb_image -DIMGUI_IMPL_OPENGL_LOADER_GLAD -DGLM_ENABLE_EXPERIMENTAL
OBJDIR = build/
IMGUI_DIR = vendor/imgui
EXE = semestral
SRC_DIR = src

SOURCES = src/main.cpp vendor/glad/glad.c src/shader.cpp src/camera.cpp src/mesh.cpp src/model.cpp vendor/stb_image/stb_image.cpp
SOURCES += src/utils.cpp src/game_state.cpp src/light.cpp src/transform.cpp src/imgui_state.cpp src/bezier.cpp src/cubemap.cpp
SOURCES += src/node.cpp src/quaternion.cpp src/rock.cpp
SOURCES += $(IMGUI_DIR)/imgui.cpp $(IMGUI_DIR)/imgui_demo.cpp $(IMGUI_DIR)/imgui_draw.cpp $(IMGUI_DIR)/imgui_tables.cpp $(IMGUI_DIR)/imgui_widgets.cpp
SOURCES += $(IMGUI_DIR)/backends/imgui_impl_glfw.cpp $(IMGUI_DIR)/backends/imgui_impl_opengl3.cpp

OBJS = $(addprefix build/, $(addsuffix .o, $(basename $(notdir $(SOURCES)))))
OBJS += $(OBJDIR)$(EXE)

$(OBJDIR)$(EXE): $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

$(OBJDIR)%.o:$(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<

$(OBJDIR)%.o:$(IMGUI_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<

$(OBJDIR)%.o:$(IMGUI_DIR)/backends/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<

$(OBJDIR)%.o:vendor/glad/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

$(OBJDIR)%.o:vendor/stb_image/%.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

all: $(OBJDIR)$(EXE)
	@echo Build complete for $(ECHO_MESSAGE)

clean:
	@echo $(OBJS)
	rm -f $(EXE) $(OBJS)