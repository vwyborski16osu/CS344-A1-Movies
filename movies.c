#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_NUM_LANG 5
#define MAX_LANG_LENGTH 20
struct movie
{
    char *title;
    int year;
    char languages[MAX_NUM_LANG][MAX_LANG_LENGTH+1];
    int language_count;
    double rating;
    struct movie *next;
};

/*
 * Function:  print_movie 
 * --------------------
 * Prints all information about a movie. Used in debugging.
 * p_movie: pointer to a struct movie
 */
void print_movie(struct movie *p_movie)
{
    printf("Title: %s\n", p_movie->title);
    printf("Year:%i\n", p_movie->year);
    printf("Languages:");
    int lang_array_counter = 0;
    while(lang_array_counter < p_movie->language_count){
        printf("%s;", (p_movie->languages[lang_array_counter]));
        lang_array_counter++;
    }
    printf("\n");
    printf("Rating: %.1f\n", p_movie->rating);
}

/*
 * Function:  createMovie
 * --------------------
 * Creates a struct movie, returns a pointer. Initializes all values based on information on a line of the given csv.
 * curr_line: pointer to a string, specifically a line in the given csv file of format "title, year, [lang1;lang2], rating"
 * 
 * returns: struct movie pointer to newly created movie
 */
struct movie *createMovie(char *curr_line)
{
    struct movie *curr_movie = malloc(sizeof(struct movie));
    curr_movie->language_count = 1;

    // For use with strtok_r
    char *saveptr;
    char *saveptr_languages;
    char *language_string; 
    int lang_num_counter = 1;

    // The first token is the title
    char *token = strtok_r(curr_line, ",", &saveptr);
    curr_movie->title = calloc(strlen(token) + 1, sizeof(char));
    strcpy(curr_movie->title, token);

    // The next token is the year
    token = strtok_r(NULL, ",", &saveptr);
    curr_movie->year = atoi(token);
    
    // The next token is the languages. Get first token which will encompass [lang_1,....,lang_n] where n <= 5
    // Strip leading [ off of languages by advancing ptr one byte
    *saveptr++; 
    token = strtok_r(NULL, ",", &saveptr);
    language_string = calloc(strlen(token) + 1, sizeof(char));
    strcpy(language_string, token);
    // Parse languages
    // Use new saveptr for languages, initialized to the previous save ptr + 1 character to strip leading [
    // Place each language into languages 2-d char array, using ] as a delim to strip trailing ].
    saveptr_languages = saveptr;
    char *sub_token = strtok_r(language_string, ";]", &saveptr_languages);
    strcpy(curr_movie->languages[0], sub_token);
    // Iterate through sub_tokens until null, count languages to correctly place in languages array
    while(sub_token != NULL)
    {
        sub_token = strtok_r(NULL, ";]", &saveptr_languages);
        if (sub_token != NULL)
        {
            strcpy(curr_movie->languages[lang_num_counter], sub_token);
            curr_movie->language_count++;
        }
        lang_num_counter++;
    }

    // Continue with original tokens and saveptr. The next token is the rating
    token = strtok_r(NULL, ",", &saveptr);
    curr_movie->rating = strtod(token, NULL);

    // Set the next node to NULL in the newly created student entry
    curr_movie->next = NULL;

    // print_movie(curr_movie);
    
    return curr_movie;
}

/*
 * Function:  processFile
 * --------------------
 * Creates a linked list of struct movie's, from the given csv file.
 * 
 * file_path: pointer to a string, which is a .csv file of movies given by the user as an argument.
 * 
 * returns: struct movie linked list, specifically returns pointer to list head
 */
struct movie *processFile(char *file_path)
{   
    // Open the specified file for reading only
    FILE *movie_file = fopen(file_path, "r");

    char *curr_line = NULL;
    size_t len = 0;
    ssize_t nread;
    int movie_count = 0;

    // The head of the linked list
    struct movie *head = NULL;
    // The tail of the linked list
    struct movie *tail = NULL;

    // Read the file line by line, ignore first line
    getline(&curr_line, &len, movie_file);
    while (( nread = getline(&curr_line, &len, movie_file)) != -1 )
    {
        // create a new movie node corresponding to the current line
        struct movie *new_movie = createMovie(curr_line);
        movie_count++;

        // Is this the first node in the linked list?
        if (head == NULL)
        {
            //Set the ehad and tail to this node
            head = new_movie;
            tail = new_movie;
        }
        else
        {
            // This is not the first node. Add node to the list and advance tail
            tail->next = new_movie;
            tail = new_movie;
        }
    }

    free(curr_line);
    fclose(movie_file);
    printf("Processed file %s and parsed data for %i movies.\n\n", file_path, movie_count);
    return head;
}

/*
 * Function:  menu_selection
 * --------------------
 * Takes user input in form of int, validates that it is in range [1,4], returns int
 * 
 * returns: int inputted by user for menu choice
 */
int menu_selection()
{
    int user_input_valid = 0;
    int num_selection;
    while(user_input_valid == 0)
    {
        printf("1. Show movies released in the specified year\n");
        printf("2. Show highest rated movie for each year\n");
        printf("3. Show the title and year of release of all movies in a specific language\n");
        printf("4. Exit from the program\n\n");
        printf("Enter a choice from 1 to 4: ");
        scanf("%d", &num_selection);
        if((0 < num_selection) & (num_selection < 5))
        {
            user_input_valid = 1;
        }
        else
        {
            printf("You entered an incorrect choice. Try again.\n\n");
        }
    }

    return num_selection;
}

/*
 * Function:  movies_in_year
 * --------------------
 * Prompts user for year input, prints a list of movies which occurred in the year.
 * 
 * movie_list_head: struct movie pointer to the head of the linked list of movies.
 */
void movies_in_year(struct movie *movie_list_head)
{
    int year_selection;
    int movie_found = 0;
    printf("Enter the year for which you want to see movies: ");
    scanf("%d", &year_selection);

    struct movie *curr_movie = movie_list_head;
    while(curr_movie != NULL){
        if(curr_movie->year == year_selection)
        {
            printf("%s\n", curr_movie->title);
            movie_found = 1;
        }
        curr_movie = curr_movie->next;
    }
    if(movie_found == 1)
    {
        printf("\n");
    }
    else
    {
        printf("No data about movies released in the year %i\n\n", year_selection);
    }
    return;
}

/*
 * Function:  highest_rated
 * --------------------
 * Prints a list of highest rated movies in each year.
 * 
 * movie_list_head: struct movie pointer to the head of the linked list of movies.
 */
void highest_rated(struct movie *movie_list_head)
{
    struct movie *curr_movie;
    struct movie *highest_rated;
    double highest_rating;
    for(int year = 1990; year <= 2021; year++)
    {
        curr_movie = movie_list_head;
        highest_rated = NULL;
        highest_rating = 0;
        while(curr_movie != NULL)
        {
            if(curr_movie->year == year)
            {
                if(curr_movie->rating > highest_rating)
                {
                    highest_rated = curr_movie;
                    highest_rating = highest_rated->rating;
                    // printf("******* curr_movie which is highest rated: %s\n", highest_rated->title);
                }
            }
            curr_movie = curr_movie->next;
        }
        if(highest_rated != NULL)
        {
            printf("%i %.1f %s\n", highest_rated->year, highest_rated->rating, highest_rated->title);
        }
    }
    printf("\n");
    return;
}

/*
 * Function:  language_movies
 * --------------------
 * Prints a list which are made in the 
 * 
 * movie_list_head: struct movie pointer to the head of the linked list of movies.
 */
void language_movies(struct movie *movie_list_head)
{
    char language_selected[21];
    printf("Enter the language for which you want to see movies: ");
    scanf("%s", language_selected);
    struct movie *curr_movie = movie_list_head;
    int language_counter;
    int one_movie_found = 0;
    while(curr_movie != NULL)
    {
        language_counter = 0;
        while(language_counter <= curr_movie->language_count)
        {
            if(0 == strcmp(language_selected, curr_movie->languages[language_counter]))
            {
                printf("%d %s\n", curr_movie->year, curr_movie->title);
                one_movie_found = 1;
                break;
            }
            language_counter++;
        }
        curr_movie = curr_movie->next;
    }
    if(one_movie_found == 0)
    {
        printf("No data about movies released in %s\n\n", language_selected);
    }
    else{
        printf("\n");
    }
    return;
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        printf("You must provide the name of the file to process\n");
        printf("Example usage: ./movies movies.csv\n");
        return EXIT_FAILURE;
    }
    struct movie* movie_list_head = processFile(argv[1]);
    int information_case=0;
    while(information_case != 4)
    {
        information_case = menu_selection(movie_list_head);
        switch(information_case){
        case 1:
            movies_in_year(movie_list_head);
            break;
        case 2:
            highest_rated(movie_list_head);
            break;
        case 3:
            language_movies(movie_list_head);
            break;
        case 4:
            break;
        }
    }
    return EXIT_SUCCESS;
}