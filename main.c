#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MAX_COMMAND_LENGTH 50
#define MAX_KEY_LENGTH 100
#define MAX_VALUE_LENGTH 1024
#define MAX_DATA_ENTRIES 1024

typedef enum {
    INT_TYPE,
    STR_TYPE
} DataType;

typedef union {
    int intValue;
    char strValue[MAX_VALUE_LENGTH];
} DataValue;

typedef struct {
    char key[MAX_KEY_LENGTH];
    DataType type;
    DataValue value;
} DataEntry;

typedef struct {
    DataEntry entries[MAX_DATA_ENTRIES];
    int count;
} GlobalStorage;

GlobalStorage globalStorage;

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

void insert_data(GlobalStorage *storage, const char *key, DataType type, const char *value) {
    if (storage->count < MAX_DATA_ENTRIES) {
        strcpy(storage->entries[storage->count].key, key);
        storage->entries[storage->count].type = type;

        if (type == INT_TYPE) {
            storage->entries[storage->count].value.intValue = atoi(value);
        } else {
            strcpy(storage->entries[storage->count].value.strValue, value);
        }

        storage->count++;
        printf("Data inserted successfully.\n");
    } else {
        fprintf(stderr, "Storage is full. Cannot insert more data.\n");
    }
}

void get_data(const GlobalStorage *storage, const char *key) {
    for (int i = 0; i < storage->count; i++) {
        if (strcmp(storage->entries[i].key, key) == 0) {
            if (storage->entries[i].type == INT_TYPE) {
                printf("Value for key '%s': %d\n", key, storage->entries[i].value.intValue);
            } else {
                printf("Value for key '%s': %s\n", key, storage->entries[i].value.strValue);
            }
            return;
        }
    }
    fprintf(stderr, "Key '%s' not found in the storage.\n", key);
}

int main(void)
{
    printf("----------------------------------------------------\n");
    printf("To see all possible command use 'help' command\n");
    printf("----------------------------------------------------\n\n");

    while (1)
    {
        char command[MAX_COMMAND_LENGTH];

        if (fgets(command, sizeof(command), stdin) == NULL) {
            fprintf(stderr, "Error reading command\n");
            return 1;
        }

        if (strlen(command) > 0 && command[strlen(command) - 1] == '\n') {
            command[strlen(command) - 1] = '\0';
        } else {
            fprintf(stderr, "Command is too long\n");
            continue;
        }
        
        char *token = strtok(command, " ");

        if (token == NULL) {
            fprintf(stderr, "Invalid command\n");
            continue;
        }

        if (strcmp(command, "help") == 0) {
            show_help_msg();
        } else if ((strcmp(command, "clear") == 0) || (strcmp(command, "c") == 0)) {
            printf("\e[1;1H\e[2J");
        } else if (strcmp(token, "set") == 0) {
            char *type = strtok(NULL, " ");
            char *key = strtok(NULL, " ");
            char *value = strtok(NULL, "");

            if (type == NULL || key == NULL || value == NULL) {
                fprintf(stderr, "Invalid set command. Usage: set {!type} {!key} {!value}\n");
            } else {
                DataType dataType;
                if (strcmp(type, "int") == 0) {
                    dataType = INT_TYPE;
                } else if (strcmp(type, "str") == 0) {
                    dataType = STR_TYPE;
                } else {
                    fprintf(stderr, "Invalid data type. Use 'int' or 'str'.\n");
                    continue;
                }

                insert_data(&globalStorage, key, dataType, value);
            }
        } else if (strcmp(token, "get") == 0) {
            char *key = strtok(NULL, "");

            if (key == NULL) {
                fprintf(stderr, "Invalid get command. Usage: get {!key}\n");
            } else {
                get_data(&globalStorage, key);
            }
        } else {
            printf("Unknown command: %s\n", command);
        }
    }

    return 0;
}
