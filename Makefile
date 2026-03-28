CXX ?= g++
CXXFLAGS ?= -std=c++20 -O2 -Wall -Wextra -Wshadow -Wconversion

SRC := $(shell find problems -type f -name "*.cpp" ! -path "*/.*")
BIN := $(patsubst problems/%.cpp,build/bin/%,$(SRC))

.PHONY: all list index clean clean-cph

all: $(BIN)

list:
	@printf "%s\n" $(BIN)

index:
	conda run -n acm python scripts/update_problem_index.py

build/bin/%: problems/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) "$<" -o "$@"

clean:
	rm -rf build

clean-cph:
	find problems -type f \( -name '*.prob' -o -name '.*.prob' \) -delete
	find problems -type d -name '.cph' -exec rm -rf {} +
