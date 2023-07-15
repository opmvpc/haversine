#include <iostream>

template <typename T>
class ArrayList
{
private:
    T *array;
    int capacity;
    int size;

public:
    ArrayList()
    {
        capacity = 10;
        array = new T[capacity];
        size = 0;
    }

    ~ArrayList()
    {
        delete[] array;
    }

    void add(const T &element)
    {
        if (size == capacity)
        {
            capacity *= 2;
            T *newArray = new T[capacity];
            for (int i = 0; i < size; i++)
            {
                newArray[i] = array[i];
            }
            delete[] array;
            array = newArray;
        }
        array[size] = element;
        size++;
    }

    T &operator[](int index)
    {
        if (index < 0 || index >= size)
        {
            throw std::out_of_range("Index out of bounds");
        }
        return array[index];
    }

    const T &operator[](int index) const
    {
        if (index < 0 || index >= size)
        {
            throw std::out_of_range("Index out of bounds");
        }
        return array[index];
    }

    int getSize() const
    {
        return size;
    }

    ArrayList &operator=(const ArrayList &other) = delete;
    ArrayList(const ArrayList &other) = delete;
};
