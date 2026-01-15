#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// ------------------ Player Structure ------------------
typedef struct Player {
    int id;
    char name[30];
    int age;
    int score;
    int wins;
    int losses;
    struct Player *next;
} Player;

// ------------------ Queue Structure ------------------
typedef struct Queue {
    Player *head;
    Player *tail;
} Queue;

// ------------------ Global Lists ------------------
Player *LG = NULL; // Winners list
Player *LP = NULL; // Losers list

// ------------------ Queue Operations ------------------
void enqueue(Queue *q, Player *p) {
    p->next = NULL;
    if (q->tail == NULL) {
        q->head = q->tail = p;
    } else {
        q->tail->next = p;
        q->tail = p;
    }
}

Player* dequeue(Queue *q) {
    if (q->head == NULL) return NULL;
    Player *p = q->head;
    q->head = q->head->next;
    if (q->head == NULL) q->tail = NULL;
    p->next = NULL;
    return p;
}

// ------------------ Utility Functions ------------------
int gcd(int a, int b) {
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

int containsDigit(int num, int digit) {
    while (num > 0) {
        if (num % 10 == digit) return 1;
        num /= 10;
    }
    return 0;
}

// Check scoring rule: GCD digits appear in either number
int checkScore(int num1, int num2) {
    int g = gcd(num1, num2);
    int temp = g;
    while (temp > 0) {
        int d = temp % 10;
        if (containsDigit(num1, d) || containsDigit(num2, d)) {
            return 1;
        }
        temp /= 10;
    }
    return 0;
}

// Insert into LG (ordered by score)
void insertLG(Player *p) {
    p->next = NULL;
    if (LG == NULL || p->score > LG->score) {
        p->next = LG;
        LG = p;
    } else {
        Player *cur = LG;
        while (cur->next != NULL && cur->next->score >= p->score) {
            cur = cur->next;
        }
        p->next = cur->next;
        cur->next = p;
    }
}

// Insert into LP
void insertLP(Player *p) {
    p->next = LP;
    LP = p;
}

// ------------------ Input Validation ------------------
int getPositiveInt(const char *prompt) {
    int value;
    do {
        printf("%s", prompt);
        if (scanf("%d", &value) != 1) {
            while(getchar() != '\n'); // clear buffer
            printf("Invalid input. Please enter a number.\n");
            value = -1;
        } else if (value <= 0) {
            printf("Value must be positive.\n");
            value = -1;
        }
    } while (value <= 0);
    return value;
}

// ------------------ Display State ------------------
void displayState(Queue *F, Queue *F1, Queue *F3) {
    printf("\n--- Current State ---\n");
    printf("Queue F: ");
    Player *cur = F->head;
    while (cur) { printf("%s ", cur->name); cur = cur->next; }
    printf("\nQueue F1: ");
    cur = F1->head;
    while (cur) { printf("%s ", cur->name); cur = cur->next; }
    printf("\nQueue F3: ");
    cur = F3->head;
    while (cur) { printf("%s ", cur->name); cur = cur->next; }
    printf("\nLG (Winners): ");
    cur = LG;
    while (cur) { printf("%s(%d) ", cur->name, cur->score); cur = cur->next; }
    printf("\nLP (Losers): ");
    cur = LP;
    while (cur) { printf("%s ", cur->name); cur = cur->next; }
    printf("\n---------------------\n");
}

// ------------------ Player Selection ------------------
Player* selectPlayer(Queue *F, Queue *F1, Queue *F3, int round) {
    if (round == 1) { // First round always from F
        return dequeue(F);
    }
    if (F1->head != NULL) return dequeue(F1);
    if (F->head != NULL) return dequeue(F);
    if (F3->head != NULL) return dequeue(F3);
    return NULL;
}

// ------------------ Play Round ------------------
void playRound(Player *p1, Player *p2, Queue *F, Queue *F1, Queue *F3) {
    int score1 = 0, score2 = 0;
    int valuesGenerated = 0;

    // Timestamp start
    time_t startTime = time(NULL);
    printf("Round Start Time: %s", ctime(&startTime));

    while (abs(score1 - score2) < 3 && valuesGenerated < 16) {
        int n1, n2;

        // Player 1 enters two numbers
        printf("%s, enter two numbers:\n", p1->name);
        n1 = getPositiveInt("First number: ");
        n2 = getPositiveInt("Second number: ");
        if (checkScore(n1, n2)) score1++;

        // Player 2 enters two numbers
        printf("%s, enter two numbers:\n", p2->name);
        n1 = getPositiveInt("First number: ");
        n2 = getPositiveInt("Second number: ");
        if (checkScore(n1, n2)) score2++;

        valuesGenerated += 2;
    }

    printf("Result: %s(%d) vs %s(%d)\n", p1->name, score1, p2->name, score2);

    // Timestamp end
    time_t endTime = time(NULL);
    printf("Round End Time: %s", ctime(&endTime));

    if (score1 > score2) {
        p1->wins++; p2->losses++;
        p1->score += score1; p2->score += score2;

        if (p2->losses >= 2) insertLP(p2);
        else enqueue(F3, p2);

        if (p1->wins >= 2) insertLG(p1);
        else enqueue(F1, p1); // Winner goes to F1
    } else if (score2 > score1) {
        p2->wins++; p1->losses++;
        p1->score += score1; p2->score += score2;

        if (p1->losses >= 2) insertLP(p1);
        else enqueue(F3, p1);

        if (p2->wins >= 2) insertLG(p2);
        else enqueue(F1, p2); // Winner goes to F1
    } else {
        enqueue(F, p1);
        enqueue(F, p2);
    }
}

// ------------------ Game Over Check ------------------
int gameOver(Queue *F, Queue *F1, Queue *F3, int round, int numPlayers) {
    if (F->head == NULL && F1->head == NULL && F3->head == NULL) {
        return 1;
    }
    if (round >= 2 * numPlayers) {
        return 1;
    }
    return 0;
}

// ------------------ Print Top 3 Winners ------------------
void printTopWinners() {
    printf("\n=== TOP 3 WINNERS ===\n");
    Player *cur = LG;
    int count = 0;
    while (cur != NULL && count < 3) {
        printf("%d. %s with %d points\n", count + 1, cur->name, cur->score);
        cur = cur->next;
        count++;
    }
    if (count == 0) {
        printf("No winners recorded.\n");
    }
}

// ------------------ Main ------------------
int main() {
    srand(time(NULL));

    Queue F = {NULL, NULL}, F1 = {NULL, NULL}, F3 = {NULL, NULL};

    int numPlayers = getPositiveInt("Enter number of players (>=2): ");
    while (numPlayers < 2) {
        printf("At least 2 players are required.\n");
        numPlayers = getPositiveInt("Enter number of players (>=2): ");
    }

    for (int i = 1; i <= numPlayers; i++) {
        Player *p = (Player*)malloc(sizeof(Player));
        p->id = i;
        printf("Enter name for Player %d: ", i);
        scanf("%s", p->name);
        p->age = getPositiveInt("Enter age: ");
                enqueue(&F, p);
    }

    int round = 1;

    while (!gameOver(&F, &F1, &F3, round, numPlayers)) {
        Player *p1, *p2;

        if (round == 1) {
            // First round: both from F
            p1 = dequeue(&F);
            p2 = dequeue(&F);
        } else {
            // Select players based on priority: F1 > F > F3
            p1 = selectPlayer(&F, &F1, &F3, round);
            p2 = selectPlayer(&F, &F1, &F3, round);
        }

        if (p1 == NULL || p2 == NULL) break;

        printf("\n--- Round %d ---\n", round);
        playRound(p1, p2, &F, &F1, &F3);
        displayState(&F, &F1, &F3);
        round++;
    }

    // Final rule: if only one player remains in a queue, send them to LP
    if (F.head != NULL && F.head->next == NULL) {
        Player *last = dequeue(&F);
        insertLP(last);
    }
    if (F1.head != NULL && F1.head->next == NULL) {
        Player *last = dequeue(&F1);
        insertLP(last);
    }
    if (F3.head != NULL && F3.head->next == NULL) {
        Player *last = dequeue(&F3);
        insertLP(last);
    }

    printf("\n=== GAME OVER ===\n");
    displayState(&F, &F1, &F3);

    // Print Top 3 Winners
    printTopWinners();

    return 0;
}