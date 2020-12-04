NODOS=$1
PARAMETROS=$2
cat ${PARAMETROS} | xargs -L1 mpirun --allow-run-as-root --oversubscribe -np ${NODOS} ./a.out