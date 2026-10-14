# syntax=docker/dockerfile:1
#
# Stages:
#   base      - Debian + Phoenix 6 runtime libraries
#   toolchain - base + compiler, CMake and development headers
#   build     - compiles a Release binary and runs the unit tests
#   runtime   - minimal image that runs the robot program (deploy this to the Pi)
#   dev       - toolchain + clangd/gdb/git, used by the dev container (default target)
#
#   docker build -t kestrel-dev .                       # dev image
#   docker build -t kestrel --target runtime .          # production image

ARG DEBIAN_RELEASE=bookworm

FROM debian:${DEBIAN_RELEASE} AS base

ARG CTR_YEAR=2026

RUN apt-get update && apt-get install -y --no-install-recommends \
        ca-certificates curl \
    && curl -fsSL --compressed -o /usr/share/keyrings/ctr-pubkey.gpg \
        "https://deb.ctr-electronics.com/ctr-pubkey.gpg" \
    && curl -fsSL --compressed -o /etc/apt/sources.list.d/ctr${CTR_YEAR}.list \
        "https://deb.ctr-electronics.com/ctr${CTR_YEAR}.list" \
    && apt-get update && apt-get install -y --no-install-recommends \
        phoenix6 libatomic1 iproute2 \
    && rm -rf /var/lib/apt/lists/*


FROM base AS toolchain

RUN apt-get update && apt-get install -y --no-install-recommends \
        build-essential cmake libgtest-dev \
    && rm -rf /var/lib/apt/lists/*


FROM toolchain AS build

WORKDIR /src
COPY . .
RUN cmake -S . -B /out -DCMAKE_BUILD_TYPE=Release \
    && cmake --build /out -j"$(nproc)" \
    && ctest --test-dir /out --output-on-failure


FROM base AS runtime

COPY --from=build /out/Kestrel /usr/local/bin/Kestrel

CMD ["Kestrel"]


FROM toolchain AS dev

# clangd must be on PATH: Zed has no prebuilt clangd download for linux/arm64
RUN apt-get update && apt-get install -y --no-install-recommends \
        clangd-19 gdb git openssh-client less \
    && ln -s /usr/bin/clangd-19 /usr/local/bin/clangd \
    && rm -rf /var/lib/apt/lists/*

CMD ["bash"]
