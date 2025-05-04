#include <cs50.h>
#include <stdio.h>
#include <string.h>
// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
} pair;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count = 0;
int candidate_count = 0;
int source = 0;
int lastsource = 0;
// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
bool cyclic(int win, int loss);
void sort_pairs(void);
void lock_pairs(void);
void print_winner(void);
void resetpreferences(void);
bool is_winner(int j);
void swap(int first, int second);
int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    int voter_count = get_int("Number of voters: ");
    resetpreferences();
    // Query for votes
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's ith preference
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        record_preferences(ranks);

        printf("\n");
    }
    add_pairs();
    sort_pairs();
    lock_pairs();
    print_winner();
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    for (int i = 0; i < candidate_count; i++)
    {
        if (strcmp(name, candidates[i]) == 0)
        {
            ranks[rank] = i;
            return true;
        }
    }

    return false;
}
void resetpreferences(void)
{
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
            preferences[i][j] = 0;
    }
}
// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{

    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = i + 1; j < candidate_count; j++)
            preferences[ranks[i]][ranks[j]]++;
    }
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{

    for (int i = 0; i < candidate_count; i++)
    {
        for (int k = i + 1; k < candidate_count; k++)
        {
            if (preferences[i][k] > preferences[k][i])
            {
                pairs[pair_count].winner = i;
                pairs[pair_count].loser = k;
                pair_count++;
            }
            else if (preferences[i][k] < preferences[k][i])
            {
                pairs[pair_count].winner = k;
                pairs[pair_count].loser = i;
                pair_count++;
            }
        }
    }
    return;
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    int difference1 = 0, difference2 = 0;
    bool swaped = false;
    do
    {
        swaped = false;
        for (int i = 0; i < (pair_count - 1); i++)
        {
            difference1 = preferences[pairs[i].winner][pairs[i].loser];
            difference2 = preferences[pairs[i + 1].winner][pairs[i + 1].loser];
            if (difference1 < difference2)
            {
                swap(i, (i + 1));
                swaped = true;
            }
        }
    }
    while (swaped == true);
    return;
}
void swap(int first, int second)
{
    int buffer1 = pairs[second].winner;
    int buffer2 = pairs[second].loser;
    pairs[second].winner = pairs[first].winner;
    pairs[second].loser = pairs[first].loser;
    pairs[first].winner = buffer1;
    pairs[first].loser = buffer2;
}
bool cyclic(int win, int loss)
{

    if (locked[loss][win] == true)
        return true;
    else
    {
        for (int i = 0; i < candidate_count; i++)
        {
            if (locked[loss][i] == true && cyclic(win, i))
                return true;
        }
    }
    return false;
}

// Lock pairs int)o the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    for (int i = 0; i < pair_count; i++)
    {
        int win = pairs[i].winner;
        int loss = pairs[i].loser;
        if (!cyclic(win, loss))
            locked[pairs[i].winner][pairs[i].loser] = true;
    }

    return;
}

bool is_winner(int j)
{
    for (int i = 0; i < candidate_count; i++)
    {
        if (locked[i][j] == true)
        {
            return false;
        }
    }

    return true;
}
// Print the winner of the election
void print_winner(void)
{
    for (int i = 0; i < candidate_count; i++)

    {
        if (is_winner(i))
        {
            printf("%s\n", candidates[i]);
            return;
        }
    }
    return;
}
