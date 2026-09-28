EXEC = solver

# nomi dei file lex e yacc

LEX_FILE   = lexer.ll
BISON_FILE = parser.yy

# nomi dei file C++ generati da lex e yacc

LEX_OUT_NAME   = lex.yy.cpp
BISON_OUT_NAME = parser.tab.cpp
BISON_HDR_NAME = parser.tab.hh
BISON_STACK_NAME = stack.hh

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

PARSER_DIR = $(SRC_DIR)/parser
ENGINE_DIR = $(SRC_DIR)/engine

PARSER_HDR_DIR = $(PARSER_DIR)/header
ENGINE_HDR_DIR = $(ENGINE_DIR)/header

# compilatori

CXX      = g++
FLEX     = flex
BISON    = bison

# flag di compilazione
# -I indica al compilatore dove andare a cercare i file header quando si fa #include
CXXFLAGS = -std=c++17 -O3 -I$(PARSER_HDR_DIR) -I$(ENGINE_HDR_DIR)

# path sorgenti e oggetti

LEX_SRC   = $(PARSER_DIR)/$(LEX_FILE)
BISON_SRC = $(PARSER_DIR)/$(BISON_FILE)

LEX_OUT   = $(PARSER_DIR)/$(LEX_OUT_NAME)
BISON_OUT = $(PARSER_DIR)/$(BISON_OUT_NAME)
BISON_HDR = $(PARSER_HDR_DIR)/$(BISON_HDR_NAME)
BISON_STACK = $(PARSER_DIR)/$(BISON_STACK_NAME)

SRCS_MANUAL  = $(wildcard $(PARSER_DIR)/*.cpp) $(wildcard $(ENGINE_DIR)/*.cpp)

SRCS_CLEAN   = $(filter-out $(LEX_OUT) $(BISON_OUT), $(SRCS_MANUAL))

# tutti i sorgenti (manuali + generati)
ALL_SRCS = $(SRCS_CLEAN) $(LEX_OUT) $(BISON_OUT)

OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(ALL_SRCS))

# target di default
.PHONY: all clean init

all: init $(BIN_DIR)/$(EXEC)

# creazione dell'albero delle directory per gli object e i bin
init:
	@mkdir -p $(BIN_DIR)
	@mkdir -p $(OBJ_DIR)/parser
	@mkdir -p $(OBJ_DIR)/engine
	@mkdir -p $(PARSER_HDR_DIR)
	@mkdir -p $(ENGINE_HDR_DIR)

# link dell'eseguibile finale
$(BIN_DIR)/$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# compilazione dei file .cpp
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BISON_HDR) $(LEX_OUT)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# generazione parser
# l'opzione --defines permette di spostare l'header direttamente in parser/header/
$(BISON_OUT) $(BISON_HDR): $(BISON_SRC)
	$(BISON) -d -o $(BISON_OUT) --defines=$(BISON_HDR) $<

# generazione lexer
$(LEX_OUT): $(LEX_SRC) $(BISON_HDR)
	$(FLEX) -o $@ $<

clean:
	@rm -rf $(OBJ_DIR)/* $(BIN_DIR)/*
	@rm -f $(LEX_OUT) $(BISON_OUT) $(BISON_HDR) $(BISON_STACK)
