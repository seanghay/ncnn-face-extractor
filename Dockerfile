FROM pachyderm/opencv:2.6.0

RUN apt-get -y update && apt-get install -y -qq neovim
WORKDIR /workspace
COPY ncnn .
RUN mkdir build
WORKDIR /workspace/build
RUN cmake ..
RUN make -j4

COPY entrypoint.sh .

ENTRYPOINT ["/workspace/build/entrypoint.sh"]
