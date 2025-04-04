FROM rabits/qt:5.15-desktop

# Устанавливаем рабочую директорию
WORKDIR /app
# Копируем исходный код приложения внутрь контейнера
COPY . /app
USER root
# Собираем приложение
RUN qmake && make
RUN apt-get update && apt-get install -y libqt5sql5-psql
ENV QT_QPA_PLATFORM=offscreen

# Указываем команду по умолчанию при запуске контейнера
CMD ["./server"]
