#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>


//#define EVAL

#define OUTPUT(msg) \
	fprintf(stdout, (msg))


static int n_station = 0;
#define QUEUE_SIZE  n_station

#define HASH_TABLE_SIZE  0x10000
static inline int get_hash_index(int distance) {
//	return distance & (HASH_TABLE_SIZE - 1);
return distance % HASH_TABLE_SIZE;
}

typedef struct car1_s {
	int               cars[512];
	int               free;
} car1_t;

typedef struct car_s {
	int           autonomy;
	int           counter;
	struct car_s *next;
} car_t;

/*
 * Station
 */
typedef struct station_s {
	int               distance;
	struct station_s *next;
	car1_t            car1;
} station_t;


/*
 * Game state machine
 */
typedef enum {
	GAME_ADD_STATION,
	GAME_REMOVE_STATION,
	GAME_ADD_CAR,
	GAME_REMOVE_CAR,
	GAME_PLAN_TRIP
} game_state_t;


#define CMD_ADD_STATION       "aggiungi-stazione"
#define CMD_REMOVE_STATION    "demolisci-stazione"
#define CMD_ADD_CAR           "aggiungi-auto"
#define CMD_REMOVE_CAR        "rottama-auto"
#define CMD_PLAN_TRIP         "pianifica-percorso"

static game_state_t decode_command(char *cmd)
{
	if (strncmp(cmd, CMD_ADD_STATION, sizeof(CMD_ADD_STATION) - 1) == 0) {
		return GAME_ADD_STATION;
	} else if (strncmp(cmd, CMD_REMOVE_STATION, sizeof(CMD_REMOVE_STATION) - 1) == 0) {
		return GAME_REMOVE_STATION;
	} else if (strncmp(cmd, CMD_ADD_CAR, sizeof(CMD_ADD_CAR) - 1) == 0) {
		return GAME_ADD_CAR;
	} else if (strncmp(cmd, CMD_REMOVE_CAR, sizeof(CMD_REMOVE_CAR) - 1) == 0) {
		return GAME_REMOVE_CAR;
	} else if (strncmp(cmd, CMD_PLAN_TRIP, sizeof(CMD_PLAN_TRIP) - 1) == 0) {
		return GAME_PLAN_TRIP;
	} else {
		printf("ERROR: wrong command (%s)\n", cmd);
		exit(-2);
	}
}

static inline void consume_input(void)
{
	while (getchar() != '\n') {}
}

static int read_token(char *token)
{
	int i;
	char ch;
	i = 0;
	ch = getchar();
	if (ch == EOF) {
		return EOF;
	}

	token[i++] = ch;
	while (1) {
		ch = getchar();
		if ((ch == ' ') || (ch == '\n')) {
			break;
		}
		token[i++] = ch;
	}
	token[i] = '\0';

	return i;
}

static void add_car_helper(station_t *station)
{
	char token[32];
	int autonomy;
	int i;

	read_token(token);
	autonomy = atoi(token);

	i = station->car1.free;
	while ((i >= 0) && (station->car1.cars[i] < autonomy)) {
	station->car1.cars[i+1] = station->car1.cars[i];
		i--;
	}
	station->car1.cars[i+1] = autonomy;
	station->car1.free++;
}

void add_station(station_t **stations)
{
	char token[32];
	station_t *station;
	int cars;
	int i;
	int distance;
	int idx;

	station_t *s;
	station_t *p;

	read_token(token);
	distance = atoi(token);

	idx = get_hash_index(distance);

	station = stations[idx];

    p = s = station;

    while (s != NULL) {
        if (s->distance >= distance) {
            break;
        }

        p = s;
        s = s->next;
    }

    if ((s != NULL) && (s->distance == distance)) {
        /* Consume data till the end of the line */
        consume_input();
        /* Station has not been added */
        OUTPUT("non aggiunta\n");
        return;
    }

    station = malloc(sizeof(station_t));
    if (station == NULL) {
        fprintf(stderr, "ERROR: out of memory (%s: %d)\n", __func__, __LINE__);
        exit(-1);
    }

    station->next = NULL;
    station->car1.cars[0] = -1;
    station->car1.free = 0;
    station->distance = distance;

    read_token(token);
    cars = atoi(token);

    for (i = 0; i < cars; i++) {
        add_car_helper(station);
    }

    if (p == s) {
        /* first element of the list */
        station->next = s;
        stations[idx] = station;
    } else {
        station->next = s;
        p->next = station;
    }

	/* Station has been added */
	n_station++;
	OUTPUT("aggiunta\n");
}

void remove_station(station_t **stations)
{
	char token[32];
	station_t *s;
	station_t *p;
	int distance;
	int idx;

	read_token(token);
	distance = atoi(token);

	idx = get_hash_index(distance);

	p = s = stations[idx];

	while (s != NULL) {

		if (s->distance == distance) {
			break;
		}
		p = s;
		s = s->next;
	}

	if (s == NULL) {
		OUTPUT("non demolita\n");
	} else {
		if (p == s) {
			stations[idx] = s->next;
		} else {
			p->next = s->next;
		}
		s->car1.cars[0] = -1;
		s->car1.free = 0;
		free(s);
		n_station--;
		OUTPUT("demolita\n");
	}
}

void add_car(station_t **stations)
{
	char token[32];
	station_t *s;
	int distance;
	int idx;

	read_token(token);
	distance = atoi(token);

	idx = get_hash_index(distance);
	s = stations[idx];

	while (s != NULL) {

		if (s->distance == distance) {
			add_car_helper(s);
			/* Car has been added */
			OUTPUT("aggiunta\n");
			return;
		}
		s = s->next;
	}

	/* Consume data till the end of the line */
	consume_input();

	OUTPUT("non aggiunta\n");
}

int binary_search(int* cars, int autonomy, int in, int fi) {
	int m = (in+fi)/2;
	if ((cars[in] < autonomy) || (cars[fi] > autonomy) || (fi < in)) {
		return -1;
	}
	if (cars[m] > autonomy) {
		return binary_search(cars, autonomy, m+1, fi);
	}
	else if (cars[m] < autonomy) {
		return binary_search(cars, autonomy, in, m-1);
	}
	else {
		return m;
	}
}

void remove_car(station_t **stations)
{
	char token[32];
	station_t *s;
	int distance;
	int autonomy;
	int idx;

	read_token(token);
	distance = atoi(token);

	idx = get_hash_index(distance);
	s = stations[idx];

	while (s != NULL) {

		if (s->distance == distance) {

			read_token(token);
			autonomy = atoi(token);

			int i = binary_search(s->car1.cars, autonomy, 0, s->car1.free-1);
			if (i == -1) {
				OUTPUT("non rottamata\n");
				return;
			}

			while (i < s->car1.free) {
				s->car1.cars[i] = s->car1.cars[i+1];
				i++;
			}
			s->car1.free--;
			/* Car has been removed */
			OUTPUT("rottamata\n");
			return;
		}
		s = s->next;
	}

	/* Consume data till the end of the line */
	consume_input();

	OUTPUT("non rottamata\n");
}


void print_path(station_t** stations, int idx, int *path, int* prec, int end)
{
	int tail = 0;

	path[tail++] = stations[idx]->distance;
	int i = idx;
	do {
		path[tail++] = stations[i]->distance;
		i = prec[i];
	} while (stations[i]->distance != stations[end]->distance);


	// printing path from source to destination
	fprintf(stdout, "%d", stations[i]->distance);
	for (int i = tail - 1; i > 0; i--) {
		fprintf(stdout, " %d", path[i]);
	}
	fprintf(stdout, "\n");
}


typedef struct queue_s {
	int *queue;
	int first;
	int last;
} queue_t;


void init(queue_t *q)
{
	q->queue = (int *)malloc(sizeof(int) * QUEUE_SIZE);
	if (q->queue == NULL) {
		fprintf(stderr, "ERROR: out of memory (%s: %d)\n", __func__, __LINE__);
		exit(-1);
	}

	q->first = 0;
	q->last = 0;
}

void put(queue_t *q, int value)
{
	int i;

	if (q->last == q->first) {
		q->queue[q->last++] = value;
		return;
	}

	i = q->last - 1;
	while ((i >= q->first) && (q->queue[i] > value)) {
		q->queue[i+1] = q->queue[i];
		i--;
	}
	q->queue[i+1] = value;
	q->last++;
}

int get(queue_t *q)
{
	if (q->last == q->first) {
		return -1;
	}

	return q->queue[q->first++];
}

void reset(queue_t *q)
{
	q->first = 0;
	q->last = 0;
}

int isempty(queue_t *q)
{
	return (q->first == q->last);
}

void deinit(queue_t *q)
{
	free(q->queue);
}

void BFS_GREEDY_backward(station_t **path, int first, int end, int* predecessor, unsigned char* visited)
{
	int distance;
	station_t *d;
	int p;
	int idx;
	int level;
	queue_t *queue[2];
	queue_t q1;
	queue_t q2;
	queue_t *q;

    init(&q1);
	init(&q2);

	queue[0] = &q1;
	queue[1] = &q2;

    // Mark the current node as visited and enqueue it
	put(queue[0], first);
    level = 0;
	q = queue[level];

    while (1) {

    	/* Dequeue a station from queue */
    	if (isempty(q)) {
    		reset(q);
        level ^= 1;
    	}
    	q = queue[level];
    	p = get(q);

    	if (p == -1) {
    		break;
    	}

		if (p == 0) {
	        print_path(path, 0, q1.queue, predecessor, first);
	        deinit(&q1);
	        deinit(&q2);
			return;
		}

		int autonomy = path[p]->car1.cars[0];

    	if (autonomy != -1) {

				distance = path[p]->distance - autonomy;
				idx = p;
				d = path[--idx];
	    		while ((d != NULL) && (d->distance >= distance) && (d->distance >= end)) {

	    			if (visited[idx] == 0) {
	    				predecessor[idx] = p;
	    				visited[idx] = 1;
	    				put(queue[level ^ 1], idx); // insert in ordered mode
	    			}
					if (idx == 0) {
						break;
					}
					d = path[--idx];
	    	}
    	}
    }

    OUTPUT("nessun percorso\n");

    deinit(&q1);
    deinit(&q2);
}

void BFS_forward(station_t **path, int first, int end, int* predecessor, unsigned char* visited)
{
	int distance;
	station_t *d;
	int idx;
	int p;
	int *queue;
	int head = 0, tail = 0;

	queue = (int *)malloc(sizeof(station_t *) * QUEUE_SIZE);
	if (queue == NULL) {
		fprintf(stderr, "ERROR: out of memory (%s: %d)\n", __func__, __LINE__);
		exit(-1);
	}

	// Mark the current node as visited and enqueue it
	queue[tail++] = first;

	while (head != tail) {

		/* Dequeue a station from queue */
		p = queue[head++];
		if (head == QUEUE_SIZE) {
			head = 0;
		}

		int autonomy = path[p]->car1.cars[0];

    if (autonomy != -1) {

			distance = path[p]->distance + autonomy;
			idx = p;
			d = path[++idx];
			while ((d != NULL) && (d->distance <= distance) && (d->distance <= end)) {

				if (visited[idx] == 0) {
					predecessor[idx] = p;
					visited[idx] = 1;

					if (d->distance == end) {
//						print_path(d, (int *)queue);
						print_path(path, idx, (int *)queue, predecessor, first);
						free(queue);
						return;
					}

					queue[tail++] = idx;
					if (tail == QUEUE_SIZE) {
						tail = 0;
					}
				}
				d = path[++idx];
			}
		}
	}

	OUTPUT("nessun percorso\n");

	free(queue);
}

static inline void swap(station_t **a, station_t **b)
{
	station_t *t;
	t = *a;
	*a = *b;
	*b = t;
}

int partition(station_t **station, int low, int high)
{
	int pivot;
	int i;
	int j;

	// select the rightmost element as pivot
	pivot = station[high]->distance;

	// pointer for greater element
	i = (low - 1);

	// traverse each element of the array
	// compare them with the pivot
	for (j = low; j < high; j++) {

		if (station[j]->distance <= pivot) {

			// if element smaller than pivot is found
			// swap it with the greater element pointed by i
			i++;

			// swap element at i with element at j
			swap(&station[i], &station[j]);
		}
	}

	// swap the pivot element with the greater element at i
	swap(&station[i + 1], &station[high]);

	// return the partition point
	return (i + 1);
}

void quickSort(station_t **station, int low, int high)
{
	if (low < high) {

		int pi;

		pi = partition(station, low, high);

		/* Ordering left */
		quickSort(station, low, pi - 1);

		/* Ordering right */
		quickSort(station, pi + 1, high);
	}
}

void plan_trip(station_t **stations)
{
	char token[32];
	int start;
	int end;
	int idx;
	int backward;
	station_t *s;
	station_t *n;
	unsigned char* visited;
	int* predecessor;

	read_token(token);
	start = atoi(token);

	read_token(token);
	end = atoi(token);

	station_t **path;
	int counter;

	path = (station_t **)calloc(QUEUE_SIZE, sizeof(station_t *));
	if (path == NULL) {
		fprintf(stderr, "ERROR: out of memory (%s: %d)\n", __func__, __LINE__);
		exit(-1);
	}

	backward = 0;
	if (start > end) {
		/* Going backward */
		backward = 1;
		int tmp;

		/* swap start and end */
		tmp = start;
		start = end;
		end = tmp;
	}

	if ((start >= HASH_TABLE_SIZE) || (end >= HASH_TABLE_SIZE)) {

		idx = 0;
		s = stations[0];
		counter = 0;
		n = NULL;
		while (s == NULL) {
			s = stations[++idx];
		}

		do {

			if ((s->distance >= start) && (s->distance <= end)) {
				path[counter++] = s;
			}

			n = s->next;

			while ((idx < HASH_TABLE_SIZE) && (n == NULL)) {
				n = stations[++idx];
			}

			s = n;
		} while (idx < HASH_TABLE_SIZE);


		// mergeSort(path, 0, counter - 1);
		quickSort(path, 0, counter - 1);

	} else {

		idx = get_hash_index(start);
		s = stations[idx];
		counter = 0;

		do {

			path[counter++] = s;

			n = s->next;

			while (n == NULL) {
				n = stations[++idx];
			}

			s = n;
		} while (s->distance < end);

		path[counter++] = s;
	}

	predecessor = (int *)calloc(counter, sizeof(int));
	visited = (unsigned char *)calloc(counter, sizeof(unsigned char));

	if (backward) {
		BFS_GREEDY_backward(path, counter - 1, start, predecessor, visited);
	} else {
		BFS_forward(path, 0, end, predecessor, visited);
	}

	free(predecessor);
	free(visited);

	free(path);

}


int main(int argc, char *argv[])
{
	char ch;
	char token[32];
	game_state_t cmd;
	station_t **stations;

#ifndef EVAL
    FILE *fp = freopen(argv[argc-1], "r", stdin);
	if (fp == NULL) {
		fprintf(stderr, "ERROR: cannot open input file (%s: %d)\n", __func__, __LINE__);
		exit(-1);
	}
	setvbuf(stdout, NULL, _IONBF, 0);
#endif

    stations = (station_t **)calloc(HASH_TABLE_SIZE, sizeof(station_t *));
	if (stations == NULL) {
		fprintf(stderr, "ERROR: out of memory (%s: %d)\n", __func__, __LINE__);
		exit(-1);
	}

	/* Main loop */
    for ( ; ; ) {

    	ch = read_token(token);
    	if (ch == EOF) {
    		break;
    	}

		cmd = decode_command(token);

		switch (cmd) {
		case GAME_ADD_STATION:
			add_station(stations);
			break;
		case GAME_REMOVE_STATION:
			remove_station(stations);
			break;
		case GAME_ADD_CAR:
			add_car(stations);
			break;
		case GAME_REMOVE_CAR:
			remove_car(stations);
			break;
		case GAME_PLAN_TRIP:
			plan_trip(stations);
			break;
		}
    }

    /*
     * Free allocated memory
     */
#if 0
	station_t *s = stations_start;
	while (s != NULL) {
		station_t *h = s;
		s = s->next;
		free(h);
	}
#endif

	return EXIT_SUCCESS;
}
