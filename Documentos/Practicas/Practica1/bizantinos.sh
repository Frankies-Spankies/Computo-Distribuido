NODOS=$1
PARAMETROS=$2
cat ${PARAMETROS} | xargs -p -L1 mpirun --allow-run-as-root --oversubscribe -np ${NODOS} ./a.out