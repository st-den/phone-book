#define _CRT_SECURE_NO_WARNINGS

#include <string.h>
#include <iostream>

using namespace std;

const int max_name_len = 32;
const int max_num_len = 18;
const int max_entries = 256;
const char filepath[] = "contacts.db";

struct Entry
{
    char name[max_name_len];
    char num[max_num_len];
};

void start_app();
int load_book(Entry book[]);
void show_book(Entry book[], int size);
bool add_entry(Entry book[], int size, int& changed);
int edit_entry(Entry book[], int size);
bool del_entry(Entry book[], int size, int& changed);
bool search_entries(Entry book[], int size);
void close_book(Entry book[], int size);

void show_entry(Entry book[], int i);
char get_action();
char get_prop();
void get_name(char* name);
void get_num(char* num);
bool is_name_ok(char* name);
bool is_num_ok(char* num);

int main()
{
    start_app();

    return EXIT_SUCCESS;
}

void start_app()
{
    system("cls");

    Entry* book = new Entry[max_entries];
    int size = load_book(book);
    int changed = 0;

    while (true)
    {
        switch (get_action())
        {
        case '1':
            show_book(book, size + changed);
            break;
        case '2':
            cout << "adding" << endl;
            cout << (add_entry(book, size + changed, changed) ?
                "added." : "already exists.") << endl << endl;
            break;
        case '3':
            cout << "editing, searching for the exact match by" << endl;
            switch (edit_entry(book, size + changed))
            {
            case 0:
                cout << "edited.";
                break;
            case 1:
                cout << "not found.";
                break;
            case 2:
                cout << "already exists.";
                break;
            }
            cout << endl << endl;
            break;
        case '4':
            cout << "deleting, searching for the exact match by" << endl;
            cout << (del_entry(book, size + changed, changed) ?
                "deleted." : "not found.") << endl << endl;
            break;
        case '5':
            cout << "searching for occurences by" << endl;
            if (!search_entries(book, size + changed))
                cout << "not found." << endl << endl;
            break;
        case '6':
            close_book(book, size + changed);
            exit(EXIT_SUCCESS);
            break;
        case '7':
            exit(EXIT_SUCCESS);
            break;
        }
    }
}

int load_book(Entry book[])
{
    FILE* f = fopen(filepath, "r");
    if (!f)
    {
        cout << "failed to open a file. does it exist?" << endl;
        exit(EXIT_FAILURE);
    }

    int size = 0;
    while (!feof(f))
    {
        Entry temp;

        fgets(temp.name, max_name_len, f);
        temp.name[strlen(temp.name) - 1] = '\0';

        fgets(temp.num, max_num_len, f);
        temp.num[strlen(temp.num) - 1] = '\0';

        book[size] = temp;
        size++;
    }

    fclose(f);

    return --size;
}

void show_book(Entry book[], int size)
{
    for (int i = 0; i < size; i++)
        show_entry(book, i);
}

bool add_entry(Entry book[], int size, int& changed)
{
    Entry temp;

    get_name(temp.name);
    get_num(temp.num);

    for (int i = 0; i < size; i++)
        if (strcmp(temp.name, book[i].num) == 0
            || strcmp(temp.num, book[i].num) == 0)
            return false;

    book[size] = temp;

    for (int i = size; i > 0; i--)
        if (strcmp(book[i].name, book[i - 1].name) < 0)
            swap(book[i], book[i - 1]);

    ++changed;
    return true;
}

int edit_entry(Entry book[], int size)
{
    Entry temp;
    bool found = false;
    int i = 0;

    if (get_prop() == '1')
    {
        get_name(temp.name);

        while (i < size)
            if (strcmp(temp.name, book[i++].name) == 0)
            {
                found = true;
                break;
            }
    }
    else
    {
        get_num(temp.num);

        while (i < size)
            if (strcmp(temp.num, book[i++].num) == 0)
            {
                found = true;
                break;
            }
    }

    if (found)
    {
        i--;

        show_entry(book, i);
        cout << "\tfound, editing the contact's: " << endl;

        if (get_prop() == '1')
        {
            show_entry(book, i);
            get_name(temp.name);

            for (int j = 0; j < size; j++)
                if (strcmp(temp.name, book[j].name) == 0)
                    return 2;

            strcpy(book[i].name, temp.name);
        }
        else
        {
            show_entry(book, i);
            get_num(temp.num);

            for (int j = 0; j < size; j++)
                if (strcmp(temp.num, book[j].num) == 0)
                    return false;

            strcpy(book[i].num, temp.num);
        }

        return 0;
    }
    else
        return 1;
}

bool del_entry(Entry book[], int size, int& changed)
{
    Entry temp;
    int changed_orig = changed;
    int i = 0;

    if (get_prop() == '1')
    {
        get_name(temp.name);

        while (i < size)
            if (strcmp(temp.name, book[i++].name) == 0)
            {
                changed--;
                break;
            }
    }
    else
    {
        get_num(temp.num);

        while (i < size)
            if (strcmp(temp.num, book[i++].num) == 0)
            {
                changed--;
                break;
            }
    }
    i--;

    if (changed_orig == changed)
        return false;
    else
        for (; i < size; i++)
            book[i] = book[i + 1];
    return true;
}

bool search_entries(Entry book[], int size)
{
    Entry* temp_book = new Entry[max_entries];
    int matched = 0;
    Entry temp;

    if (get_prop() == '1')
    {
        get_name(temp.name);

        for (int i = 0; i < size; i++)
            if (strstr(book[i].name, temp.name))
                temp_book[matched++] = book[i];
    }
    else
    {
        get_num(temp.num);

        for (int i = 0; i < size; i++)
            if (strstr(book[i].num, temp.num))
                temp_book[matched++] = book[i];
    }

    if (!matched)
        return false;

    show_book(temp_book, matched);
    delete[] temp_book;
    return true;
}

void close_book(Entry book[], int size)
{
    FILE* f = fopen(filepath, "w+");
    if (!f)
    {
        cout << "failed to open a file. does it exist?" << endl;
        exit(EXIT_FAILURE);
    }

    for (int j = 0; j < size; j++)
    {
        fputs(book[j].name, f);
        fputc('\n', f);

        fputs(book[j].num, f);
        fputc('\n', f);
    }

    fclose(f);
    delete[] book;
}

void show_entry(Entry book[], int i)
{
    cout << book[i].name << endl
        << (char)200 << (char)205 << (char)205 << (char)205
        << ' ' << book[i].num << endl << endl;
}

char get_action()
{
    char* buffer = new char[256];

    do
    {
        cout << "\t1 - show all contacts" << endl
            << "\t2 - add a contact" << endl
            << "\t3 - edit a contact" << endl
            << "\t4 - delete a contact" << endl
            << "\t5 - search for contacts" << endl
            << "\t6 - save changes and exit" << endl
            << "\t7 - discard changes and exit" << endl << endl
            << "> ";

        cin >> buffer;
        cout << endl;
        system("cls");
    } while ((strlen(buffer) > 1 || buffer[0] < '1' || buffer[0] > '7')
        && cout << "wrong format. try again." << endl << endl);

    char choice = buffer[0];
    delete[] buffer;

    return choice;
}

char get_prop()
{
    char* buffer = new char[256];

    do
    {
        cout << "\t1 - name" << endl
            << "\t2 - number" << endl
            << "> ";

        cin >> buffer;
        cout << endl;
    } while ((strlen(buffer) > 1 || buffer[0] < '1' || buffer[0] > '2')
        && cout << "wrong format. try again." << endl << endl);

    char choice = buffer[0];
    delete[] buffer;

    return choice;
}

void get_name(char* name)
{
    do
    {
        cout << "name: ";
        cin >> name;
    } while (!(is_name_ok(name))
        && cout << "wrong format. try again." << endl);
}

void get_num(char* num)
{
    do
    {
        cout << "number: ";
        cin >> num;
    } while (!(is_num_ok(num))
        && cout << "wrong format. try again." << endl);
}

bool is_name_ok(char* name)
{
    for (int i = 0; name[i]; i++)
        if (!((name[i] >= 'a' && name[i] <= 'z')
            || (name[i] >= 'A' && name[i] <= 'Z')
            || name[i] == ' '))
            return false;
    return strlen(name) < max_name_len;
}

bool is_num_ok(char* num)
{
    for (int i = 0; num[i]; i++)
        if (!(num[i] >= '0' && num[i] <= '9'))
            return false;
    return strlen(num) < max_num_len;
}
