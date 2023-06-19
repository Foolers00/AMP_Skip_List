NAME = main

CC ?= gcc
RM ?= @rm
MKDIR ?= @mkdir

CFLAGS := -O3 -Wall -Wextra -fopenmp

SRC_DIR = src
BUILD_DIR = build
DATA_DIR = data
INCLUDES = inc

OBJECTS = $(NAME).o skip_list_seq.o skip_list_lock.o skip_list_lockf.o test.o skip_list_lockf_improved.o skip_list_lockf_pred.o


all: $(BUILD_DIR) $(NAME) $(NAME).so
	@echo "Built $(NAME)"

$(DATA_DIR):
	@echo "Creating data directory: $(DATA_DIR)"
	$(MKDIR) $(DATA_DIR)

$(BUILD_DIR):
	@echo "Creating build directory: $(BUILD_DIR)"
	$(MKDIR) $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "Compiling $<"
	$(CC) $(CFLAGS) -fPIC -I$(INCLUDES) -c -o $@ $<

$(NAME): $(foreach object,$(OBJECTS),$(BUILD_DIR)/$(object))
	@echo "Linking $(NAME)"
	$(CC) $(CFLAGS) -o $@ $^

$(NAME).so: $(foreach object,$(OBJECTS),$(BUILD_DIR)/$(object))
	@echo "Linking $(NAME)"
	$(CC) $(CFLAGS) -fPIC -shared -o $@ $^ 

bench:
	@echo "This could run a sophisticated benchmark"

small-bench: $(BUILD_DIR) $(NAME).so $(DATA_DIR)
	@echo "Running small-bench ..."
	@python3 benchmark.py

levels-plot:
	@echo "Plotting small-bench (levels) results ..."
	bash -c 'cd plots && pdflatex "\newcommand{\DATAPATH}{../data/$$(ls ../data/ | sort -r | head -n 1)}\input{seq_levels_plot.tex}" > /dev/null'
	bash -c 'cd plots && pdflatex "\newcommand{\DATAPATH}{../data/$$(ls ../data/ | sort -r | head -n 1)}\input{lock_levels_plot.tex}" > /dev/null'
	bash -c 'cd plots && pdflatex "\newcommand{\DATAPATH}{../data/$$(ls ../data/ | sort -r | head -n 1)}\input{lfree_levels_plot.tex}" > /dev/null'
	bash -c 'cd plots && pdflatex "\newcommand{\DATAPATH}{../data/$$(ls ../data/ | sort -r | head -n 1)}\input{lfree_improved_levels_plot.tex}" > /dev/null'
	bash -c 'cd plots && pdflatex "\newcommand{\DATAPATH}{../data/$$(ls ../data/ | sort -r | head -n 1)}\input{lfree_pred_levels_plot.tex}" > /dev/null'

throughput-plot:
	@echo "Plotting small-bench (throughput) results ..."
	bash -c 'cd plots && pdflatex "\newcommand{\DATAPATH}{../data/$$(ls ../data/ | sort -r | head -n 1)}\input{throughput_plot.tex}" > /dev/null'

throughput-percent-plot:
	@echo "Plotting small-bench (throughput percent) results ..."
	bash -c 'cd plots && pdflatex "\newcommand{\DATAPATH}{../data/$$(ls ../data/ | sort -r | head -n 1)}\input{throughput_percent_plot.tex}" > /dev/null'

small-plot: throughput-plot throughput-percent-plot levels-plot
	@echo "============================================"
	@echo "Created plots/throughput_plot.pdf"
	@echo "Created plots/throughput_percent_plot.pdf"
	@echo "Created plots/seq_levels_plot.pdf"
	@echo "Created plots/lock_levels_plot.pdf"
	@echo "Created plots/lfree_levels_plot.pdf"

report: small-plot
	@echo "Compiling report ..."
	bash -c 'cd report; pdflatex report.tex; biblatex report; pdflatex report.tex; pdflatex report.tex'
	@echo "============================================"
	@echo "Done"

zip:
	@zip Fromherz_John_Hofmann_amp_project5.zip benchmark.py Makefile README src/* plots/*.tex report/report.pdf report/report.tex report/plots_for_report/*

clean:
	@echo "Cleaning build directory: $(BUILD_DIR) and binaries: $(NAME) $(NAME).so"
	$(RM) -Rf $(BUILD_DIR)
	$(RM) -f $(NAME) $(NAME).so

.PHONY: clean report
