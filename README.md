# XSquare

`XSquare` is a 2D side-scrolling platform game development playground. It is written in `C++` and relies on `X11`, `X11's Back-buffer Extension`, and `libpng`. Rather than offering a completed game, `XSquare` explores some basic game development concepts, e.g., separating out interface-, engine-, video-, audio-, level-, and networking; level design; using sprites; and other things.

`XSquare` will run on `Linux` and `MacOS`, where the former requires `OSS` support for sound, and the latter the [XQuartz](https://www.xquartz.org) X11 window manager installed.

![XSquare in Action](XSquare.png "XSquare in Action")

## Usage

`XSquare` program is compiled with:

```shell
make
```

This results in a binary executable called `xsquare`, which is invoked with:

```shell
./xsquare
```

## Notes

1. On `MacOS` `XCode` and the developer tools needs to be installed.
2. On `Ubuntu` the `osspd-alsa` package is needed for sound.
3. Both `Linux` and `MacOS` require `libpng`.

## BSD-3 License

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
