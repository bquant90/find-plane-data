/*
* find_plane_data.c
* 
* A program that reads aircraft data from a text file and allows users
* to view all plane details or edit plane information.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

// Constants for array sizes
#define MAX_PLANES 10
#define LEN_NAME 50
#define LEN_CRUISE 10
#define LEN_WINGSPAN 20
#define LEN_DESC 100

// Structure to represent aircraft data
typedef struct{
char name[LEN_NAME]; // Name of the plane
char cruise[LEN_CRUISE]; // Cruise speed of the plane
char wingspan[LEN_WINGSPAN]; // Wingspan of the plane
char desc[LEN_DESC]; // Description of the plane
} Plane;

// Function prototypes
int load_plane_data(Plane planes[], const char* filename);
bool read_plane_data(FILE *fp, Plane *plane);
void run_program_menu(Plane planes[], int plane_count, const char* filename);
bool save_plane_data(Plane planes[], int count, const char* filename);
void edit_plane(Plane planes[], int count);
void print_all_planes(Plane planes[], int count);
void print_plane_details(const Plane* plane);
bool ask_edit_input(const char* question, const char* prompt, char* buffer, int max_length);
void clear_input_buffer(void);
void trim_string(char* str);
bool get_string_input(const char* prompt, char* buffer, int max_length);

/**
 * Main function to run the aircraft information program.
 * 
 * Returns:
 *     int: 0 for successful execution
 */
int main(void) {
    Plane planes[MAX_PLANES];
    int plane_count;
    const char* filename = "plane_data.txt";

    printf("Aircraft Information System\n");
    printf("------------------------------\n");

    // Load plane data from file
    plane_count = load_plane_data(planes, filename);

    if (plane_count == 0) {
        printf("Error: No plane data found or could not open file.\n");
        return 1;
    }

    printf("Successfully loaded data for %d aircraft.\n\n", plane_count);

    // Run the main program menu
    run_program_menu(planes, plane_count, filename);

    return 0;
}

/**
 * Load plane data from a text file.
 * 
 * Parameters:
 *     planes[]: Array to store plane data
 *     filename: Name of the text file to read
 * 
 * Returns:
 *     int: Number of planes loaded
 */
int load_plane_data(Plane planes[], const char* filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Error: Could not open file %s\n", filename);
        return 0;
    }
    
    int plane_count = 0;
    
    // Read planes until we reach EOF or MAX_PLANES
    while (plane_count < MAX_PLANES) {
        if (read_plane_data(fp, &planes[plane_count])) {
            plane_count++;
        }
        else {
            // Exit once no more plane data can be read in
            break;
        }
    }
    
    fclose(fp);
    
    // Trim whitespace from all strings
    for (int i = 0; i < plane_count; i++) {
        trim_string(planes[i].name);
        trim_string(planes[i].cruise);
        trim_string(planes[i].wingspan);
        trim_string(planes[i].desc);
    }
    
    return plane_count;
}

/**
 * Read data for a single plane from the file.
 *
 * Parameters:
 *     fp: File pointer to read from
 *     plane: Pointer to the Plane structure to fill
 *
 * Returns:
 *     bool: True if plane data was successfully read, false otherwise
 */
bool read_plane_data(FILE *fp, Plane *plane) {
    char buffer[LEN_DESC];

    // Skip any empty lines before reading a plane
    do {
        if (fgets(buffer, sizeof(buffer), fp) == NULL) return false;
        buffer[strcspn(buffer, "\n")] = '\0'; // Remove newline
    } while (strlen(buffer) == 0);

    // Buffer should contain non-empty name line
    strncpy(plane->name, buffer, LEN_NAME - 1);
    plane->name[LEN_NAME - 1] = '\0';

    // Read cruise speed
    if (fgets(buffer, sizeof(buffer), fp) == NULL) return false;
    buffer[strcspn(buffer, "\n")] = '\0';
    strncpy(plane->cruise, buffer, LEN_CRUISE - 1);
    plane->cruise[LEN_CRUISE - 1] = '\0';
    
    // Read wingspan
    if (fgets(buffer, sizeof(buffer), fp) == NULL) return false;
    buffer[strcspn(buffer, "\n")] = '\0';
    strncpy(plane->wingspan, buffer, LEN_WINGSPAN - 1);
    plane->wingspan[LEN_WINGSPAN - 1] = '\0';
    
    // Read description
    if (fgets(buffer, sizeof(buffer), fp) == NULL) return false;
    buffer[strcspn(buffer, "\n")] = '\0';
    strncpy(plane->desc, buffer, LEN_DESC - 1);
    plane->desc[LEN_DESC - 1] = '\0';

    return true;
}

/**
 * Run the main program menu loop.
 * 
 * Parameters:
 *     planes[]: Array of plane data
 *     plane_count: Number of planes in the array
 *     filename: Name of the file to save changes to
 */
void run_program_menu(Plane planes[], int plane_count, const char* filename) {
    int choice;
    bool running = true;

    while (running) {
        printf("MENU\n");
        printf("-----\n");
        printf("1. Display all planes\n");
        printf("2. Edit plane information\n");
        printf("3. Quit\n");
        printf("\n");
        printf("Enter your choice (1-3): ");
        
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n\n");
            clear_input_buffer();
            continue;
        }
        
        clear_input_buffer();
        
        switch(choice) {
            case 1:
                print_all_planes(planes, plane_count);
                break;
                
            case 2:
                edit_plane(planes, plane_count);
                // Save changes to file after editing
                if (save_plane_data(planes, plane_count, filename)) {
                    printf("Changes saved successfully.\n\n");
                }
                else {
                    printf("Error: Could not save changes to file.\n\n");
                }
                break;
            
            case 3:
                running = false;
                printf("\n");
                printf("Thank you for using the Aircraft Information System. Goodbye!\n");
                break;
            
            default:
                printf("Invalid choice. Please enter a number between 1 and 3.\n\n");
                break;
        }
    }
}


/**
 * Save plane data to a text file.
 * 
 * Parameters:
 *     planes[]: Array of plane data
 *     count: Number of planes in the array
 *     filename: Name of the text file to write
 * 
 * Returns:
 *     bool: True if save was successful, false otherwise
 */
bool save_plane_data(Plane planes[], int count, const char* filename) {
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        return false;
    }
    
    for (int i = 0; i < count; i++) {
        // Write each field followed by a newline
        fprintf(fp, "%s\n", planes[i].name);
        fprintf(fp, "%s\n", planes[i].cruise);
        fprintf(fp, "%s\n", planes[i].wingspan);
        
        // For the last field of the last plane, don't add a newline at the end
        if (i == count - 1) {
            fprintf(fp, "%s", planes[i].desc);
        }
        else {
            fprintf(fp, "%s\n", planes[i].desc);
            fprintf(fp, "\n");
        }
    }
    
    fclose(fp);
    return true;
}

/**
 * Edit plane information.
 * 
 * Parameters:
 *     planes[]: Array of plane data
 *     count: Number of planes in the array
 */
void edit_plane(Plane planes[], int count) {
    int choice;
    char buffer[LEN_DESC];
    
    // Display all planes with indices
    printf("\n");
    printf("Select a plane to edit:\n");
    printf("-------------------------\n");
    
    for (int i = 0; i < count; i++) {
        printf("%d. %s\n", i + 1, planes[i].name);
    }
    
    // Get user selection
    while (true) {
        printf("\n");
        printf("Enter plane number (1-%d): ", count);
        if (scanf("%d", &choice) != 1 || choice < 1 || choice > count) {
            printf("Invalid selection. Please enter a number between 1 and %d.\n", count);
            clear_input_buffer();
            continue;
        }
        clear_input_buffer();
        break;
    }
    
    // Adjust for zero-based indexing
    int index = choice - 1;
    
    // Display current information
    printf("\n");
    printf("Editing plane: %s\n", planes[index].name);
    printf("Current information:\n");
    printf("----------------------------------\n");
    print_plane_details(&planes[index]);
    
    // Edit name
    if (ask_edit_input("Do you want to edit the name?", "Enter new name: ", buffer, LEN_NAME)) {
        strncpy(planes[index].name, buffer, LEN_NAME - 1);
        planes[index].name[LEN_NAME - 1] = '\0';
    }
    
    // Edit cruise speed
    if (ask_edit_input("Do you want to edit the cruise speed?", "Enter new cruise speed: ", buffer, LEN_CRUISE)) {
        strncpy(planes[index].cruise, buffer, LEN_CRUISE - 1);
        planes[index].cruise[LEN_CRUISE - 1] = '\0';
    }
    
    // Edit wingspan
    if (ask_edit_input("Do you want to edit the wingspan?", "Enter new wingspan: ", buffer, LEN_WINGSPAN)) {
        strncpy(planes[index].wingspan, buffer, LEN_WINGSPAN - 1);
        planes[index].wingspan[LEN_WINGSPAN - 1] = '\0';
    }
    
    // Edit description
    if (ask_edit_input("Do you want to edit the description?", "Enter new description: ", buffer, LEN_DESC)) {
        strncpy(planes[index].desc, buffer, LEN_DESC - 1);
        planes[index].desc[LEN_DESC - 1] = '\0';
    }
    
    printf("\n");
    printf("Updated information:\n");
    printf("----------------------------------\n");
    print_plane_details(&planes[index]);
}

/**
 * Print details for all planes.
 * 
 * Parameters:
 *     planes[]: Array of plane data
 *     count: Number of planes in the array
 */
void print_all_planes(Plane planes[], int count) {
    printf("\n");
    printf("All Aircraft Information\n");
    printf("----------------------------------\n");
    
    for (int i = 0; i < count; i++) {
        print_plane_details(&planes[i]);
    }
    
    printf("\n");
}

/**
 * Print detailed information for a single plane.
 * 
 * Parameters:
 *     plane: Pointer to the plane structure
 */
void print_plane_details(const Plane* plane) {
    printf("Name:     %s\n", plane->name);
    printf("Speed:    %s mph\n", plane->cruise);
    printf("Wingspan: %s\n", plane->wingspan);
    printf("Type:     %s\n", plane->desc);
    printf("----------------------------------\n");
}

/**
 * Ask a yes/no edit question and optionally get string input if answer is yes.
 * 
 * Parameters:
 *     question: The yes/no edit question to ask
 *     prompt: The prompt for string input if answer is yes
 *     buffer: Buffer to store the string input
 *     max_length: Maximum length of the string input
 * 
 * Returns:
 *     bool: True if user answered yes and input was successful, false otherwise
 */
bool ask_edit_input(const char* question, const char* prompt, char* buffer, int max_length) {
    char response;
    bool valid_response = false;
    
    // Get valid Y/N response
    while (!valid_response) {
        printf("\n");
        printf("%s (Y/N): ", question);
        scanf(" %c", &response);
        clear_input_buffer();
        
        response = toupper(response);
        if (response == 'Y' || response == 'N') {
            valid_response = true;
        } 
        else {
            printf("Invalid input. Please enter Y or N.\n");
        }
    }
    
    // If yes, get string input
    if (response == 'Y') {
        return get_string_input(prompt, buffer, max_length);
    }
    
    return false;
}

/**
 * Clear the input buffer.
 */
void clear_input_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/**
 * Trim leading and trailing whitespace from a string.
 * 
 * Parameters:
 *     str: String to trim
 */
void trim_string(char* str) {
    if (str == NULL) return;
    
    // Trim trailing space
    int len = strlen(str);
    while (len > 0 && isspace((unsigned char)str[len - 1])) {
        str[--len] = '\0';
    }
    
    // Trim leading space
    char* start = str;
    while (*start && isspace((unsigned char)*start)) {
        start++;
    }
    
    // Shift if needed
    if (start != str) {
        memmove(str, start, strlen(start) + 1);
    }
}
  
/**
 * Get a string input from the user.
 * 
 * Parameters:
 *     prompt: The prompt to display
 *     buffer: Buffer to store the input
 *     max_length: Maximum length of the input
 * 
 * Returns:
 *     bool: True if input was successful, false otherwise
 */
bool get_string_input(const char* prompt, char* buffer, int max_length) {
    printf("%s", prompt);
    
    if (fgets(buffer, max_length, stdin) == NULL) {
        printf("Error reading input.\n");
        return false;
    }
    
    // Remove newline character if present
    buffer[strcspn(buffer, "\n")] = '\0';
    trim_string(buffer);
    
    return true;
}
  
  
 
 