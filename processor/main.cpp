#include <stdio.h>
#include "parser.h"
#include "solver/solver.h"

typedef double f64;

int main(int argc, char const *argv[])
{
    if (argc != 2 && argc != 3)
    {
        printf("Usage:   haversine_processor [input.json]\n");
        printf("                             [input.json] [answers.f64]\n");
        return 1;
    }

    const char *inputFileName = argv[1];
    const char *resultsFileName = argv[2];

    // open the results file if provided
    FILE *resultsFile = NULL;
    char *resultsBuffer = NULL;
    int resultsSize = 0;
    if (resultsFileName)
    {
        resultsFile = fopen(resultsFileName, "rb");
        // load the results in a buffer
        fseek(resultsFile, 0, SEEK_END);
        resultsSize = ftell(resultsFile);
        fseek(resultsFile, 0, SEEK_SET);
        resultsBuffer = new char[resultsSize];
        fread(resultsBuffer, 1, resultsSize, resultsFile);
        fclose(resultsFile);
    }

    Json json = parse(inputFileName);

    ArrayList<Value> &coordinates = json["pairs"].array->values;
    f64 totalDistance = 0.0;
    for (int i = 0; i < coordinates.getSize(); i++)
    {
        Value &coordinate = coordinates[i];
        f64 x0 = coordinate["x0"].number;
        f64 y0 = coordinate["y0"].number;
        f64 x1 = coordinate["x1"].number;
        f64 y1 = coordinate["y1"].number;
        f64 distance = referenceHaversine(x0, y0, x1, y1, 6372.8);
        totalDistance += distance;

        // compare the results if provided
        if (resultsSize > 0)
        {
            f64 result = *(f64 *)(resultsBuffer + i * sizeof(f64));
            if (result != distance)
            {
                printf("Error: result %d is %.20f, expected %.20f\n", i, distance, result);
            }
        }
    }

    f64 averageDistance = totalDistance / coordinates.getSize();
    printf("Total distance: %.20f\n", totalDistance);
    printf("Average distance: %.20f\n", averageDistance);

    // compare the results if provided
    if (resultsSize > 0)
    {
        f64 result = *(f64 *)(resultsBuffer + coordinates.getSize() * sizeof(f64));
        if (result != averageDistance)
        {
            printf("\nError: average distance is %.20f, expected %.20f\n", averageDistance, result);
        }
        else
        {
            printf("\nAverage distance is correct! ✨\n");
        }

        delete[] resultsBuffer;
    }

    return 0;
}
