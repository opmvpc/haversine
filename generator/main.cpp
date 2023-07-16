#include <stdio.h>
#include <stdlib.h>
#include <random>
#include <string.h>
#include "solver/solver.h"
#include <algorithm>

int main(int argc, char const *argv[])
{
    if (argc != 4)
    {
        printf("Usage: %s [uniform/cluster] [random seed] [number of coordinate pairs to generate]\n", argv[0]);
        return 1;
    }

    const char *mode = argv[1];
    int seed = atoi(argv[2]);
    int pairsCount = atoi(argv[3]);

    // print cool things

    printf("Generating %d coordinate pairs using %s mode with seed %d\n", pairsCount, mode, seed);

    std::mt19937_64 rng(seed);

    // create a json file
    char fileName[128];
    sprintf(fileName, "storage/coordinates_%s_%d_%d.json", mode, seed, pairsCount);
    FILE *jsonFile = fopen(fileName, "wb");
    fprintf(jsonFile, "{\"pairs\":[\n");

    // create a result file
    sprintf(fileName, "storage/results_%s_%d_%d.f64", mode, seed, pairsCount);
    FILE *resultFile = fopen(fileName, "wb");

    f64 totalDistance = 0.0;
    bool isUniform = strcmp(mode, "uniform") == 0;
    bool isCluster = strcmp(mode, "cluster") == 0;
    double x0, y0, x1, y1;
    int minX, minY, maxX, maxY;

    // generate coordinate pairs
    if (isUniform)
    {
        for (int i = 0; i < pairsCount; i++)
        {

            x0 = rng() / (double)rng.max() * 360.0 - 180.0;
            y0 = rng() / (double)rng.max() * 180.0 - 90.0;
            x1 = rng() / (double)rng.max() * 360.0 - 180.0;
            y1 = rng() / (double)rng.max() * 180.0 - 90.0;

            fprintf(jsonFile, "{\"x0\":%.20f,\"y0\":%.20f,\"x1\":%.20f,\"y1\":%.20f}", x0, y0, x1, y1);

            if (i < pairsCount - 1)
            {
                fprintf(jsonFile, ",\n");
            }

            // calculate distance
            f64 distance = referenceHaversine(x0, y0, x1, y1, 6372.8);
            totalDistance += distance;

            // write to result file
            fwrite(&distance, sizeof(f64), 1, resultFile);
        }
    }
    else if (isCluster)
    {
        int clusterCount = 18;
        int counter = 0;
        for (int i = 0; i < clusterCount; i++)
        {
            minX = rng() / (double)rng.max() * 360.0 - 180.0;
            minY = rng() / (double)rng.max() * 180.0 - 90.0;
            maxX = std::min(minX + rng() / (double)rng.max() * 180.0, 180.0);
            maxY = std::min(minY + rng() / (double)rng.max() * 90.0, 90.0);

            int maxIndex = pairsCount / clusterCount * (i + 1);
            if (i == clusterCount - 1)
            {
                maxIndex = pairsCount;
            }

            while (counter < maxIndex)
            {
                x0 = rng() / (double)rng.max() * (maxX - minX) + minX;
                y0 = rng() / (double)rng.max() * (maxY - minY) + minY;
                x1 = rng() / (double)rng.max() * (maxX - minX) + minX;
                y1 = rng() / (double)rng.max() * (maxY - minY) + minY;

                fprintf(jsonFile, "{\"x0\":%.20f,\"y0\":%.20f,\"x1\":%.20f,\"y1\":%.20f}", x0, y0, x1, y1);

                if (counter < pairsCount - 1)
                {
                    fprintf(jsonFile, ",\n");
                }

                // calculate distance
                f64 distance = referenceHaversine(x0, y0, x1, y1, 6372.8);
                totalDistance += distance;

                // write to result file
                fwrite(&distance, sizeof(f64), 1, resultFile);

                counter++;
            }
        }
    }
    else
    {
        printf("Invalid mode: %s\n", mode);
        return 1;
    }

    // compute the average distance
    f64 averageDistance = totalDistance / pairsCount;
    // print cool things
    printf("Average distance: %.20f\n", averageDistance);
    // write to result file
    fwrite(&averageDistance, sizeof(f64), 1, resultFile);
    fclose(resultFile);

    // end of json file
    fprintf(jsonFile, "]}");
    fclose(jsonFile);

    return 0;
}
