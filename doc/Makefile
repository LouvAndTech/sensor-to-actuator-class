LATEX = pdflatex
SRC_DIR = src
BUILD_DIR = build
DIST_DIR = dist

TEXFILE = $(SRC_DIR)/main.tex
OUTPUT = $(DIST_DIR)/main.pdf

all: $(OUTPUT)

$(OUTPUT): $(TEXFILE)
	mkdir -p $(BUILD_DIR) $(DIST_DIR)
	cp -r $(SRC_DIR)/img $(BUILD_DIR)/img
	$(LATEX) -output-directory=$(BUILD_DIR) $(TEXFILE)
	$(LATEX) -output-directory=$(BUILD_DIR) $(TEXFILE) # Run twice for proper references
	cp $(BUILD_DIR)/main.pdf $(OUTPUT)

clean:
	rm -rf $(BUILD_DIR)/* $(DIST_DIR)/*

.PHONY: all clean