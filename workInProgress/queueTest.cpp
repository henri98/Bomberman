#include <QueueLib.h>

int main() {
	int i;
	Queue *pQ = ConstructQueue(7);
	NODE *pN;

	for (i = 0; i < 9; i++) {
		pN = (NODE*)malloc(sizeof(NODE));
		pN->data.info = 100 + i;
		Enqueue(pQ, pN);
	}

	while (!isEmpty(pQ)) {
		pN = Dequeue(pQ);
		printf("\nDequeued: %d", pN->data);
		free(pN);
	}
	DestructQueue(pQ);
	return (EXIT_SUCCESS);
}