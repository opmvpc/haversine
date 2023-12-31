#include <stdio.h>
#include "ArrayList.h"
#include <string.h>
#include <exception>
#include <charconv>

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
void escapeWhitespaces(char *buffer, int *current);
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

    char *print()
    {
        int indent = 0;
        return printRecursive(value, &indent);
    }

    char *printRecursive(Value &value, int *indent)
    {
        char *buffer = new char[1024];

        if (value.type == Value::OBJECT)
        {

            if (value.object->members.getSize() == 0)
            {
                buffer[0] = '{';
                buffer[1] = '}';
                buffer[2] = '\0';
                return buffer;
            }

            buffer[0] = '{';
            buffer[1] = '\n';
            buffer[2] = '\0';
            *indent += 1;

            for (int i = 0; i < value.object->members.getSize(); i++)
            {
                for (int j = 0; j < *indent; j++)
                {
                    strcat(buffer, "    ");
                }
                strcat(buffer, "\"");
                strcat(buffer, value.object->members[i].name);
                strcat(buffer, "\"");

                strcat(buffer, ": ");
                strcat(buffer, printRecursive(value.object->members[i].value, indent));
                if (i < value.object->members.getSize() - 1)
                {
                    strcat(buffer, ",\n");
                }
            }
            *indent -= 1;
            strcat(buffer, "\n");
            for (int j = 0; j < *indent; j++)
            {
                strcat(buffer, "    ");
            }
            strcat(buffer, "}");
            return buffer;
        }
        else if (value.type == Value::ARRAY)
        {
            buffer[0] = '[';
            buffer[1] = '\0';
            for (int i = 0; i < value.array->values.getSize(); i++)
            {
                strcat(buffer, printRecursive(value.array->values[i], indent));
                if (i < value.array->values.getSize() - 1)
                {
                    strcat(buffer, ", ");
                }
            }
            strcat(buffer, "]");
            return buffer;
        }
        else if (value.type == Value::STRING)
        {
            buffer[0] = '"';
            buffer[1] = '\0';
            strcat(buffer, value.string);
            strcat(buffer, "\"");
            return buffer;
        }
        else if (value.type == Value::NUMBER)
        {
            sprintf(buffer, "%f", value.number);
            return buffer;
        }
        else if (value.type == Value::BOOLEAN)
        {
            if (value.boolean)
            {
                strcpy(buffer, "true");
            }
            else
            {
                strcpy(buffer, "false");
            }
            return buffer;
        }
        else if (value.type == Value::NULL_VALUE)
        {
            strcpy(buffer, "null");
            return buffer;
        }
        else
        {
            throw std::runtime_error("Unknown value type");
        }
    }
};

Json parse(const char *inputFileName)
{
    Json json = Json();

    try
    {
        // Open input file
        FILE *inputFile = fopen(inputFileName, "rb");
        if (!inputFile)
        {
            throw std::runtime_error("Failed to open input file.");
        }

        // Get the file size
        fseek(inputFile, 0, SEEK_END);
        long inputFileSize = ftell(inputFile);
        fseek(inputFile, 0, SEEK_SET);

        // Create a buffer to hold the file contents
        char *inputFileBuffer = new char[inputFileSize];

        // Read the file into the buffer
        size_t bytesRead = fread(inputFileBuffer, 1, inputFileSize, inputFile);
        if (bytesRead != inputFileSize)
        {
            throw std::runtime_error("Failed to read input file.");
        }

        int current = 0;
        json.value = getElement(inputFileBuffer, &current);

        delete[] inputFileBuffer;
        fclose(inputFile);
    }
    catch (const std::exception &e)
    {
        std::cout << "Exception occurred: " << e.what() << std::endl;
    }

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
    escapeWhitespaces(buffer, current);
    Value value = getValue(buffer, current);
    escapeWhitespaces(buffer, current);

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
        escapeWhitespaces(buffer, current);
        value.object = getObject(buffer, current);
        escapeWhitespaces(buffer, current);
        break;
    case '[':
        value.type = Value::ARRAY;
        escapeWhitespaces(buffer, current);
        value.array = getArray(buffer, current);
        escapeWhitespaces(buffer, current);
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

void escapeWhitespaces(char *buffer, int *current)
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
    escapeWhitespaces(buffer, current);
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

    while (peak(buffer, current) == ',')
    {
        next(buffer, current);
        members->add(getMember(buffer, current));
    }
}

Member getMember(char *buffer, int *current)
{
    Member member = Member();
    escapeWhitespaces(buffer, current);
    char c = next(buffer, current);
    if (c != '"')
    {
        throw std::runtime_error("Expected '\"'");
    }
    member.name = getString(buffer, current);
    escapeWhitespaces(buffer, current);
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
    escapeWhitespaces(buffer, current);
    if (peak(buffer, current) == ']')
    {
        next(buffer, current);
        return array;
    }

    getValues(buffer, current, &array->values);

    escapeWhitespaces(buffer, current);

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

    while (peak(buffer, current) == ',')
    {
        next(buffer, current);
        escapeWhitespaces(buffer, current);
        values->add(getValue(buffer, current));
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
    throw std::runtime_error("No such member: \"" + std::string(name) + "\"");
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

    try
    {
        double d;
        std::from_chars(buffer + start, buffer + *current, d);
        // return strtod(buffer + start, nullptr);
        return d;
    }
    catch (const std::out_of_range &e)
    {
        // throw std::runtime_error("Invalid number, too large : \"" + std::string(number) + "\"");
    }
}
