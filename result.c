#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef char String[50];  

typedef struct {
    String flow_name;
    int time;
    int priority;
} TrafficData;

#define MAX_SIZE 10

typedef struct {
    TrafficData data[MAX_SIZE];
    int size;
} PriorityQueue;

int comparePriority(TrafficData *a, TrafficData *b) {
    int priority = 0;

    if (strstr(a->flow_name, "Main Lane") && strstr(b->flow_name, "Diversion Lane")) {
        priority = -1;
    } else if (strstr(a->flow_name, "Diversion Lane") && strstr(b->flow_name, "Main Lane")) {
        priority = 1;
    }

    if (priority == 0) {
        if (strstr(a->flow_name, "Left Turn from Main Lane") && strstr(b->flow_name, "Left Turn from Diversion Lane")) {
            priority = -1;
        } else if (strstr(a->flow_name, "Left Turn from Diversion Lane") && strstr(b->flow_name, "Left Turn from Main Lane")) {
            priority = 1;
        }
    }

    if (priority == 0) {
        if (strstr(a->flow_name, "Right Turn from Main Lane") && strstr(b->flow_name, "Right Turn from Diversion Lane")) {
            priority = -1;
        } else if (strstr(a->flow_name, "Right Turn from Diversion Lane") && strstr(b->flow_name, "Right Turn from Main Lane")) {
            priority = 1;
        }
    }

    if (priority == 0) {
        if (strstr(a->flow_name, "Pedestrian") && strstr(b->flow_name, "Pedestrian")) {
            if (strstr(a->flow_name, "Main Lane") && !strstr(b->flow_name, "Main Lane")) {
                priority = -1;
            } else if (strstr(b->flow_name, "Main Lane") && !strstr(a->flow_name, "Main Lane")) {
                priority = 1;
            }
        }
    }

    return priority;
}

void initQueue(PriorityQueue *pq) {
    pq->size = 0;
}

void push(PriorityQueue *pq, TrafficData data) {
    if (pq->size < MAX_SIZE) {
        pq->data[pq->size] = data;
        pq->size++;

        int index = pq->size - 1;
        while (index > 0 && comparePriority(&pq->data[index], &pq->data[(index - 1) / 2]) < 0) {
            TrafficData temp = pq->data[index];
            pq->data[index] = pq->data[(index - 1) / 2];
            pq->data[(index - 1) / 2] = temp;
            index = (index - 1) / 2;
        }
    }
}

TrafficData pop(PriorityQueue *pq) {
    if (pq->size == 0) {
        TrafficData empty = {"", 0, 0};
        return empty;
    }

    TrafficData result = pq->data[0];
    pq->size--;
    pq->data[0] = pq->data[pq->size];

    int index = 0;
    while (2 * index + 1 < pq->size) {
        int left = 2 * index + 1;
        int right = 2 * index + 2;
        int smallest = index;

        if (left < pq->size && comparePriority(&pq->data[left], &pq->data[smallest]) < 0) {
            smallest = left;
        }
        if (right < pq->size && comparePriority(&pq->data[right], &pq->data[smallest]) < 0) {
            smallest = right;
        }

        if (smallest == index) break;

        TrafficData temp = pq->data[index];
        pq->data[index] = pq->data[smallest];
        pq->data[smallest] = temp;

        index = smallest;
    }

    return result;
}

int readTrafficData(const char *filename, TrafficData *trafficDataArr) {
    FILE *fp = fopen(filename, "r");
    int index = 0;

    if (fp) {
        while (fscanf(fp, "%49[^,],%d\n", trafficDataArr[index].flow_name, &trafficDataArr[index].time) != EOF) {
            index++;
            if (index >= MAX_SIZE) break;
        }
        fclose(fp);
    } else {
        printf("Unable to open file %s\n", filename);
        return -1;
    }

    return index;
}

void processTrafficData(const char *inputFile, const char *outputFile) {
    TrafficData trafficDataArray[MAX_SIZE];
    int numEntries = readTrafficData(inputFile, trafficDataArray);

    if (numEntries < 0) return;

    PriorityQueue pq;
    initQueue(&pq);

    int i;
    for (i = 0; i < numEntries; i++) {
        if (strstr(trafficDataArray[i].flow_name, "Main Lane")) {
            trafficDataArray[i].priority = 1;
        } else if (strstr(trafficDataArray[i].flow_name, "Pedestrian on Main Lane")) {
            trafficDataArray[i].priority = 2;
        } else if (strstr(trafficDataArray[i].flow_name, "Pedestrian on Diversion Lane")) {
            trafficDataArray[i].priority = 3;
        } else if (strstr(trafficDataArray[i].flow_name, "Diversion Lane")) {
            trafficDataArray[i].priority = 4;
        } else if (strstr(trafficDataArray[i].flow_name, "Left Turn from Main Lane")) {
            trafficDataArray[i].priority = 5;
        } else {
            trafficDataArray[i].priority = 6;
        }

        push(&pq, trafficDataArray[i]);
    }

    int totalWaitTime = 0;
    FILE *outFile = fopen(outputFile, "w");
    if (outFile) {
        while (pq.size > 0) {
            TrafficData current = pop(&pq);
            if (strstr(current.flow_name, "Pedestrian")) {
                totalWaitTime += current.time;
                fprintf(outFile, "Scenario: %s, Total Wait Time for Pedestrian: %d seconds\n", current.flow_name, totalWaitTime);
            }
        }
        fclose(outFile);
    } else {
        printf("Unable to open output file %s\n", outputFile);
    }
}

void displayTrafficResults(const char *filename) {
    FILE *fp = fopen(filename, "r");
    char line[100];

    if (fp != NULL) {
        printf("Contents of %s:\n", filename);
        while (fgets(line, sizeof(line), fp) != NULL) {
            printf("%s", line);
        }
        fclose(fp);
    } else {
        printf("Unable to open file %s for reading.\n", filename);
    }
}

int main() {
    processTrafficData("traffic.dat", "traffic_result.dat");
    printf("Results written to traffic_result.dat\n");

    displayTrafficResults("traffic_result.dat");

    return 0;
}

