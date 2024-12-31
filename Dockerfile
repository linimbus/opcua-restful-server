FROM --platform=amd64 ubuntu:24.04 as builder

ARG PROXY=

ENV HTTP_PROXY=$PROXY
ENV HTTPS_PROXY=$PROXY
ENV http_proxy=$PROXY
ENV https_proxy=$PROXY

RUN echo "Proxy value is $HTTP_PROXY"

WORKDIR /workspace

RUN apt-get update
RUN apt-get install -y build-essential python3 cmake linux-libc-dev \
    tar curl zip unzip gcc g++ git clang-format pkg-config automake \
    autoconf libtool linux-tools-generic

RUN git clone https://github.com/microsoft/vcpkg

ENV VCPKG_ROOT=/workspace/vcpkg
ENV PATH=${VCPKG_ROOT}:${PATH}

WORKDIR /workspace/vcpkg
RUN ./bootstrap-vcpkg.sh

COPY . /workspace/opcua
WORKDIR /workspace/opcua

RUN cmake --preset=release
RUN cmake --build build --parallel 4
RUN cmake --install build

FROM --platform=amd64 ubuntu:24.04 as publish

COPY --from=builder /workspace/opcua/publish/bin/opcua-restful-server /usr/bin/opcua-restful-server

EXPOSE 3344

CMD [ "/usr/bin/opcua-restful-server" ]
