/*******************GLOBAL INCLUDES*******************/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <search.h>
#include <sys/types.h>
#include "card.h"

/*******************MACRO DEFINITIONS*******************/
#define _GNU_SOURCE


/**********************GLOBAL FUNCTION PROTOTYPES***************************/

// Function to parse a line from abilities.csv and return an ABILITY_T*
ABILITY_T* parse_ability(char * buf); 

// Function that parses a line from pokemon.csv and returns a POKEMON_T*
POKEMON_T* parse_pokemon(char * buf, ABILITY_T** abilities, size_t total_abilities); 

// Function to write the abilities to the abilities file
void write_abilities(ABILITY_T** Abilities, size_t total_abilities);

// Function to write the pokemons to the pokemon file
INDEX_T** write_pokemons(POKEMON_T** Pokedex, size_t total_pokemons);

// Function to write the indexes to the indexes file
void write_indexes(INDEX_T** Index, size_t total_pokemons);

// Function to convert type string to Type enum
Type get_type(const char* typeStr) {
    if (strcmp(typeStr, "Normal") == 0) return NORMAL;
    if (strcmp(typeStr, "Fire") == 0) return FIRE;
    if (strcmp(typeStr, "Water") == 0) return WATER;
    if (strcmp(typeStr, "Electric") == 0) return ELECTRIC;
    if (strcmp(typeStr, "Grass") == 0) return GRASS;
    if (strcmp(typeStr, "Ice") == 0) return ICE;
    if (strcmp(typeStr, "Fighting") == 0) return FIGHTING;
    if (strcmp(typeStr, "Poison") == 0) return POISON;
    if (strcmp(typeStr, "Ground") == 0) return GROUND;
    if (strcmp(typeStr, "Flying") == 0) return FLYING;
    if (strcmp(typeStr, "Psychic") == 0) return PSYCHIC;
    if (strcmp(typeStr, "Bug") == 0) return BUG;
    if (strcmp(typeStr, "Rock") == 0) return ROCK;
    if (strcmp(typeStr, "Ghost") == 0) return GHOST;
    if (strcmp(typeStr, "Dragon") == 0) return DRAGON;
    if (strcmp(typeStr, "Dark") == 0) return DARK;
    if (strcmp(typeStr, "Steel") == 0) return STEEL;
    if (strcmp(typeStr, "Fairy") == 0) return FAIRY;
    return NONE; // Default case if no match is found
}

/**********************GLOBAL VARIABLE DECLARATIONS***************************/

// Array of pointers to ABILITY_T called Abilities
ABILITY_T** Abilities = NULL; 

// Total number of abilities in the array above
size_t total_abilities;

// Array of pointers to POKEMON_T called Pokedex
POKEMON_T** Pokedex = NULL; 

// Total number of pokemons in the array above
size_t total_pokemons;

// Array of pointers to INDEX_T called indexes
INDEX_T** indexes = NULL;

// File pointers for abilities binary file
FILE* abilitiesFile = NULL;

// File pointers for pokemon binary file
FILE* pokemonFile = NULL;

// File pointers for indexes binary file
FILE* indexesFile = NULL;


/**********************FUNCTION DEFINITIONS***************************/

// This is used when sorting the abilities by name
int sort_abilities(const void *a, const void *b) {
    
    // Casting 'a' from const void* argument to ABILITY_T*
    ABILITY_T* ability_a = *((ABILITY_T**)a);

    // Casting 'b' from const void* argument to ABILITY_T*
	ABILITY_T *ability_b = *((ABILITY_T**)b);
    
    // Compare the names of the abilities
    return strcmp(ability_a->name, ability_b->name);
}


// This is used when sorting the pokemon by name
int sort_pokemon(const void *a, const void *b) {
    
    // Casting 'a' from const void* to POKEMON_T*
    POKEMON_T* pokemon_a = *(POKEMON_T**)a;

    // Casting 'b' from const void* to POKEMON_T*
    POKEMON_T *pokemon_b = *(POKEMON_T**)b; 
    
    // Compare the names of the pokemons
    return strcmp(pokemon_a->name, pokemon_b->name);
}


// This is used when searching for abilities by name
int search_comp(const void *a, const void *b) {

    // Casting 'a' from const void* to char*
    char* ability_name = (char*)a;

    // Casting 'b' from const void* to ABILITY_T*
    ABILITY_T* ability = *(ABILITY_T**)b;

    // Compare the names of the abilities
    return strcmp(ability_name, ability->name);

}

// This is used when sorting the index by name
int sort_index(const void *a, const void *b) {
    
    // Casting 'a' from const void* to INDEX_T*
    INDEX_T* index_a = *(INDEX_T**)a;

    // Casting 'b' from const void* to INDEX_T*
    INDEX_T *index_b = *(INDEX_T**)b; 
    
    // Compare the names of the pokemons
    return strcmp(index_a->name, index_b->name);
}

/**********************MAIN FUNCTION***************************/


int main(int argc, char *argv[]) {

    /********MAIN FUNCTION INITIALIZATIONS********/
    
    // Allocating memory for the pointers to POKEMON_T 
    Pokedex = malloc(sizeof(POKEMON_T*)); 
    
    // Allocating memory foe the pointers to ABILITY_T
    Abilities = malloc(sizeof(ABILITY_T*)); 
    
    // Initializing total_pokemons for the get_line function
    size_t total_pokemons = 0;
    
    // Initializing total_abilities for the get_line function
    size_t total_abilities = 0;
    
    // Initializing the line variable for the get_line function
    char *line = NULL; 
    
    // Initializing the len variable for the length of the line in the csv file
    size_t len = 0;


    
    /**********READING CSV FILES**********/
    
    // Name of the abilities file
    char *abilities_fd = "abilities.csv"; 
   
    // Open the abilities file
    FILE *abilities_file = fopen(abilities_fd, "r"); 

    if (abilities_file == NULL) {
        fprintf(stderr, "Error: Could not open file %s\n", abilities_fd);
        return -1;
    }

    // Read the header of the abilities file to skip it
    getline(&line, &len, abilities_file); 
   
    // Initializing the variable
    size_t get_line = 0;


    // Read the rest of the lines
    while((get_line = getline(&line, &len, abilities_file)) != -1) {  

        // If the line is not empty
        if (get_line > 0){

        
        // Allocate memory for the ABILITY_T* (an ability pointer)
        ABILITY_T** tempAbilities = realloc(Abilities, (total_abilities + 1) * sizeof(ABILITY_T*));
        if (tempAbilities == NULL) 
        {
            fprintf(stderr, "Error: Memory allocation failed\n");
            free(line);

            line = NULL;

            continue;
        } 
        else 
        {
            Abilities = tempAbilities; 
        }
            // Parse the line and store the ABILITY_T* in the array
            Abilities[total_abilities] = parse_ability(line); 

            // Increment the number of abilities
            total_abilities++; 
        
        }
    }

    

    // Write the abilities to the abilities bin file before sorting
    write_abilities(Abilities, total_abilities);
    
    // Sorting the ABILITY_T** array
    //qsort(Abilities, total_abilities, sizeof(ABILITY_T*), sort_abilities); 

    // Name the pokemon file
    char *pokemon_fd = argv[1];

    // Open the pokemon file
    FILE *pokemon_file = fopen(pokemon_fd, "r"); 

    // Read the header of the pokemon.csv to skip it
    getline(&line, &len, pokemon_file);

    // Read the rest of the lines
    while((get_line = getline(&line, &len, pokemon_file)) != -1)
    {
        // If the line is not empty
        if (get_line > 0){
        
            // Allocate memory for the POKEMON_T* (a pokemon pointer)
            POKEMON_T* new_pokemon = NULL; 

            // Parse the line and store the POKEMON_T* in the array
            new_pokemon = parse_pokemon(line, Abilities, total_abilities); 

            // If new_pokemon is not NULL
            if (new_pokemon != NULL) {

                // Allocate memory for the POKEMON_T* (a pokemon pointer)
                POKEMON_T** tempPokedex = realloc(Pokedex, (total_pokemons + 1) * sizeof(POKEMON_T*));

                // Check if memory allocation failed
                if (Pokedex == NULL) {

                    // Print an error message
                    fprintf(stderr, "Error: Memory allocation failed\n");
                    
                    // Return an error code
                    return -3;
                }
                else {
                    Pokedex = tempPokedex; 
                }
                
                // Store the new_pokemon in the array
                Pokedex[total_pokemons] = new_pokemon; 
                
                // Increment the number of pokemons
                total_pokemons++; 
            }
            else {
                // Return an error code
                continue;
            }

            
        }
    }

    // Write the pokemons to the pokemon bin file before sorting
    INDEX_T** indexes = write_pokemons(Pokedex, total_pokemons);
    
    // sort index
   qsort(indexes, total_pokemons, sizeof(INDEX_T*), sort_index);

    // Write the indexes to the indexes bin file
    write_indexes(indexes, total_pokemons);


    // For loop that iterates for the number of pokemon
    for (size_t i = 0; i < total_pokemons; i++) {
        
        // Free name of index struct 
        free((*(indexes + i))->name);

        // Free elements in the index struct
        free(*(indexes + i));
    }

    // Free the indexes array
    free(indexes);

    // Set indexes to NULL (proper practice)
    indexes = NULL;

    for (size_t i = 0; i < total_pokemons; i++) {
        
        // Free name of index struct 
        free((*(Pokedex + i))->name);

        // Free elements in the index struct
        free(*(Pokedex + i));
    }

    // Free the Pokedex array
     free(Pokedex);

    // Set Pokedex to NULL (proper practice)
    Pokedex = NULL;

    // For loop that iterates for the number of abilities
    for (size_t i = 0; i < total_abilities; i++) {
        
        // Check if the description is not NULL
        if ((*(Abilities + i))->desc != NULL) {
            
            // Free the description of the ability
            free((*(Abilities + i))->desc);
        }
        
        // Check if the name is not NULL
        if ((*(Abilities + i))->name != NULL) {
            
            // Free the name of the ability
            free((*(Abilities + i))->name);
        }
        
        // Free elements in the ability struct
        free(*(Abilities + i));
    }

    // Free the Pokedex array
     free(Abilities);

    // Set Pokedex to NULL (proper practice)
    Abilities = NULL;

    // TODO cleanup the rest of resources used in program
    free(line);
    fclose(abilities_file);
    fclose(pokemon_file);
    return 0;
}

// this function has to allocate an ABILITY_T* (an ability pointer) for the ability
// then call strsep to parse out the name and description
// from the buffer text, and return
ABILITY_T* parse_ability(char * buf) { // Parsing a line from abilities.csv and returning an ABILITY_T*. Parsing means to analyze a string and break it down into its components.
   
    ABILITY_T* ability = malloc(sizeof(ABILITY_T)); // allocate memory for the ABILITY_T. Allocate memory for Abilitiies
    
    if (!ability) // if memory allocation failed
    {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return NULL;
    }
    
    char *name = NULL; // name is a pointer to the name of the ability

    char *desc = NULL; // desc is a pointer to the description of the ability

    name = strsep(&buf, ",\n\0"); // parse the name of the ability

    if (strstr(name,"-") != NULL) // if the name contains a hyphen
    {
        char *temp = malloc(strlen(name) + 1); // allocate memory for the temp variable

        char *sep = strsep(&name, "-"); // parse the first part of the name

        memset(sep, (sep[0] - 32), 1); // convert the first letter to uppercase

        strcpy(temp, sep); // copy the first part of the name to the temp variable

        while (name != NULL) // while the name is not null
        {
            strcat(temp," "); // concatenate a space to the temp variable
            
            sep = strsep(&name, "-"); // parse the next part of the name

            if (sep[0] >= 97 && sep[0] <= 122) // if the first letter is lowercase
            {
                memset(sep, (sep[0] - 32), 1); // convert the first letter to uppercase
            }

            strcat(temp, sep); // concatenate the next part of the name to the temp variable
        }

        ability->name = strdup(temp); // Duplicate the temp variable and store it in the name of the ability

        free(temp); // free the temp variable
    }

    else // if the name does not contain a hyphen
    {
        if (name[0] >= 97 && name[0] <= 122) // if the first letter is lowercase
        {
            memset(name, (name[0] - 32), 1); // convert the first letter to uppercase
        }

        ability->name = strdup(name); // Duplicate the name and store it in the name of the ability
    }

    desc = strsep(&buf, "\n\0"); // parse the description of the ability

    size_t len = strlen(desc); // store the length of the description

    desc[len - 1] = '\0'; // remove the newline character from the end of the description

    ability->desc = strdup(desc); // Duplicate the description and store it in the desc of the ability
 

    return ability; // return the ABILITY_T*
} 


POKEMON_T* parse_pokemon(char * buf , ABILITY_T** abilities, size_t total_abilities) { // Parsing a line from pokemon.csv and returning a POKEMON_T*
    // Pokemon struct. A pokemon struct is a structure that contains information about a pokemon.
    // The information includes the pokemon's name,id,type,subtype,ablty_one,ablty_two,ablty_three,base_exp,weight,height,hp,attack,defense,spec_attack,spec_defense,speed,offset
    // Read the pokemon.csv file and parse the data into a POKEMON_T*.
    

    POKEMON_T* pokemon = malloc(sizeof(POKEMON_T)); // allocate memory for the POKEMON_T
    
    if (!pokemon) {
    fprintf(stderr, "Memory allocation for POKEMON_T failed\n");
    return NULL;
}
// Initialize all pointer members to NULL
//pokemon->name = NULL;

  
    // Initializing token variable for parsing
    char *token;  



    // Parse and filling each field of the POKEMON_T
   
   // Name Parsing
    token = strsep(&buf, ",\n"); 
    
    
    if (token[0] >= 97 && token[0] <= 122) // if the first letter is lowercase 
    {
        memset(token, (token[0] - 32), 1); // convert the first letter to uppercase
    }

    pokemon->name = strdup(token);

    // ID Parsing

    token = strsep(&buf, ",\n"); // parse the id


    if (strcmp(token, "") == 0) {

        if (pokemon->name != NULL) {
            free(pokemon->name);
        }
       // fprintf(stderr, "Error: Missing ID\n");
        free (pokemon);
        return NULL;
    }

    pokemon->id = atoi(token); // store the id (atoi converts a string to an integer)

    

    // Type & Subtype Parsing

    for (int i=0; i <= 1 ; i++)
    {
        token = strsep(&buf, ",\n"); // parse the type or subtype

        if (strcmp(token, "") == 0) {

            pokemon->subtype = NONE;
            
            break;
        }

        if (token[0] >= 97 && token[0] <= 122) // if the first letter is lowercase
        {
            memset(token, (token[0] - 32), 1); // convert the first letter to uppercase
        }

        if (i == 0)
        {
            pokemon->type = get_type(token); // store the type
        }
        if (i == 1)
        {
            pokemon->subtype = get_type(token); // store the subtype
        }  
    }
    
    

    // Ability Parsing

    pokemon->ability_one = NULL; // set the first ability to NULL
    pokemon->ability_two = NULL; // set the second ability to NULL
    pokemon->ability_three = NULL; // set the third ability to NULL

    for (int i = 0; i < 3; i++) { // loop through abilities

        token = strsep(&buf, ","); // parse the ability

        ABILITY_T *search_ability = NULL; // set the search ability to NULL

        char *realtoken = strdup(token); // duplicate the token and store it in realtoken

        char *realtoken_ptr = realtoken; // store the realtoken in realtoken_ptr to free it later

        char *temp = malloc(strlen(realtoken_ptr) + 1); // allocate memory for the temp variable
   
        *temp = '\0'; // set the first character of temp to null

        char *sep = strsep(&realtoken_ptr, "-"); // parse the first part of the token

        while (sep != NULL) // while sep is not null
        {   
            if (*sep)
            {

            if (sep[0] >= 97 && sep[0] <= 122) // if the first letter is lowercase
            {
                memset(sep, (sep[0] - 32), 1); // convert the first letter to uppercase
            }

            if (*temp){
                strcat(temp, " "); // concatenate a space to the temp variable
            }

            strcat(temp, sep); // concatenate the next part of the token to the temp variable
            }
            sep = strsep(&realtoken_ptr, "-"); // parse the next part of the token
            
        }

    
        //ABILITY_T* key = (ABILITY_T){ .name = temp }; // Temporary key with the name
        ABILITY_T** result = lfind(temp, abilities, &total_abilities, sizeof(ABILITY_T*), search_comp);

        if (result) {
            search_ability = *result; // Dereference the result of lfind to get ABILITY_T*
        }           
        
        
        free(temp); // free the temp variable

        free(realtoken); // free the realtoken variable

        if (i == 0)
        {
            pokemon->ability_one = search_ability; // store the first ability in the POKEMON_T

            pokemon->ability_two = NULL; // set the second ability to NULL

            pokemon->ability_three = NULL; // set the third ability to NULL
        }
        if (i == 1)
        {
            pokemon->ability_two = search_ability; // store the second ability in the POKEMON_T

        }

        if (i == 2)
        {
            pokemon->ability_three = search_ability; // store the third ability in the POKEMON_T

        }
    }
    // Base Experience Parsing

    token = strsep(&buf, ","); // parse the base experience

    

    /*
    if (token == NULL || *token == '\0') { // if the token is empty
        fprintf(stderr, "Error: Missing base experience\n");
        free (pokemon);
        return NULL;
    }
*/
    pokemon->base_experience = atoi(token); // store the base experience (atoi converts a string to an integer)
    
    // Weight Parsing

    token = strsep(&buf, ",\n"); // parse the weight
    
    if (token == NULL) {
        fprintf(stderr, "Error: Missing weight\n");
        return NULL;
    }
    
    pokemon->weight = atof(token) / 10; // store the weight (atof converts a string to a float)
    
    /*
    if (!pokemon->weight) {
        fprintf(stderr, "Error: Invalid weight\n");
        return NULL;
    }
*/
    // Height Parsing

    token = strsep(&buf, ","); // parse the height
    
    if (token == NULL) {
        fprintf(stderr, "Error: Missing height\n");
        return NULL;
    }

    pokemon->height = atof(token) / 10; // store the height (atof converts a string to a float)
    
    /*
    if (!pokemon->height) {
        fprintf(stderr, "Error: Invalid height\n");
        return NULL;
    }
    */
    // HP Parsing

    token = strsep(&buf, ",\n"); // parse the hp

    if (token == NULL) {
        fprintf(stderr, "Error: Missing hp\n");
        return NULL;
    }

    pokemon->hp = atoi(token); // store the hp (atoi converts a string to an integer)
    
    /*
    if (!pokemon->hp) {
        fprintf(stderr, "Error: Invalid hp\n");
        return NULL;
    }
    */
    // Attack Parsing
    
    token = strsep(&buf, ",\n"); // parse the attack
    
    if (token == NULL) {
        fprintf(stderr, "Error: Missing attack\n");
        return NULL;
    }

    pokemon->attack = atoi(token); // store the attack (atoi converts a string to an integer)
    
    /*
    if (!pokemon->attack) {
        fprintf(stderr, "Error: Invalid attack\n");
        return NULL;
    }
    */

    // Defense Parsing

    token = strsep(&buf, ",\n"); // parse the defense
    
    if (token == NULL) {
        fprintf(stderr, "Error: Missing defense\n");
        return NULL;
    }

    pokemon->defense = atoi(token); // store the defense (atoi converts a string to an integer)
   
    /*
    if (!pokemon->defense) {
        fprintf(stderr, "Error: Invalid defense\n");
        return NULL;
    }
    */
    // Special Attack Parsing

    token = strsep(&buf, ",\n"); // parse the special attack
    if (token == NULL) {
        fprintf(stderr, "Error: Missing special attack\n");
        return NULL;
    }

    pokemon->special_attack = atoi(token); // store the special attack (atoi converts a string to an integer)
    
    /*
    if (!pokemon->special_attack) {
        fprintf(stderr, "Error: Invalid special attack\n");
        return NULL;
    }
    */

    // Special Defense Parsing
    
    token = strsep(&buf, ",\n"); // parse the special defense
    if (token == NULL) {
        fprintf(stderr, "Error: Missing special defense\n");
        return NULL;
    }

    pokemon->special_defense = atoi(token); // store the special defense (atoi converts a string to an integer)
    /*
    if (!pokemon->special_defense) {
        fprintf(stderr, "Error: Invalid special defense\n");
        return NULL;
    }
    */
    // Speed Parsing

    token = strsep(&buf, ",\n"); // parse the speed
    
    if (token == NULL) {
        fprintf(stderr, "Error: Missing speed\n");
        return NULL;
    }

    pokemon->speed = atoi(token); // store the speed (atoi converts a string to an integer)
   /*
    if (!pokemon->speed) {
        fprintf(stderr, "Error: Invalid speed\n");
        return NULL;
    }
    */
    // Offset Parsing

    token = strsep(&buf, ",\n"); // parse the offset
   
    if (token == NULL) {
        fprintf(stderr, "Error: Missing offset\n");
        return NULL;
    }
    
    pokemon->offset = atoi(token); // store the offset (atoi converts a string to an integer)
    
    /*
    if (!pokemon->offset) {
        fprintf(stderr, "Error: Invalid offset\n");
        return NULL;
    }
    */

    return pokemon; // return the POKEMON_T*
}

/**********************WRITE FUNCTIONS***************************/

void write_abilities(ABILITY_T** Abilities, size_t total_abilities)
{
   // Open the abilities file for writing
    FILE * abilitiesFile = fopen("abilities.bin", "wb");

    // Check if file was successfully opened
    if (!abilitiesFile)
    {
        // Print an error message
        fprintf(stderr, "Could not open file abilities.bin\n");

        // Return an error code
        return;
    }
   
    for (size_t i = 0; i < total_abilities; i++)
    {
        // Access each ability in the array
        ABILITY_T* ability = Abilities[i];

        // Get current offset before writing
        ability->offset = ftell(abilitiesFile); 



        // Write the length of the ability name (8 Byte name length)
        size_t name_len = strlen(ability->name); 

        // Write the length of the name to the file (Should be 8 bytes)
        fwrite(&name_len, sizeof(size_t), 1, abilitiesFile);

        // Write the name to the file
        fwrite(ability->name, sizeof(char), name_len, abilitiesFile);



        // Write the length of the description (8 Byte description length)
        size_t desc_len = strlen(ability->desc); // +1 for null terminator

        // Write the length of the description to the file
        fwrite(&desc_len, sizeof(size_t), 1, abilitiesFile);

        // Write the description to the file
        fwrite(ability->desc, sizeof(char), desc_len, abilitiesFile);
    
       
    }


    fclose(abilitiesFile);
}

INDEX_T** write_pokemons(POKEMON_T** Pokedex, size_t total_pokemons)
{   
    // Declare the index array
    INDEX_T** indexes = malloc(sizeof(INDEX_T*) * total_pokemons);

    // Open the pokemon file for writing
   FILE * pokemonFile = fopen("pokemon.bin", "wb");
    
    // Check if file was successfully opened
    if (pokemonFile == NULL) {
        
        // Print an error message
        fprintf(stderr, "Error opening pokemon.bin for writing.\n");
        
        // Exit the program
        exit(EXIT_FAILURE);
    }

    // Initialize the noAbility variable
    int64_t noAbility = __INT64_MAX__;

    // Iterate through all pokemons
    for (size_t i = 0; i < total_pokemons; ++i)
    {
        // Allocate memory for a new index
        INDEX_T* index = malloc(sizeof(INDEX_T));
        // Check if memory allocation was successful
        if (index == NULL) {
            fprintf(stderr, "Failed to allocate memory for index.\n");

            break;
        }

        // Store the current offset of the pokemon file in the index
        index->offset = ftell(pokemonFile);
    
        // Duplicate the pokemon's name and store it in the index
        index->name = strdup(Pokedex[i]->name);
        // Check if duplication was successful
        if (index->name == NULL) {
            fprintf(stderr, "Failed to duplicate pokemon name.\n");
            // Free the previously allocated memory for index
            free(index);

            // Break the loop
            break;
        }



        // Assign the reallocated memory back to indexes
        indexes[i] = index;
        
        
    

        // Write the id to the file
        fwrite(&Pokedex[i]->id, sizeof(int), 1, pokemonFile);

        // Write the type to the file
        fwrite(&Pokedex[i]->type, sizeof(int), 1, pokemonFile);

        // Write the subtype to the file
        fwrite(&Pokedex[i]->subtype, sizeof(int), 1, pokemonFile);


              
         // Write offset for ability one if it exists, else write no_abil
        if (Pokedex[i]->ability_one != NULL) {
            fwrite(&Pokedex[i]->ability_one->offset, sizeof(off_t), 1, pokemonFile);
        } 
        else {
            fwrite(&noAbility, sizeof(off_t), 1, pokemonFile);
        }
        // Write offset for ability two if it exists, else write no_abil
        if(Pokedex[i]->ability_two != NULL)
        {
            fwrite(&Pokedex[i]->ability_two->offset, sizeof(off_t), 1, pokemonFile);
        }
        else
        {
            fwrite(&noAbility, sizeof(off_t), 1, pokemonFile);
        }

        // Write offset for ability three if it exists, else write no_abil
        if(Pokedex[i]->ability_three != NULL)
        {
            fwrite(&Pokedex[i]->ability_three->offset, sizeof(off_t), 1, pokemonFile);
        }
        else
        {
            fwrite(&noAbility, sizeof(off_t), 1, pokemonFile);
        }
 
        // Write the base experience to the file
        fwrite(&Pokedex[i]->base_experience, sizeof(int), 1, pokemonFile);

        // Write the weight to the file
        fwrite(&Pokedex[i]->weight, sizeof(float), 1, pokemonFile);

        // Write the height to the file
        fwrite(&Pokedex[i]->height, sizeof(float), 1, pokemonFile);

        // Write the hp to the file
        fwrite(&Pokedex[i]->hp, sizeof(int), 1, pokemonFile);

        // Write the attack to the file
        fwrite(&Pokedex[i]->attack, sizeof(int), 1, pokemonFile);

        // Write the defense to the file
        fwrite(&Pokedex[i]->defense, sizeof(int), 1, pokemonFile);

        // Write the special attack to the file
        fwrite(&Pokedex[i]->special_attack, sizeof(int), 1, pokemonFile);

        // Write the special defense to the file
        fwrite(&Pokedex[i]->special_defense, sizeof(int), 1, pokemonFile);

        // Write the speed to the file
        fwrite(&Pokedex[i]->speed, sizeof(int), 1, pokemonFile);

        // Write the ascii offset to the file
        fwrite(&Pokedex[i]->offset, sizeof(off_t), 1, pokemonFile);

    }

    // Close the pokemon file
    fclose(pokemonFile);


    return indexes;
}

void write_indexes(INDEX_T** Index, size_t total_pokemons)
{
    // Open the indexes file for writing
    FILE * indexesFile = fopen("indexes.bin", "wb");

    // Check if file was successfully opened
    if (indexesFile == NULL) {
        
        // Print an error message
        fprintf(stderr, "Error opening indexes.bin for writing.\n");
        
        // Exit the program
        exit(EXIT_FAILURE);
    }

    // Write the total number of pokemons to the file
    fwrite(&total_pokemons, sizeof(size_t), 1, indexesFile);

    // Write the offset of each pokemon to the file
    for (size_t i = 0; i < total_pokemons; i++)
    {



        // Write the name length of the pokemon to the file
        size_t name_len = strlen(Index[i]->name);

        fwrite(&name_len, sizeof(size_t), 1, indexesFile);

        // Write the name of the pokemon to the file
        fwrite(Index[i]->name, name_len, 1, indexesFile);

        // Write the offset of the pokemon to the file
        fwrite(&Index[i]->offset, sizeof(off_t), 1, indexesFile);
    
    }

    //free(Index);
    fclose(indexesFile);
}
