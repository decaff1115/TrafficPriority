#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef char String[50];  

typedef struct {
    int priority;   
    String flow_name;
    float time;
} TrafficData;

void writeTrafficData(String filename, TrafficData *trafficDataArr, int numEntries);
int readTrafficData(String filename, TrafficData *trafficDataArr);
void processTrafficData(String inputFile, String outputFile);
int comparePriority(const void *a, const void *b);  

int main() {
    TrafficData trafficDArr[] = {
        {1, "Main Lane", 10},
        {2, "Diversion Lane", 15},
        {3, "Pedestrian on Main Lane", 20},
        {4, "Pedestrian on Diversion Lane", 25},
        {5, "Left Turn from Main Lane", 30},
        {6, "Left Turn from Diversion Lane", 35}
    };

    int numEntries = 6;
    String filename = "traffic.dat";  

    writeTrafficData(filename, trafficDArr, numEntries);
    printf("Traffic data has been written to '%s'.\n", filename);

    processTrafficData(filename, "traffic_result.dat");

    return 0;
}

void writeTrafficData(String filename, TrafficData *trafficDataArr, int numEntries) {
    FILE *fp = fopen(filename, "w");
    if (fp != NULL) {
        int x;
        for (x = 0; x < numEntries; x++) {
            fprintf(fp, "%d,%s,%.2f\n", trafficDataArr[x].priority, trafficDataArr[x].flow_name, trafficDataArr[x].time);
        }
        fclose(fp);
    } else {
        printf("Unable to open file %s for writing.\n", filename);
    }
}

int readTrafficData(String filename, TrafficData *trafficDataArr) {
    FILE *fp = fopen(filename, "r");
    int index = 0;

    if (fp != NULL) {
        while (fscanf(fp, "%d,%49[^,],%f\n", &trafficDataArr[index].priority, trafficDataArr[index].flow_name, &trafficDataArr[index].time) != EOF) {
            index++;
            if (index >= 10) break; 
        }
        fclose(fp);
    } else {
        printf("Unable to open file %s for reading.\n", filename);
        return -1;
    }

    return index;
}

void processTrafficData(String inputFile, String outputFile) {
    TrafficData trafficDataArray[10];
    int numEntries = readTrafficData(inputFile, trafficDataArray);

    if (numEntries < 0) {
        return;
    }

    qsort(trafficDataArray, numEntries, sizeof(TrafficData), comparePriority);

    float totalWaitTime = 0;

    int i;
    for (i = 0; i < numEntries; i++) {
        if (strstr(trafficDataArray[i].flow_name, "Pedestrian") != NULL) {
            totalWaitTime += trafficDataArray[i].time;
        }
    }

    printf("Total Wait Time for All Pedestrian Scenarios: %.2f seconds\n", totalWaitTime);  

   
    FILE *outFile = fopen(outputFile, "w");
    if (outFile != NULL) {
        for (i = 0; i < numEntries; i++) {
            if (strstr(trafficDataArray[i].flow_name, "Pedestrian") != NULL) {
                fprintf(outFile, "Scenario: %s, Priority: %d, Total Wait Time for Pedestrian: %.2f seconds\n", 
                        trafficDataArray[i].flow_name, trafficDataArray[i].priority, totalWaitTime);
            }
        }
        fclose(outFile);
    } else {
        printf("Unable to open output file %s\n", outputFile);
    }
}

int comparePriority(const void *a, const void *b) {
 
    TrafficData *ta = (TrafficData *)a;
    TrafficData *tb = (TrafficData *)b;

   
    return ta->priority - tb->priority;
}

