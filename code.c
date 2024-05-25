#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_NAME_LENGTH 256

struct GraphNode {
    char name[MAX_NAME_LENGTH];
    int is_directory;
    int size;
    time_t creation_time;
    struct GraphNode *next_sibling;
    struct GraphNode *children;
    struct GraphNode *parent;
};

struct Graph {
    struct GraphNode *root;
    struct GraphNode *current_directory;
};

struct GraphNode *createGraphNode(const char *name, int is_directory) {
    struct GraphNode *newNode = malloc(sizeof(struct GraphNode));
    strncpy(newNode->name, name, MAX_NAME_LENGTH);
    newNode->name[MAX_NAME_LENGTH - 1] = '\0';
    newNode->is_directory = is_directory;
    newNode->size = is_directory ? 0 : rand() % 1024;
    newNode->creation_time = time(NULL);
    newNode->next_sibling = NULL;
    newNode->children = NULL;
    newNode->parent = NULL;

    return newNode;
}

void addChildToGraph(struct GraphNode *parent, struct GraphNode *child) {
    child->next_sibling = parent->children;
    child->parent = parent;
    parent->children = child;
}

void listContentsInGraph(struct GraphNode *node, int depth) {
    printf("Contents of %s:\n", node->name);

    struct GraphNode *current = node->children;
    while (current != NULL) {
        for (int i = 0; i < depth; ++i) {
            printf("  ");
        }
        printf("%s %s (Size: %d, Created: %s)",
            current->is_directory ? "D" : "F",
            current->name,
            current->size,
            ctime(&current->creation_time));

        if (current->is_directory) {
            listContentsInGraph(current, depth + 1);
        }

        current = current->next_sibling;
    }
}

void displayFileMetadataInGraph(struct GraphNode *node) {
    printf("Metadata of files in %s:\n", node->name);

    struct GraphNode *current = node->children;
    while (current != NULL) {
        if (!current->is_directory) {
            printf("  %s (Size: %d, Created: %s)\n",
                current->name,
                current->size,
                ctime(&current->creation_time));
        }

        current = current->next_sibling;
    }
}

struct GraphNode *changeDirectoryInGraph(struct Graph *graph, const char *path) {
    if (strcmp(path, "..") == 0) {
        if (graph->current_directory->parent != NULL) {
            return graph->current_directory->parent;
        } else {
            printf("Already in the root directory.\n");
            return graph->current_directory;
        }
    }

    char *token = strtok((char *)path, "/");
    struct GraphNode *current = graph->current_directory->children;

    while (token != NULL) {
        while (current != NULL) {
            if (strcmp(current->name, token) == 0 && current->is_directory) {
                break;
            }
            current = current->next_sibling;
        }

        if (current != NULL) {
            token = strtok(NULL, "/");
            if (token != NULL) {
                current = current->children;
            }
        } else {
            printf("Directory '%s' not found.\n", token);
            return graph->current_directory;
        }
    }

    return current;
}

void createDirectoryInGraph(struct Graph *graph, const char *dirName) {
    struct GraphNode *newDir = createGraphNode(dirName, 1);
    addChildToGraph(graph->current_directory, newDir);
    printf("Directory '%s' created successfully.\n", dirName);
}

void createFileInGraph(struct Graph *graph, const char *fileName) {
    struct GraphNode *newFile = createGraphNode(fileName, 0);
    addChildToGraph(graph->current_directory, newFile);
    printf("File '%s' created successfully.\n", fileName);
}

int main() {
    struct Graph graph;
    graph.root = createGraphNode("/", 1);
    graph.current_directory = graph.root;

    while (1) {
        printf("\nCurrent Directory: %s\n", graph.current_directory->name);
        printf("\nMenu:\n");
        printf("1. List Contents\n");
        printf("2. Change Directory\n");
        printf("3. Create File\n");
        printf("4. Create Directory\n");
        printf("5. Display File Metadata\n");
        printf("6. Exit\n");
        char input[MAX_NAME_LENGTH];
        printf("Enter option (1-6): ");
        fgets(input, sizeof(input), stdin);
        switch (atoi(input)) {
            case 1:
                listContentsInGraph(graph.current_directory, 0);
                break;
            case 2:
                printf("Enter directory path: ");
                fgets(input, sizeof(input), stdin);
                input[strcspn(input, "\n")] = 0;
                graph.current_directory = changeDirectoryInGraph(&graph, input);
                break;
            case 3:
                printf("Enter new file name: ");
                fgets(input, sizeof(input), stdin);
                input[strcspn(input, "\n")] = 0;
                createFileInGraph(&graph, input);
                break;
            case 4:
                printf("Enter new directory name: ");
                fgets(input, sizeof(input), stdin);
                input[strcspn(input, "\n")] = 0;
                createDirectoryInGraph(&graph, input);
                break;
            case 5:
                displayFileMetadataInGraph(graph.current_directory);
                break;
            case 6:
                printf("Exiting...\n");
                return 0;
            default:
                printf("Invalid option. Please enter a number between 1 and 6.\n");
                break;
        }
    }
    return 0;
}