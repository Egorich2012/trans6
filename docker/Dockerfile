FROM ubuntu:24.04

# ────────────────────────────────────────────────────────────────────────────
# 1. Системные зависимости
# ────────────────────────────────────────────────────────────────────────────
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        build-essential \
        cmake \
        git \
        openjdk-17-jre-headless \
        pkg-config \
        uuid-dev \
        ca-certificates \
        wget \
    && rm -rf /var/lib/apt/lists/*

# ────────────────────────────────────────────────────────────────────────────
# 2. ANTLR4 C++ Runtime
# ────────────────────────────────────────────────────────────────────────────
RUN git clone --depth 1 --branch 4.13.2 https://github.com/antlr/antlr4.git /tmp/antlr4 && \
    cmake -S /tmp/antlr4/runtime/Cpp -B /tmp/antlr4-build \
        -DCMAKE_BUILD_TYPE=Release \
        -DANTLR_BUILD_CPP_TESTS=OFF \
        -DANTLR_BUILD_SHARED=OFF \
        -DCMAKE_INSTALL_PREFIX=/usr/local && \
    cmake --build /tmp/antlr4-build --parallel "$(nproc)" && \
    cmake --install /tmp/antlr4-build && \
    rm -rf /tmp/antlr4 /tmp/antlr4-build

# ────────────────────────────────────────────────────────────────────────────
# 3. ANTLR4 JAR
# ────────────────────────────────────────────────────────────────────────────
RUN wget -q https://www.antlr.org/download/antlr-4.13.2-complete.jar \
        -O /opt/antlr4-4.13.2-complete.jar

ENV ANTLR_JAR_PATH=/opt/antlr4-4.13.2-complete.jar

# ────────────────────────────────────────────────────────────────────────────
WORKDIR /workspace

# копируем проект
COPY . .

# entrypoint
COPY docker/docker-entrypoint.sh /usr/local/bin/entrypoint.sh
RUN chmod +x /usr/local/bin/entrypoint.sh

ENTRYPOINT ["entrypoint.sh"]
CMD ["test"]