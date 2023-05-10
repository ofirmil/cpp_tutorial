PYBIND11_INCLUDES = $(shell python3 -m pybind11 --includes)
PYTHON3_LIB_EXT = $(shell python3-config --extension-suffix)

# -I./ -I./yaml-cpp/include/ optimus.cpp robot_config.cpp -o example$(python3-config --extension-suffix) -lyaml-cpp -L./build/yaml-cpp/
# g++ -Wfatal-errors -Wall -Wextra -Werror -O3 -shared -std=c++20 -fPIC $(python3 -m pybind11 --includes) -I./yaml-cpp/include/ optimus.cpp robot_config.cpp -o robot_brain$(python3-config --extension-suffix) -lyaml-cpp -L./build/yaml-cpp/
CXX_INCLUDES := $(addprefix -I,./submodules/yaml-cpp/include/ ./)
CXX_INCLUDES += $(PYBIND11_INCLUDES)

CXX_COMPILER := g++
CXX_FLAGS := -std=c++20 -Wfatal-errors -Wall -Wextra -Werror -O3 -fPIC
CXX_FLAGS_STATIC := -std=c++20 -Wfatal-errors -Wall -Wextra -Werror -O0 -gdwarf

SRC_FILES := robot_config.cpp motors_positions_generator.cpp robot_position_generator.cpp optimus.cpp robot_pybind.cpp
OBJS := $(addprefix build/shared/,$(SRC_FILES:.cpp=.o))
STATIC_OBJS := $(addprefix build/static/,$(SRC_FILES:.cpp=.o))

HEADER_FILES := robot_config.h frequency_timer.h atomic_consumer_producer.h optimus.h motors_positions_generator.h robot_types.h robot_position_generator.h
ROBOT_LIB_NAME := robot_brain$(PYTHON3_LIB_EXT)
ROBOT_LIB := build/shared/$(ROBOT_LIB_NAME)

SRC_FILES := optimus.cpp robot_config.cpp motors_positions_generator.cpp robot_position_generator.cpp

# ./robot_pybind.cpp
#all: frequency_test robot_config_test

.PHONY: optimus
optimus: $(ROBOT_LIB)

# $(ROBOT_LIB): $(SRC_FILES) $(HEADER_FILES)
# 	g++ $(CXX_FLAGS) -shared $(PYBIND11_INCLUDES) -I./yaml-cpp/include/ $(SRC_FILES) -o $@ -lstdc++ -lyaml-cpp -L./build/yaml-cpp/

$(ROBOT_LIB): $(OBJS) yaml-cpp
	$(CXX_COMPILER) $(CXX_FLAGS) -shared -o $@ $(OBJS) -lstdc++ -lyaml-cpp -L./build/yaml-cpp/

build/shared:
	mkdir -p build/shared

build/static:
	mkdir -p build/static

build/tests:
	mkdir -p build/tests

.PHONY: yaml-cpp
yaml-cpp: build/yaml-cpp/libyaml-cpp.a

build/yaml-cpp/libyaml-cpp.a:
	cmake -S submodules/yaml-cpp/ -B build/yaml-cpp
	cmake --build build/yaml-cpp/ --target yaml-cpp

# TODO: build dependency files for each cpp
build/shared/%.o: %.cpp | build/shared
	$(CXX_COMPILER) $(CXX_FLAGS) $(CXX_INCLUDES) -c $< -o $@ -L./build/yaml-cpp/ -lyaml-cpp

build/static/%.o: %.cpp | build/static
	$(CXX_COMPILER) $(CXX_FLAGS_STATIC) $(CXX_INCLUDES) -c $< -o $@ -L./build/yaml-cpp/ -lyaml-cpp -lstdc++


# static lib
.PHONY: optimus_static
optimus_static: build/static/liboptimus.a

build/static/liboptimus.a: $(STATIC_OBJS) yaml-cpp
	ar rvs $@ $(STATIC_OBJS)

.PHONY: optimus_test
optimus_test: build/tests/optimus_test

# working shared library linkage
# run with
# $ (LD_LIBRARY_PATH=./build/shared/ ./build/tests/optimus_test)
# build/tests/optimus_test: tests/optimus_test.cpp $(ROBOT_LIB)
# 	$(CXX_COMPILER) $(CXX_FLAGS) $(CXX_INCLUDES) $< -o $@ -lgtest -l:$(ROBOT_LIB_NAME) -L./build/shared

# working
build/tests/optimus_test: build/static/liboptimus.a tests/optimus_test.cpp | build/tests
	$(CXX_COMPILER) $(CXX_FLAGS_STATIC) $(CXX_INCLUDES) tests/optimus_test.cpp -o $@ -lstdc++ -lgtest -loptimus -lyaml-cpp -L./build/yaml-cpp/ -L./build/static

frequency_test:
	gcc -Wfatal-errors -Wall -Wextra -Werror -std=c++20 -gdwarf -I./ tests/frequency_test.cpp -lstdc++ -lgtest -o build/tests/frequency_test

.PHONY: config_test
config_test: build/tests/config_test

# build/tests/config_test:
# 	gcc -Wfatal-errors -Wall -Wextra -Werror -std=c++20 -gdwarf -I./ -I./yaml-cpp/include/ robot_config.cpp tests/config_test.cpp -o build/tests/config_test -lstdc++ -lgtest -loptimus
build/tests/config_test: tests/config_test.cpp build/static/liboptimus.a
	$(CXX_COMPILER) $(CXX_FLAGS_STATIC) $(CXX_INCLUDES) $< -o $@ -lstdc++ -lgtest -lyaml-cpp -loptimus -L./build/yaml-cpp/  -L./build/static

atomic_test:
	gcc -Wfatal-errors -Wall -Wextra -Werror -std=c++20 -gdwarf -I./ -I./yaml-cpp/include/ tests/atomic_test.cpp -lstdc++ -lgtest -lyaml-cpp -L./build/yaml-cpp/ -o build/tests/atomic_test

.PHONY: clean
clean:
	rm -f build/*.o build/tests/* build/static/* build/shared/*
	rm -rf build/yaml-cpp
