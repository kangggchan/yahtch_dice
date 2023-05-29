#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

#define NUM_DICE 5

volatile sig_atomic_t termination_flag = 0;

// Function to handle SIGINT (Ctrl+C) signal
void sigint_handler(int sig)
{
    printf("Game terminated by user.\n");
    exit(EXIT_SUCCESS);
}

// Function to handle SIGTSTP (Ctrl+Z) signal
void sigtstp_handler(int sig)
{
    printf("Game paused. Press Ctrl+C to exit or Ctrl+Z to resume.\n");
    signal(SIGTSTP, SIG_DFL);
}

void display_menu(void)
{
    printf("Welcome to Yahtzee!\n");
    printf("1. Print Game Rules\n");
    printf("2. Start a Game of Yahtzee\n");
    printf("3. Exit\n");
}

int get_option(void) // prompts for option once
{
    int option = 0;
    printf("Please enter the number corresponding with the desired action: ");
    scanf("%d", &option);
    return option;
}

int evaluate_option(int option) // makes sure option is between 1 and 3, inclusively
{
    if (option < 1 || option > 3)
    {
        printf("That's not a valid option!\n");
        return 0; // not satisfied
    }
    else
    {
        return 1; // satisfied
    }
}

int menu_select(void) // continues to display menu until option requirements are satisfied
{
    int option = 0;
    do {
        display_menu();
        option = get_option();
        system("cls");
    } while (!evaluate_option(option));
    return option;
}

void diceOne ()
{
    printf ("+-------+\n");
    printf ("|       |\n");
    printf ("|   o   |\n");
    printf ("|       |\n");
    printf ("+-------+\n");
}

void diceTwo ()
{
    printf ("+-------+\n");
    printf ("|     o |\n");
    printf ("|       |\n");
    printf ("| o     |\n");
    printf ("+-------+\n");
}

void diceThree ()
{
    printf ("+-------+\n");
    printf ("|     o |\n");
    printf ("|   o   |\n");
    printf ("| o     |\n");
    printf ("+-------+\n");
}

void diceFour ()
{
    printf ("+-------+\n");
    printf ("| o   o |\n");
    printf ("|       |\n");
    printf ("| o   o |\n");
    printf ("+-------+\n");
}

void diceFive ()
{
    printf ("+-------+\n");
    printf ("| o   o |\n");
    printf ("|   o   |\n");
    printf ("| o   o |\n");
    printf ("+-------+\n");
}

void diceSix ()
{
    printf ("+-------+\n");
    printf ("| o   o |\n");
    printf ("| o   o |\n");
    printf ("| o   o |\n");
    printf ("+-------+\n");
}


void print_dice(int dice[NUM_DICE]) {
    printf("Dice: \n");
    for (int i = 0; i <= NUM_DICE; i++) {
        switch (dice[i]){
            case 1:
                diceOne();
                break;
            case 2:
                diceTwo();
                break;
            case 3:
                diceThree();
                break;
            case 4:
                diceFour();
                break;
            case 5:
                diceFive();
                break;
            case 6:
                diceSix();
                break;

            default:
                break;
        }
    }
    printf("\n");
}

void save_results(int scores[], int num_players) {
    int fd = open("results.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("Error opening file");
        return;
    }

    char buffer[256];
    for (int i = 0; i < num_players; i++) {
        int bytes_written = snprintf(buffer, sizeof(buffer), "Player %d: %d\n", i + 1, scores[i]);
        write(fd, buffer, bytes_written);
    }

    close(fd);
}

int will_be_a_yahtzee(int frequency[]){
    for (int i = 1; i < 7; i++)
        if (frequency[i] == 5)
            return 1;
    return 0;
}

int evaluate_score_card_option(int option, int score_card[], int frequency[]) // returns 1 if option's slot can be used, and 0 if not
{
    if (option < 1 || option > 13) // return 0 if that option isn't one of the 13
    {
        printf("That is not a valid option.\n");
        return 0;
    }
    else if (option == 12) // if Yahtzee is selected
    {
        if (score_card[option] < 0)
        {
            return 1; // Yahtzee can be used if the value is -1 (hasn't been used  or scratched yet)
        }
        else if (score_card[option] == 0) // if the value is 0, it can't be used or scratched because it already has been scratched
        {
            printf("You already used that slot!\n");
            return 0;
        }
        else if (will_be_a_yahtzee(frequency)) // if the value for yahtzee is greater than 0 (so there was a previous successful Yahtzee) then you aren't allowed to scratch, but you can add bonus Yahtzees
        {
            return 1;
        }
        else
        {
            printf("You can't scratch your bonus Yahtzee slots.\n");
            return 0;
        }
    }
    else if (score_card[option] >= 0) // for every other slot, if the value is less than 0, it can be used or scratched, but otherwise it can't be used
    {
        printf("You have already used that slot!\n");
        return 0;
    }
    else
    {
        return 1;
    }
}

void sum_ones(int score_card[], int frequency[]){
    score_card[1] = frequency[1] * 1;
}

void sum_twos(int score_card[], int frequency[]){
    score_card[2] = frequency[2] * 2;
}

void sum_threes(int score_card[], int frequency[]){
    score_card[3] = frequency[3] * 3;
}

void sum_fours(int score_card[], int frequency[]){
    score_card[4] = frequency[4] * 4;
}

void sum_fives(int score_card[], int frequency[]){
    score_card[5] = frequency[5] * 5;
}

void sum_sixes(int score_card[], int frequency[]){
    score_card[6] = frequency[6] * 6;
}

void three_of_a_kind(int score_card[], int frequency[]){
    score_card[7] = 0;
    int three = 0, sum = 0;
    for (int i = 1; i < 7; i++){
        sum += frequency[i] * i;
        if (frequency[i] >= 3)
        {
            three = 1;
        }
    }
    if (three)
        score_card[7] = sum;
}

void four_of_a_kind(int score_card[], int frequency[]){
    score_card[8] = 0;
    int four = 0, sum = 0;
    for (int i = 1; i < 7; i++){
        sum += frequency[i] * i;
        if (frequency[i] >= 4)
        {
            four = 1;
        }
    }
    if (four)
        score_card[8] = sum;
}

void full_house(int score_card[], int frequency[]){
    score_card[9] = 0;
    int two = 0, three = 0;
    for (int i = 1; i < 7; i++)
    {
        if (frequency[i] == 2)
            two = 1;

        if (frequency[i] == 3)
            three = 1;
    }

    if (two && three)
        score_card[9] = 25;
}

void small_straight(int score_card[], int frequency[]){
    score_card[10] = 0;
    int sequence = 0;
    for (int i = 1; i < 7; i++)
    {
        if (frequency[i] > 0){
            sequence += 1;
            if (sequence == 4)
                score_card[10] = 30; // updating score card if so
        }
        else{
            sequence = 0; // reseting sequence to 0 if there is none of that die face
        }
    }
}

void large_straight(int score_card[], int frequency[]) // if all 5 dice are sequential, then there's a large straight
{
    score_card[11] = 0;
    int sequence = 0;
    for (int i = 1; i < 7; i++)
    {
        if (frequency[i] > 0){
            sequence += 1;
            if (sequence == 5)
                score_card[11] = 40;

        }
        else
            sequence = 0;
    }
}

void yahtzee(int score_card[], int frequency[]){
    int yahtzee_found = 0;
    for (int i = 1; i < 7; i++){
        if (frequency[i] == 5){
            yahtzee_found = 1;
            if (score_card[12] == -1)
                score_card[12] = 50; // if first yahtzee, its worth 50

            else
                score_card[12] += 100; // all subsequent yahtzees are worth 100
        }
    }
    if (!yahtzee_found)
        score_card[12] = 0; // scratched yahtzee
}

void chance(int score_card[], int frequency[]){
    score_card[13] = 0;
    int sum = 0;
    for (int i = 1; i < 7; i++)
        sum += frequency[i] * i; // multiplying frequency by index  and summing them all
    score_card[13] = sum;
}


void display_score_card(int score_card[]) // displays the score card. the box titles are kept in an array of strings that parallels the score card array, so that they both can be accessed in a single i for loop
{
    char slot_names[14][18] = { "0th slot", " Sum of 1's      ", " Sum of 2's      ", " Sum of 3's      ",  " Sum of 4's      ",  " Sum of 5's      ",  " Sum of 6's      ", " Three of a kind ", " Four of a kind  ", " Full House      ", "Small straight  ", "Large straight  ", "Yahtzee         ", "Chance          " };

    printf("\nYour Score Card:\n");
    printf(" _________________________\n");
    for (int i = 1; i < 14; i++)
    {
        printf("| %d: %s:", i, slot_names[i]);
        if (score_card[i] < 10)
        {
            printf(" "); // adding a space for smaller digits so that the score card looks nice
        }
        if (score_card[i] < 100)
        {
            printf(" ");
        }
        if (score_card[i] >= 0) // only displays the score if it is 0 or greater, otherwise it will show a -1 for unused slots instead of being empty
        {
            printf("%d", score_card[i]);
        }
        else
        {
            printf(" ");
        }
        printf("|\n");
    }
    printf("|_________________________|\n\n");
}


void roll_and_print_dice(int actual_dice[], int reroll[])
{
    char character = '\0';
    printf("Press any non-whitespace character to roll: ");
    scanf(" %c", &character); // letting character interact and roll dice

    printf("Dice values:\n");
    for (int i = 1; i < 6; i++){
        if (reroll[i])
        {
            actual_dice[i] = rand() % 6 + 1;
        }
    }
    print_dice(actual_dice);
}

void count_dice(int actual_dice[], int frequency[]){
    for (int i = 1; i < 6; i++)
        frequency[actual_dice[i]] += 1;
}

int roll_again_question(int rolls, int reroll[])
{
    int roll_again = 0;
    char yes_no_dice = '\0';
    if (rolls < 2) // if more rolls are available
    {
        do {
            printf("Would you like to roll again? input 'y' for yes or 'n' for no: "); // asks user if they want to roll again. continuously prompts until yes or no
            scanf(" %c", &yes_no_dice);
        } while (yes_no_dice != 'y' && yes_no_dice != 'n');

        if (yes_no_dice == 'y')
        {
            for (int i = 1; i < 6; i++)
            {
                char yes_no_die = '\0';
                do {
                    printf("Would you like to reroll die %d? input 'y' for yes or 'n' for no: ", i); // asks which dice specifically should be rerolled
                    scanf(" %c", &yes_no_die);
                } while (yes_no_die != 'y' && yes_no_die != 'n');

                if (yes_no_die == 'n')
                {
                    reroll[i] = 0; // changes reroll values to 0 for certain dice if they should be kept
                }
            }

            roll_again = 1;
        }
    }

    return roll_again; // returns 1 if the user wants another roll, 0 otherwise
}


void calculate_score_for_option(int score_card[], int frequency[], int option) // switch statement to channel to proper function to calculate score and update the score card
{
    switch (option)
    {
        case 1:
            sum_ones(score_card, frequency);
            break;
        case 2:
            sum_twos(score_card, frequency);
            break;
        case 3:
            sum_threes(score_card, frequency);
            break;
        case 4:
            sum_fours(score_card, frequency);
            break;
        case 5:
            sum_fives(score_card, frequency);
            break;
        case 6:
            sum_sixes(score_card, frequency);
            break;
        case 7:
            three_of_a_kind(score_card, frequency);
            break;
        case 8:
            four_of_a_kind(score_card, frequency);
            break;
        case 9:
            full_house(score_card, frequency);
            break;
        case 10:
            small_straight(score_card, frequency);
            break;
        case 11:
            large_straight(score_card, frequency);
            break;
        case 12:
            yahtzee(score_card, frequency);
            break;
        case 13:
            chance(score_card, frequency);
            break;
    }
}


int score_counting(int score_card[]) // adding up score on card
{
    int upper_sum = 0, lower_sum = 0, bonus = 0;

    for (int i = 1; i < 7; i++) // tallying upper sum
    {
        if (score_card[i] >= 0)
        {
            upper_sum += score_card[i];
        }
    }

    if (upper_sum >= 63)
        bonus = 35; // if upper sum is 63 or greater, adding 35

    for (int i = 7; i < 14; i++) // tallying lower sum
    {
        if (score_card[i] >= 0)
        {
            lower_sum += score_card[i];
        }
    }

    return upper_sum + bonus + lower_sum; // adding all together and returning
}


void take_turn(int score_card[])
{
    int roll_again = 1, option = 0;
    int actual_dice[6] = { 0, 0, 0, 0, 0, 0 };
    int reroll[6] = { 1, 1, 1, 1, 1, 1 }; // initialized to 1, because this represents a reroll, and all the dice need to be rolled on the first roll
    int frequency[7] = { 0, 0, 0, 0, 0, 0, 0 }; // set to 0 because these will be added to based on final 5 dice

    for (int rolls = 0; roll_again == 1; rolls++)
    {
        display_score_card(score_card); // showing user score card at beginning of round and after every roll
        roll_and_print_dice(actual_dice, reroll); // updating dice based on rerolls
        roll_again = roll_again_question(rolls, reroll); // offering reroll if less than 3 rolls
        system("cls");
    }

    count_dice(actual_dice, frequency);

    display_score_card(score_card); // displaying score card for final decision
    print_dice(actual_dice);

    do {
        option = get_option();
    } while (!evaluate_score_card_option(option, score_card, frequency)); // requesting a score card option until input is valid

    calculate_score_for_option(score_card, frequency, option); // calculating score card update
    system("cls");
    display_score_card(score_card); // showing the resulting scorecard
    char character = '\0';
    printf("Press any non-whitespace character to end your turn: ");
    scanf(" %c", &character); // letting player end their turn
}

void play_game(int num_players) {
    int round = 1, player_1_score = 0, player_2_score = 0;
    int scores[2] = {0, 0};
    int player_1_scorecard[14] = { -1, -1, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, -1 }; // both players scores initialized to -1 because this score is impossible, where 0 represents a scratch
    int player_2_scorecard[14] = { -1, -1, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, -1 };

    srand(time(NULL));

    pid_t pid;
    int player_turn = 0;

    while (!termination_flag && round < 5) {
        pid = fork();

        if (pid == 0) {
            system("cls");
            printf("Round %d:\n", round);
            printf("\nIt's your turn, Player %d!\n", player_turn + 1);
            if (player_turn == 0) take_turn(player_1_scorecard);
            else take_turn(player_2_scorecard);


        } else if (pid > 0) {
            int status;
            waitpid(pid, &status, 0);

            if (WIFEXITED(status)) {
                int exit_status = WEXITSTATUS(status);
                scores[player_turn] += exit_status;
            }
            player_turn = (player_turn + 1) % num_players;

        } else {
            perror("Error forking process");
            exit(1);
        }
        round++;
    }

    if (pid > 0) {
        kill(pid, SIGKILL);
    }

    system("cls"); // clearing screen to clean up output
    printf("Game finished.\n");
    printf("Final results is in the result.txt file.\n");
    player_1_score = score_counting(player_1_scorecard); // displaying end results
    player_2_score = score_counting(player_2_scorecard);

    save_results(scores, num_players);
}

void display_rules(void) // shows rules
{
    printf("\nYahtzee is a dice game played with two players. The objective of the game\n"
           "is to score the highest number of points by rolling dice and selecting categories to score\n"
           "based on the outcomes of the rolls.\n"
           "At the beginning of the game, each player starts with a score of 0.\n"
           "Players take turns rolling five dice.\n"
           "After each roll, the player can choose to keep any number of dice and reroll the rest."
           "The player can roll the dice up to three times on their turn."
           "Once the player is satisfied with their roll or has reached three rolls,\n"
           "they must choose a category to score based on the dice values."
           "The available categories include Ones, Twos, Threes, Fours, Fives, Sixes, Three of a Kind, \n"
           "Four of a Kind, Full House, Small Straight, Large Straight, Yahtzee, and Chance.\n\n"
           "The score for each category is calculated based on specific rules: as follows:\n\n"
           "Name            | Combination                    | Score\n"
           "------------------------------------------------------------------------------------\n"
           "Three of a kind | Three dice with the same face  | Sum of all face values on 5 dice\n"
           "Four of a kind  | Four dice with the same face   | Sum of all face values on 5 dice\n"
           "Full house      | One pair and a three of a kind | 25\n"
           "Small straight  | A sequence of four dice        | 30\n"
           "Large straight  | A sequence of five dice        | 40\n"
           "Yahtzee         | Five dice with the same face   | 50 for first, 100 for extras\n"
           "Chance          | Catch-all combination          | Sum of all face values on 5 dice\n\n");

}


int main() {
    srand(time(NULL));  // Seed the random number generator with current time

    signal(SIGINT, sigint_handler);    // Register SIGINT handler
    signal(SIGTSTP, sigtstp_handler);  // Register SIGTSTP handler

    printf("Welcome to Yahtzee!\n");
    int option = 0, program_running = 1;

    while (program_running)
    {
        option = menu_select();

        switch (option)
        {
            case 1:
                display_rules();
                break;
            case 2:
                play_game(2);
                break;
            case 3:
                program_running = 0;
                printf("Thanks for playing!\n");
                break;
        }
    }
    return 0;
}
