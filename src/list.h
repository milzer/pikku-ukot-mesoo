#ifndef __LIST_H__
#define __LIST_H__

template <class T>
class CList
{
    struct  Index {
        T       *object;
        Index   *next;
    };

    Index   *first,
    *current;

public:
    CList();
    ~CList();

    void    Clear();
    void    Add(T *);
    T       *DelCurrent();
    T       *First();
    T       *Last();
    T       *Next();
    T       *Current();
    int     Count();
};

template <class T>
CList<T>::CList()
{
    first = current = NULL;
}

template <class T>
CList<T>::~CList()
{
    Index   *del;

    current = first;

    while (current != NULL) {
        if (current->object != NULL)
            delete current->object;

        del = current;
        current = current->next;

        delete del;
    }
}

template <class T>
void CList<T>::Clear()
{
    Index   *del;

    current = first;

    while (current != NULL) {
        del = current;
        current = current->next;

        delete del;
    }
}

template <class T>
void CList<T>::Add(T *o)
{
    Index   *tmp = new Index;

    tmp->object = o;
    tmp->next = NULL;

    if (first == NULL) {
        first = current = tmp;
    } else {
        current = first;

        while (current != NULL && current->next != NULL)
            current = current->next;

        current->next = tmp;
        current = tmp;
    }
}

template <class T>
T *CList<T>::DelCurrent()
{
    T   *del;

    if (first == NULL) {
        del = NULL;
    } else {
        if (first == current) {
            del = first->object;
            first = first->next;

            delete current;

            current = first;
        } else {
            Index   *prev = first;

            while (prev != NULL && prev->next != current)
                prev = prev->next;

            del = current->object;
            prev->next = current->next;

            delete current;

            current = prev;
        }
    }

    return del;
}

template <class T>
T *CList<T>::First()
{
    T   *tmp = NULL;

    if (first != NULL) {
        tmp = first->object;
        current = first;
    }

    return tmp;
}

template <class T>
T *CList<T>::Last()
{
    T   *tmp = NULL;

    current = first;

    while (current != NULL && current->next != NULL)
        current = current->next;

    if (current != NULL)
        tmp = current->object;

    return tmp;
}

template <class T>
T *CList<T>::Next()
{
    T   *tmp = NULL;

    if (current != NULL && current->next != NULL) {
        current = current->next;
        tmp = current->object;
    }

    return tmp;
}

template <class T>
T *CList<T>::Current()
{
    T   *tmp = NULL;

    if (current != NULL)
        tmp = current->object;

    return tmp;
}

template <class T>
int CList<T>::Count()
{
    int c = 0;
    T   *tmp = NULL;

    current = first;

    while (current != NULL && current->next != NULL) {
        c++;
        current = current->next;
    }

    if (current != NULL)
        tmp = current->object;

    return c;
}

#endif