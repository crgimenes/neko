# Neko

![noko](https://raw.githubusercontent.com/crgimenes/neko/master/assets/awake.png)

[Neko](https://en.wikipedia.org/wiki/Neko_(software)) is a cat that chases the mouse cursor across the screen, an app written in the late 1980s and ported for many platforms.

![Neco](https://github.com/crgimenes/neko/blob/master/fixtures/neko.gif)

This code is a re-implementation using Golang and **has no relationship to the original software**. Furthermore, this version does not use any part of the original source code except sprites and sounds.

The goal is to demonstrate the Go language with a fun and very nostalgic example, without necessarily being tied to the behavior of the original version.

In this example, we used the [Ebitengine](https://ebitengine.org), an incredibly easy-to-use gaming library with a vibrant community.

## How to run

Note that some operating systems may restrict the execution of binaries downloaded from the internet for security reasons. Please consult your operating system documentation to learn how to enable Neko to run.

```bash
cd neko
go run ./main.go
```

Parameters

- `-mousepassthrough` Enable mouse passthrough (default false).
- `-quiet` Disable sound.
- `-scale` The scale of the cat on the screen (default 2.0).
- `-speed` The speed of the cat (default 2).
- `-h` Show help.

## How to install

Before installing Neko, make sure you have Go installed on your system, as we will be using `go install`.

Install dependencies, build, and install the project into your Go bin directory:

```bash
cd neko
go mod tidy
go install
```

To use Neko globally across your system, check if your Go bin directory is in your `$PATH` by running:

```bash
echo $PATH
```
If you don't see a Go bin directory, you'll need to add the following line to your `~/.bashrc`, `~/.zshrc`, or the equivalent shell configuration file:

```bash
export PATH=$PATH:$(go env GOPATH)/bin
```

After that, you can simply run `neko` in your terminal. If you want it to start with your system, you can add it to your `.xinitrc` or a similar startup script.

## How to Contribute

Please follow our [contribution guide](CONTRIBUTING.md).
