#include <stdio.h>
#include "ArrayList.h"
#include <string.h>
#include <exception>

typedef double f64;

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
void getValues(char *buffer, int *current, ArrayList<Value> *values);
Value &getByIndex(ArrayList<Member> &elements, int index);
Value &getByIndex(ArrayList<Value> &elements, int index);
Value &getByName(ArrayList<Member> &elements, const char *name);
bool isDigit(char c);
f64 getNumber(char *buffer, int *current);

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
        f64 number;
        bool boolean;
        bool null;
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
        deleteRecursive(value);
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

    void deleteRecursive(Value &value)
    {
        if (value.type == Value::OBJECT)
        {
            for (int i = 0; i < value.object->members.getSize(); i++)
            {
                deleteRecursive(value.object->members[i].value);
            }
            delete value.object;
        }
        else if (value.type == Value::ARRAY)
        {
            for (int i = 0; i < value.array->values.getSize(); i++)
            {
                deleteRecursive(value.array->values[i]);
            }
            delete value.array;
        }
        else if (value.type == Value::STRING)
        {
            delete[] value.string;
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

Json parse(const char *inputFileName)
{
    FILE *inputFile = fopen(inputFileName, "rb");
    fseek(inputFile, 0, SEEK_END);
    long inputFileSize = ftell(inputFile);
    fseek(inputFile, 0, SEEK_SET);
    char *inputFileBuffer = new char[inputFileSize];
    fread(inputFileBuffer, 1, inputFileSize, inputFile);

    Json json = Json();

    int current = 0;
    json.value = getElement(inputFileBuffer, &current);

    delete[] inputFileBuffer;
    fclose(inputFile);

    return json;
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
        clearWhitespace(buffer, current);
        value.array = getArray(buffer, current);
        clearWhitespace(buffer, current);
        break;
    case '"':
    {
        char *string = getString(buffer, current);
        if (strcmp(string, "true") == 0)
        {
            value.type = Value::BOOLEAN;
            value.boolean = true;
        }
        else if (strcmp(string, "false") == 0)
        {
            value.type = Value::BOOLEAN;
            value.boolean = false;
        }
        else if (strcmp(string, "null") == 0)
        {
            value.type = Value::NULL_VALUE;
            value.null = true;
        }
        else
        {
            value.type = Value::STRING;
            value.string = string;
        }
        break;
    }
    default:
        if (isDigit(c))
        {
            value.type = Value::NUMBER;
            value.number = getNumber(buffer, current);
        }
        else
        {
            throw std::runtime_error("Unexpected character");
        }

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
    clearWhitespace(buffer, current);
    if (peak(buffer, current) == ']')
    {
        next(buffer, current);
        return array;
    }

    getValues(buffer, current, &array->values);

    clearWhitespace(buffer, current);

    if (peak(buffer, current) != ']')
    {
        throw std::runtime_error("Expected ']'");
    }

    next(buffer, current);

    return array;
}

void getValues(char *buffer, int *current, ArrayList<Value> *values)
{
    values->add(getValue(buffer, current));
    if (peak(buffer, current) == ',')
    {
        next(buffer, current);
        clearWhitespace(buffer, current);
        getValues(buffer, current, values);
    }
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

bool isDigit(char c)
{
    return c == '-' || (c >= '0' && c <= '9');
}

f64 getNumber(char *buffer, int *current)
{
    (*current)--;
    int start = *current;
    char c = next(buffer, current);
    while (isDigit(c))
    {
        c = next(buffer, current);
    }
    (*current)--;

    if (peak(buffer, current) == '.')
    {
        next(buffer, current);
        c = next(buffer, current);
        while (isDigit(c))
        {
            c = next(buffer, current);
        }
        (*current)--;
    }

    if (peak(buffer, current) == 'e' || peak(buffer, current) == 'E')
    {
        next(buffer, current);
        c = peak(buffer, current);
        if (c == '+' || c == '-')
        {
            next(buffer, current);
        }
        c = next(buffer, current);
        while (isDigit(c))
        {
            c = next(buffer, current);
        }
        (*current)--;
    }

    char *number = strncpy(new char[(*current) - (start) + 1], buffer + (start), (*current) - (start));
    number[(*current) - (start)] = '\0';

    try
    {
        return std::stod(number);
    }
    catch (const std::out_of_range &e)
    {
        throw std::runtime_error("Invalid number, too large : \"" + std::string(number) + "\"");
    }
}
