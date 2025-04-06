FROM rabits/qt:5.15-desktop

WORKDIR /app
COPY . /app

USER root
RUN qmake && make
RUN apt-get update && apt-get install -y libqt5sql5-psql

ENV QT_QPA_PLATFORM=offscreen

CMD ["./server"]
