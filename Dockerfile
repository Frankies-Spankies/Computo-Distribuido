FROM gcc
RUN apt-get update && apt-get install openmpi-bin libopenmpi-dev -y; exit 0
RUN mkdir Documentos
WORKDIR /root
RUN echo alias mpirun=\'mpirun --allow-run-as-root\' >> .bashrc
CMD  "/bin/bash"