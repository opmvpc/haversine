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

    printf("%s\n", json["glossary"]["title"].string);
    printf("%s\n", json["glossary"]["GlossDiv"]["title"].string);
    printf("%s\n", json["glossary"]["GlossDiv"]["GlossList"]["GlossEntry"]["ID"].string);
    printf("%s\n", json["glossary"]["GlossDiv"]["GlossList"]["GlossEntry"]["GlossDef"]["GlossSeeAlso"][0].string);

    // printf("%s\n", json[0][0].string);
    // printf("%d\n", json[0].size());
    // printf("%d\n", json[0][2].size());
    // printf("%s\n", json["hello"]["coucou"].string);
    // printf("%d\n", json["hello"]["sava"].size());
    // printf("%d\n", json["hello"]["salut"].size());
    // printf("%s\n", json["array"][0][0].string);
    // printf("%s\n", json["array"][0][1].string);
    // printf("%s\n", json["array"][0][2].string);
    // printf("%s\n", json["array"][1][0].string);
    // printf("%s\n", json["array"][1][1].string);
    // printf("%s\n", json["array"][1][2].string);
    // printf("%d\n", json["true"].boolean);
    // printf("%d\n", json["false"].boolean);
    // printf("%d\n", json["null"].null);
    // printf("%f\n", json["numbers"][0].number);
    // printf("%f\n", json["numbers"][1].number);
    // printf("%f\n", json["numbers"][2].number);
    // printf("%f\n", json["numbers"][3].number);
    // printf("%.10f\n", json["numbers"][4].number);

    return 0;
}
