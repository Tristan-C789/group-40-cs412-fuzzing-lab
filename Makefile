LIBPNG_INSTR_PATH := /fuzz/install
LIBPNG_VANIL_PATH := /fuzz/install_vanilla
DICTIONARY := /AFLplusplus/dictionaries/png.dict
SEEDS := /fuzz/seeds

.PHONY: build build-instrumented build-nosan build-persistent build-qemu \
        fuzz fuzz-qemu fuzz-nosan fuzz-persistent \
        plot plot-instrumented plot-qemu \
        clean

# -- Harness builds --

build: build-instrumented build-qemu build-nosan build-persistent

build-instrumented:
	afl-clang-fast src/harness.c \
		-I$(LIBPNG_INSTR_PATH)/include -L$(LIBPNG_INSTR_PATH)/lib \
		-lpng16 -lz -lm \
		-fsanitize=address -g -O1 \
		-o png_fuzzer_instrumented

build-qemu:
	gcc src/harness.c \
		-I$(LIBPNG_VANIL_PATH)/include -L$(LIBPNG_VANIL_PATH)/lib \
		-lpng16 -lz -lm \
		-g -O1 \
		-o png_fuzzer_qemu

build-nosan:
	afl-clang-fast src/harness.c \
		-I$(LIBPNG_VANIL_PATH)/include -L$(LIBPNG_VANIL_PATH)/lib \
		-lpng16 -lz -lm \
		-g -O1 \
		-o png_fuzzer_nosan

build-persistent:
	afl-clang-fast src/harness_persistent.c \
		-I$(LIBPNG_INSTR_PATH)/include -L$(LIBPNG_INSTR_PATH)/lib \
		-lpng16 -lz -lm \
		-fsanitize=address -g -O1 \
		-o png_fuzzer_persistent


# -- Fuzzing campains --

fuzz:
	AFL_AUTORESUME=1 \
	afl-fuzz \
		-i $(SEEDS) \
		-o findings \
		-x $(DICTIONARY) \
		-- ./png_fuzzer_instrumented @@

fuzz-qemu:
	AFL_AUTORESUME=1 \
	afl-fuzz -Q \
		-i $(SEEDS) \
		-o findings-qemu \
		-x $(DICTIONARY) \
		-- ./png_fuzzer_qemu @@

fuzz-nosan:
	AFL_AUTORESUME=1 \
	afl-fuzz \
		-i $(SEEDS) \
		-o findings-nosan \
		-x $(DICTIONARY) \
		-- ./png_fuzzer_nosan @@

fuzz-persistent:
	AFL_AUTORESUME=1 \
	afl-fuzz \
		-i $(SEEDS) \
		-o findings-persistent \
		-x $(DICTIONARY) \
		-- ./png_fuzzer_persistent @@


# -- Plotting --

plot: plot-instrumented plot-qemu

plot-instrumented: 
	afl-plot findings/default plot_output

plot-qemu:
	afl-plot findings-qemu/default plot_output_qemu


# -- Clean --

clean:
	rm -f png_fuzzer_instrumented png_fuzzer_qemu png_fuzzer_nosan png_fuzzer_persistent 