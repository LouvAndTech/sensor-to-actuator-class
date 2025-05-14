LATEX = pdflatex
PLANTUML = java -jar ../.devcontainer/Latex/plantuml.jar

SRC_DIR = src
BUILD_DIR = build
DIST_DIR = dist
DIAGRAM_DIR = diagram

TEXFILE = $(SRC_DIR)/main.tex
OUTPUT = $(DIST_DIR)/main.pdf
PLANTUML_SRC = $(wildcard $(SRC_DIR)/$(DIAGRAM_DIR)/*.puml)
PLANTUML_OUTPUT = $(patsubst $(SRC_DIR)/$(DIAGRAM_DIR)/%.puml, $(BUILD_DIR)/$(DIAGRAM_DIR)/%.png, $(PLANTUML_SRC))


all: $(OUTPUT)

# Generate PDF from LaTeX
$(OUTPUT): $(PLANTUML_OUTPUT) $(TEXFILE)
	@echo "Generating PDF: $@"
	mkdir -p $(BUILD_DIR) $(DIST_DIR)
	cp -r $(SRC_DIR)/img $(BUILD_DIR)/img
	$(LATEX) -output-directory=$(BUILD_DIR) $(TEXFILE)
	$(LATEX) -output-directory=$(BUILD_DIR) $(TEXFILE) # Run twice for proper references
	cp $(BUILD_DIR)/main.pdf $(OUTPUT)
	@echo "Generated PDF: $@"

# PlantUML diagrams
$(PLANTUML_OUTPUT): $(PLANTUML_SRC)
	@echo "Generating diagram: $<"
	mkdir -p $(BUILD_DIR)/$(DIAGRAM_DIR)
	$(PLANTUML) -tpng -o ../../$(BUILD_DIR)/$(DIAGRAM_DIR) $<
	@echo "Generated diagram: $@"

clean:
	rm -rf $(BUILD_DIR)/* $(DIST_DIR)/*

.PHONY: all clean