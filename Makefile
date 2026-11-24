#BIN_DIR := /usr/bin
#HEAD_DIR := /usr/include/kyle

BUILD_DIR := build
INC_DIR := $(BUILD_DIR)/include

all: darray llist stack queue iniread memleak arena tree

darray:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(INC_DIR)
	@cd ./$@ && make lib -B
	@cp ./$@/*.* $(BUILD_DIR)/.
	@cp ./$@/include/*.h $(INC_DIR)/.
	@printf "\e[32m"
	@echo $@ Built
	@printf "\e[39m"

llist:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(INC_DIR)
	@cd ./$@ && make lib -B
	@cp ./$@/*.* $(BUILD_DIR)/.
	@cp ./$@/include/*.h $(INC_DIR)/.
	@printf "\e[32m"
	@echo $@ Built
	@printf "\e[39m"

iniread:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(INC_DIR)
	@cd ./$@ && make lib -B
	@cp ./$@/*.* $(BUILD_DIR)/.
	@cp ./$@/include/*.h $(INC_DIR)/.
	@printf "\e[32m"
	@echo $@ Built
	@printf "\e[39m"

stack:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(INC_DIR)
	@cd ./$@ && make lib -B
	@cp ./$@/*.* $(BUILD_DIR)/.
	@cp ./$@/include/*.h $(INC_DIR)/.
	@printf "\e[32m"
	@echo $@ Built
	@printf "\e[39m"

queue:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(INC_DIR)
	@cd ./$@ && make lib -B
	@cp ./$@/*.* $(BUILD_DIR)/.
	@cp ./$@/include/*.h $(INC_DIR)/.
	@printf "\e[32m"
	@echo $@ Built
	@printf "\e[39m"

memleak:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(INC_DIR)
	@cd ./$@ && make lib -B
	@cp ./$@/*.* $(BUILD_DIR)/.
	@cp ./$@/include/*.h $(INC_DIR)/.
	@printf "\e[32m"
	@echo $@ Built
	@printf "\e[39m"

arena:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(INC_DIR)
	@cd ./$@ && make lib -B
	@cp ./$@/*.* $(BUILD_DIR)/.
	@cp ./$@/include/*.h $(INC_DIR)/.
	@printf "\e[32m"
	@echo $@ Built
	@printf "\e[39m"

tree:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(INC_DIR)
	@cd ./$@ && make lib -B
	@cp ./$@/*.* $(BUILD_DIR)/.
	@cp ./$@/include/*.h $(INC_DIR)/.
	@printf "\e[32m"
	@echo $@ Built
	@printf "\e[39m"

install:
ifeq ($(OS), Windows_NT)
	cp $(BUILD_DIR)/*.dll $(BIN_DIR)
else
	cp $(BUILD_DIR)/*.so $(BIN_DIR)
endif
	cp $(INC_DIR)/*.h $(HEAD_DIR)


clean:
	rm -R -f $(BUILD_DIR)

.PHONY: clean darray llist stack queue iniread memleak arena install tree
