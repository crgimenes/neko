# Neko

![noko](https://raw.githubusercontent.com/crgimenes/neko/master/assets/awake.png)

[Neko](https://en.wikipedia.org/wiki/Neko_(software)) is a cat that chases the mouse cursor across the screen, an app written in the late 1980s and ported for many platforms.

![Neco](https://github.com/crgimenes/neko/blob/master/fixtures/neko.gif)

This code is a re-implementation using Golang and **has no relationship to the original software**. Furthermore, this version does not use any part of the original source code except sprites and sounds.

The goal is to demonstrate the Go language with a fun and very nostalgic example, without necessarily being tied to the behavior of the original version.

In this example, we used the [Ebitengine](https://ebitengine.org), an incredibly easy-to-use gaming library with a vibrant community.

## How to run

Note that some operating systems may restrict the execution of binaries downloaded from the internet for security reasons. Please consult your operating system documentation to learn how to enable Neko to run.

## Parameters

- `-mousepassthrough` Enable mouse passthrough (default false).
- `-quiet` Disable sound.
- `-scale` The scale of the cat on the screen (default 2.0).
- `-speed` The speed of the cat (default 2).
- `-h` Show help.


## How to Contribute


Please follow our [contribution guide](CONTRIBUTING.md).
