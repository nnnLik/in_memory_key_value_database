#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MAX_COMMAND_LENGTH 50
#define MAX_KEY_LENGTH 100
#define MAX_VALUE_LENGTH 1024
#define MAX_DATA_ENTRIES 1024

typedef enum
{
    INT_TYPE,
    STR_TYPE
} DataType;

typedef union
{
    int intValue;
    char strValue[MAX_VALUE_LENGTH];
} DataValue;

typedef struct
{
    char key[MAX_KEY_LENGTH];
    DataType type;
    DataValue value;
} DataEntry;

typedef struct
{
    DataEntry entries[MAX_DATA_ENTRIES];
    int count;
} GlobalStorage;

GlobalStorage globalStorage;

int _get(const GlobalStorage *storage, const char *key);

char *aget(const GlobalStorage *storage);

void get_data(const GlobalStorage *storage, const char *key);
void insert_data(GlobalStorage *storage, const char *key, DataType type, const char *value);
void delete_data(GlobalStorage *storage, const char *key);

void cleanup(GlobalStorage *storage);

void show_help_msg(void)
{
    printf("Command Format:\n");
    printf("  > set {!type} {!key} {!value} - Insert data.\n");
    printf("  > get {!key} - Retrieves data using the specified key.\n\n");

    printf("Command Components:\n");
    printf("  {!type} - Type of data (e.g., string, int, etc.).\n");
    printf("  {!key}  - Unique identifier for the data.\n");
    printf("  {!value} - Value associated with the key.\n\n");

    printf("Note:\n");
    printf("  ! - Indicates that the argument is required.\n");
}

void insert_data(GlobalStorage *storage, const char *key, DataType type, const char *value)
{
    if (storage->count < MAX_DATA_ENTRIES)
    {
        int is_key_already_exists = _get(storage, key);

        if (is_key_already_exists)
        {
            fprintf(stderr, "Key already exists in database.\n");
        }

        strcpy(storage->entries[storage->count].key, key);
        storage->entries[storage->count].type = type;

        if (type == INT_TYPE)
        {
            storage->entries[storage->count].value.intValue = atoi(value);
        }
        else
        {
            strcpy(storage->entries[storage->count].value.strValue, value);
        }

        storage->count++;
    }
    else
    {
        fprintf(stderr, "Storage is full. Cannot insert more data.\n");
    }
}

int _get(const GlobalStorage *storage, const char *key)
{
    for (int i = 0; i < storage->count; i++)
    {
        if (strcmp(storage->entries[i].key, key) == 0)
        {
            if (storage->entries[i].type == INT_TYPE)
            {
                printf("%d\n", storage->entries[i].value.intValue);
                return 1;
            }
            else
            {
                printf("%s\n", storage->entries[i].value.strValue);
                return 1;
            }
            return 0;
        }
    }
}

void get_data(const GlobalStorage *storage, const char *key)
{
    int result = _get(storage, key);
    if (!result)
    {
        fprintf(stderr, "Key '%s' not found in the storage.\n", key);
    }
    else
    {
        return;
    }
}

void delete_data(GlobalStorage *storage, const char *key)
{
    int index = -1;
    for (int i = 0; i < storage->count; i++)
    {
        if (strcmp(storage->entries[i].key, key) == 0)
        {
            index = i;
            break;
        }
    }

    if (index != -1)
    {
        if (storage->entries[index].type == STR_TYPE)
        {
            free(storage->entries[index].value.strValue);
        }

        if (index != storage->count - 1)
        {
            memcpy(&storage->entries[index], &storage->entries[storage->count - 1], sizeof(DataEntry));
        }
        storage->count--;
    }
    else
    {
        fprintf(stderr, "Key '%s' not found in the storage.\n", key);
    }
}

char *aget(const GlobalStorage *storage)
{
    if (storage->count == 0)
    {
        return NULL;
    }

    size_t buffer_size = 0;

    for (int i = 0; i < storage->count; i++)
    {
        buffer_size += strlen(storage->entries[i].key) + 1 + 8 + 1;

        if (storage->entries[i].type == INT_TYPE)
        {
            buffer_size += snprintf(NULL, 0, "%d", storage->entries[i].value.intValue) + 1;
        }
        else
        {
            buffer_size += strlen(storage->entries[i].value.strValue) + 1;
        }
    }

    char *result = malloc(buffer_size);
    if (result == NULL)
    {
        fprintf(stderr, "Memory allocation error.\n");
        exit(1);
    }

    sprintf(result, "| %-8s | %-8s | %-8s |\n", "name", "type", "value");

    char *current_position = result + strlen(result);
    for (int i = 0; i < storage->count; i++)
    {
        sprintf(
            current_position,
            "| %-8s | %-8s | ",
            storage->entries[i].key, (storage->entries[i].type == INT_TYPE) ? "int" : "str");
        current_position += strlen(current_position);

        if (storage->entries[i].type == INT_TYPE)
        {
            sprintf(current_position, "%-8d |\n", storage->entries[i].value.intValue);
        }
        else
        {
            sprintf(current_position, "%-8s |\n", storage->entries[i].value.strValue);
        }
        current_position += strlen(current_position);
    }

    return result;
}

void cleanup(GlobalStorage *storage)
{
    for (int i = 0; i < storage->count; i++)
    {
        if (storage->entries[i].type == STR_TYPE)
        {
            free(storage->entries[i].value.strValue);
        }
    }
    memset(storage, 0, sizeof(GlobalStorage));
}

int main(void)
{
    printf("----------------------------------------------------\n");
    printf("To see all possible command use 'help' command\n");
    printf("----------------------------------------------------\n\n");

    while (1)
    {
        char command[MAX_COMMAND_LENGTH];

        if (fgets(command, sizeof(command), stdin) == NULL)
        {
            fprintf(stderr, "Error reading command\n");
            return 1;
        }

        if (strlen(command) > 0 && command[strlen(command) - 1] == '\n')
        {
            command[strlen(command) - 1] = '\0';
        }
        else
        {
            fprintf(stderr, "Command is too long\n");
            continue;
        }

        char *token = strtok(command, " ");

        if (token == NULL)
        {
            fprintf(stderr, "Invalid command\n");
            continue;
        }

        if (strcmp(command, "help") == 0)
        {
            show_help_msg();
        }
        else if ((strcmp(command, "clear") == 0) || (strcmp(command, "c") == 0))
        {
            printf("\e[1;1H\e[2J");
        }
        else if (strcmp(token, "set") == 0)
        {
            char *type = strtok(NULL, " ");
            char *key = strtok(NULL, " ");
            char *value = strtok(NULL, "");

            if (type == NULL || key == NULL || value == NULL)
            {
                fprintf(stderr, "Invalid set command. Usage: set {!type} {!key} {!value}\n");
            }
            else
            {
                DataType dataType;
                if (strcmp(type, "int") == 0)
                {
                    dataType = INT_TYPE;
                }
                else if (strcmp(type, "str") == 0)
                {
                    dataType = STR_TYPE;
                }
                else
                {
                    fprintf(stderr, "Invalid data type. Use 'int' or 'str'.\n");
                    continue;
                }

                insert_data(&globalStorage, key, dataType, value);
            }
        }
        else if (strcmp(token, "get") == 0)
        {
            char *key = strtok(NULL, "");

            if (key == NULL)
            {
                fprintf(stderr, "Invalid get command. Usage: get {!key}\n");
            }
            else
            {
                get_data(&globalStorage, key);
            }
        }
        else if (strcmp(token, "delete") == 0)
        {
            char *key = strtok(NULL, "");

            if (key == NULL)
            {
                fprintf(stderr, "Invalid delete command. Usage: delete {!key}\n");
            }
            else
            {
                delete_data(&globalStorage, key);
            }
        }
        else if (strcmp(token, "aget") == 0)
        {
            char *all_data = aget(&globalStorage);
            if (all_data != NULL)
            {
                printf("%s\n", all_data);
                free(all_data);
            }
            else
            {
                printf("No data available.\n");
            }
        }
        else if (strcmp(token, "exit") == 0)
        {
            cleanup(&globalStorage);
            return 0;
        }
        else
        {
            printf("Unknown command: %s\n", command);
        }
    }

    return 0;
}
