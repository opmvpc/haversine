#include <stdio.h>
#include "ArrayList.h"
#include <string.h>
#include <exception>

struct Json;
struct Value;
struct Object;
struct Member;
struct Array;

char next(char *buffer, int *current);
char peak(char *buffer, int *current);
char nextPeak(char *buffer, int *current);
Value getElement(char *buffer, int *current);
Value getValue(char *buffer, int *current);
void clearWhitespace(char *buffer, int *current);
Object *getObject(char *buffer, int *current);
void getMembers(char *buffer, int *current, ArrayList<Member> *members);
Member getMember(char *buffer, int *current);
char *getString(char *buffer, int *current);
Array *getArray(char *buffer, int *current);
Value &getByIndex(ArrayList<Member> &elements, int index);
Value &getByIndex(ArrayList<Value> &elements, int index);
Value &getByName(ArrayList<Member> &elements, const char *name);

struct Object
{
    ArrayList<Member> members;
};

struct Array
{
    ArrayList<Value> values;
};

struct Value
{
    enum Type
    {
        OBJECT,
        ARRAY,
        STRING,
        NUMBER,
        BOOLEAN,
        NULL_VALUE,
    } type;
    union
    {
        Object *object;
        Array *array;
        char *string;
        double number;
        bool boolean;
    };

    int size()
    {
        if (type == OBJECT)
        {
            return object->members.getSize();
        }
        else if (type == ARRAY)
        {
            return array->values.getSize();
        }
        else
        {
            throw std::runtime_error("Not an array or object");
        }
    }

    // [] operator
    Value &operator[](int index)
    {
        if (type == OBJECT)
        {
            return getByIndex(object->members, index);
        }
        else if (type == ARRAY)
        {
            return getByIndex(array->values, index);
        }
        else
        {
            throw std::runtime_error("Not an array or object");
        }
    }

    Value &operator[](const char *name)
    {
        if (type == OBJECT)
        {
            return getByName(object->members, name);
        }
        else
        {
            throw std::runtime_error("Not an object");
        }
    }
};

struct Member
{
    char *name;
    Value value;
};

struct Json
{
    Value value;
    ~Json()
    {
        // delete everything
    }

    int size()
    {
        if (value.type == Value::OBJECT)
        {
            return value.object->members.getSize();
        }
        else if (value.type == Value::ARRAY)
        {
            return value.array->values.getSize();
        }
        else
        {
            throw std::runtime_error("Not an array or object");
        }
    }

    Value &operator[](int index)
    {
        if (value.type == Value::OBJECT)
        {
            return getByIndex(value.object->members, index);
        }
        else if (value.type == Value::ARRAY)
        {
            return getByIndex(value.array->values, index);
        }
        else
        {
            throw std::runtime_error("Not an array or object");
        }
    }

    Value &operator[](const char *name)
    {
        if (value.type == Value::OBJECT)
        {
            return getByName(value.object->members, name);
        }
        else
        {
            throw std::runtime_error("Not an object");
        }
    }
};

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

    // put everything in a buffer
    FILE *inputFile = fopen(inputFileName, "rb");
    fseek(inputFile, 0, SEEK_END);
    long inputFileSize = ftell(inputFile);
    fseek(inputFile, 0, SEEK_SET);
    char *inputFileBuffer = new char[inputFileSize];
    fread(inputFileBuffer, 1, inputFileSize, inputFile);

    int current = 0;
    int start = 0;

    Json json = Json();

    json.value = getElement(inputFileBuffer, &current);

    printf("%s\n", json[0][0].string);
    printf("%d\n", json[0].size());
    printf("%d\n", json[0][2].size());
    printf("%s\n", json["hello"]["coucou"].string);
    printf("%d\n", json["hello"]["sava"].size());
    printf("%d\n", json["hello"]["salut"].size());

    return 0;
}

char next(char *buffer, int *current)
{
    return buffer[(*current)++];
}

char peak(char *buffer, int *current)
{
    return buffer[*current];
}

char nextPeak(char *buffer, int *current)
{
    return buffer[*current + 1];
}

Value getElement(char *buffer, int *current)
{
    clearWhitespace(buffer, current);
    Value value = getValue(buffer, current);
    clearWhitespace(buffer, current);

    return value;
}

Value getValue(char *buffer, int *current)
{
    Value value = Value();
    char c = next(buffer, current);
    switch (c)
    {
    case '{':
        value.type = Value::OBJECT;
        clearWhitespace(buffer, current);
        value.object = getObject(buffer, current);
        clearWhitespace(buffer, current);
        break;
    case '[':
        value.type = Value::ARRAY;
        value.array = getArray(buffer, current);
        break;
    case '"':
        value.type = Value::STRING;
        value.string = getString(buffer, current);
        break;
    default:
        break;
    }
    return value;
}

void clearWhitespace(char *buffer, int *current)
{
    char c = next(buffer, current);
    while (c == ' ' || c == '\n' || c == '\t' || c == '\r')
    {
        c = next(buffer, current);
    }
    (*current)--;
}

Object *getObject(char *buffer, int *current)
{
    Object *object = new Object();
    clearWhitespace(buffer, current);
    if (peak(buffer, current) == '}')
    {
        next(buffer, current);
        return object;
    }
    getMembers(buffer, current, &object->members);

    if (peak(buffer, current) != '}')
    {
        throw std::runtime_error("Expected '}'");
    }

    next(buffer, current);

    return object;
}

void getMembers(char *buffer, int *current, ArrayList<Member> *members)
{
    members->add(getMember(buffer, current));
    if (peak(buffer, current) == ',')
    {
        next(buffer, current);
        getMembers(buffer, current, members);
    }
}

Member getMember(char *buffer, int *current)
{
    Member member = Member();
    clearWhitespace(buffer, current);
    char c = next(buffer, current);
    if (c != '"')
    {
        throw std::runtime_error("Expected '\"'");
    }
    member.name = getString(buffer, current);
    clearWhitespace(buffer, current);
    c = next(buffer, current);
    if (c != ':')
    {
        throw std::runtime_error("Expected ':'");
    }
    member.value = getElement(buffer, current);

    return member;
}

char *getString(char *buffer, int *current)
{
    int i = 0;

    int start = *current;

    char c = next(buffer, current);
    while (c != '"')
    {
        c = next(buffer, current);
    }

    if (c != '"')
    {
        throw std::runtime_error("Expected '\"'");
    }

    char *string = strncpy(new char[(*current) - (start)], buffer + (start), (*current) - (start));
    string[(*current - 1) - (start)] = '\0';
    return string;
}

Array *getArray(char *buffer, int *current)
{
    Array *array = new Array();
    return array;
}

Value &getByIndex(ArrayList<Member> &elements, int index)
{
    return elements[index].value;
}

Value &getByIndex(ArrayList<Value> &elements, int index)
{
    return elements[index];
}

Value &getByName(ArrayList<Member> &elements, const char *name)
{
    for (int i = 0; i < elements.getSize(); i++)
    {
        if (strcmp(elements[i].name, name) == 0)
        {
            return elements[i].value;
        }
    }
    throw std::runtime_error("No such member");
}
