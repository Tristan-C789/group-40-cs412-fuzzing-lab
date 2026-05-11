# CS-412 Fuzzing Lab — libpng 1.6.18 / CVE-2015-8126

This is a summary 

## Setup

Build the image:

```bash
docker build -t png-fuzz .
```

Start a container:

```bash
docker run -it -name png-fuzz-container png-fuzz
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

Run the specific Sanitizer-free and persistent mode campains:

```bash
make fuzz-nosan
make fuzz-persistent
```

Generate plots for each campaign:

```bash
make plot
```

## Getting results out

In a separate regular terminal, run:

```bash
docker cp png-fuzz-container:/fuzz/findings ./findings
docker cp png-fuzz-container:/fuzz/findings-qemu ./findings-qemu
docker cp png-fuzz-container:/fuzz/plot_output ./plot_output
docker cp png-fuzz-container:/fuzz/plot_output_qemu ./plot_output_qemu
```
