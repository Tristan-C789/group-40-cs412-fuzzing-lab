# CS-412 Fuzzing Lab - libpng 1.6.18 & CVE-2015-8126

The associated GitHub repository can be found at: https://github.com/Tristan-C789/group-40-cs412-fuzzing-lab.git

> [!WARNING]
> **Windows is not supported for cloning this repository.**
> AFL++ findings artifacts use filenames containing colons, which are illegal on Windows. Cloning on Windows will succeed but checkout will fail. Use **WSL, Linux** or **macOS** instead. 
> Since the handout is unclear about what should be included, we chose to still keepthe findings folders in the repository. Nonetheless, they can be easily recomputed using the commands below.

This is a summary of the setup and run commands.

## Setup

Build the image:

```bash
docker build -t png-fuzz .
```

Start a container:

```bash
docker run -it --name png-fuzz-container png-fuzz
```

## Inside the container

Compile all the harnesses:

```bash
make build
```

Run the instrumented campaign:

```bash
make fuzz
```

Run the QEMU campaign:

```bash
make fuzz-qemu
```

Run the buggy campaign:

```bash
make fuzz-bug
```

Run the specific Sanitizer-free and persistent mode campains:

```bash
make fuzz-nosan
make fuzz-persistent
```

Generate plots for each campaign:

```bash
make plot
```

Clean up compiled harnesses:

```bash
make clean
```

## Getting results out

In a separate regular terminal, run:

```bash
docker cp png-fuzz-container:/fuzz/findings ./findings
docker cp png-fuzz-container:/fuzz/findings-qemu ./findings-qemu
docker cp png-fuzz-container:/fuzz/findings-bug ./findings-bug
docker cp png-fuzz-container:/fuzz/findings-nosan ./findings-nosan
docker cp png-fuzz-container:/fuzz/findings-persistent ./findings-persistent
docker cp png-fuzz-container:/fuzz/plot_output ./plot_output
docker cp png-fuzz-container:/fuzz/plot_output_qemu ./plot_output_qemu
```
