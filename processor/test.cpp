#include <stdio.h>
#include "parser.h"
#include <cassert>

int main(int argc, char const *argv[])
{
    printf("Testing parser...\n\n");
    const char *fileName = "processor/test.json";
    Json json = parse(fileName);
    assert(json.size() == 6);
    assert(json["hello"].size() == 3);
    assert(json["hello"]["sava"].size() == 0);
    printf("\t✅ Can access to object size\n");
    assert(strcmp(json["hello"]["coucou"].string, "hadopire") == 0);
    printf("\t✅ Can access to object string member\n");
    assert(json[0].size() == 3);
    assert(strcmp(json[0][0].string, "hadopire") == 0);
    printf("\t✅ Can access to object size by index\n");
    assert(json["array"].size() == 2);
    assert(json["array"][0].size() == 3);
    assert(json["array"][1].size() == 3);
    printf("\t✅ Can access to array size\n");
    assert(strcmp(json["array"][0][0].string, "a") == 0);
    assert(strcmp(json["array"][0][1].string, "b") == 0);
    assert(strcmp(json["array"][0][2].string, "c") == 0);
    assert(strcmp(json["array"][1][0].string, "d") == 0);
    assert(strcmp(json["array"][1][1].string, "e") == 0);
    assert(strcmp(json["array"][1][2].string, "f") == 0);
    printf("\t✅ Can access to array string member\n");
    assert(json["true"].boolean == true);
    assert(json["false"].boolean == false);
    assert(json["null"].null == true);
    printf("\t✅ Can access to boolean and null member\n");
    assert(json["numbers"].size() == 8);
    assert(json["numbers"][0].number == 1);
    assert(json["numbers"][1].number == 2231);
    assert(json["numbers"][2].number == 9956567912364354);
    assert(json["numbers"][3].number == 0.1);
    assert(json["numbers"][4].number == 0.01212054821);
    assert(json["numbers"][5].number == 11000000000);
    assert(json["numbers"][6].number == 0.011000);
    assert(json["numbers"][7].number == 110);
    printf("\t✅ Can access to number member\n");

    const char *fileName2 = "processor/test2.json";
    Json json2 = parse(fileName2);
    assert(strcmp(json2["glossary"]["title"].string, "example glossary") == 0);
    assert(strcmp(json2["glossary"]["GlossDiv"]["title"].string, "S") == 0);
    assert(strcmp(json2["glossary"]["GlossDiv"]["GlossList"]["GlossEntry"]["ID"].string, "SGML") == 0);
    assert(strcmp(json2["glossary"]["GlossDiv"]["GlossList"]["GlossEntry"]["GlossDef"]["GlossSeeAlso"][0].string, "GML") == 0);
    printf("\t✅ Can access to nested object\n");

    printf("\nAll tests passed 🎉\n");

    return 0;
}
