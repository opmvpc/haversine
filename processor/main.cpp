#include <stdio.h>
#include "parser.h"

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

    Json json = parse(inputFileName);

    printf("%s\n", json[0][0].string);
    printf("%d\n", json[0].size());
    printf("%d\n", json[0][2].size());
    printf("%s\n", json["hello"]["coucou"].string);
    printf("%d\n", json["hello"]["sava"].size());
    printf("%d\n", json["hello"]["salut"].size());

    return 0;
}
