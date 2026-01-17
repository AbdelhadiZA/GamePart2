#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct Player {
    int id;
    char name[30];
    int age;
    int score;
    int wins;
    int losses;
    int consecutiveWins;
    int consecutiveLosses;
    struct Player *next;
} Player;

typedef struct Queue {
    Player *head;
    Player *tail;
} Queue;

Player *LG = NULL; // Winners list
Player *LP = NULL; // Losers list

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

int sumDigits(int num) {
    int sum = 0;
    while (num > 0) {
        sum += num % 10;
        num /= 10;
    }
    return sum;
}

int checkScore(int n1, int n2) {
    int g = gcd(n1, n2);
    while (g > 0) {
        int d = g % 10;
        if (containsDigit(n1, d) || containsDigit(n2, d)) return 1;
        g /= 10;
    }
    return 0;
}

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

void insertLP(Player *p) {
    p->next = LP;
    LP = p;
}

int getPositiveInt(const char *prompt) {
    int value;
    do {
        printf("%s", prompt);
        if (scanf("%d", &value) != 1) {
            while(getchar() != '\n');
            printf("Invalid input. Please enter a number.\n");
            value = -1;
        } else if (value <= 0) {
            printf("Value must be positive.\n");
            value = -1;
        }
    } while (value <= 0);
    return value;
}

void displayState(Queue *F, Queue *F1, Queue *F3) {
    printf("\n--- Current State ---\n");
    printf("Queue F: ");
    for (Player *cur = F->head; cur; cur = cur->next) printf("%s ", cur->name);
    printf("\nQueue F1: ");
    for (Player *cur = F1->head; cur; cur = cur->next) printf("%s ", cur->name);
    printf("\nQueue F3: ");
    for (Player *cur = F3->head; cur; cur = cur->next) printf("%s ", cur->name);
    printf("\nLG (Winners): ");
    for (Player *cur = LG; cur; cur = cur->next) printf("%s(%d) ", cur->name, cur->score);
    printf("\nLP (Losers): ");
    for (Player *cur = LP; cur; cur = cur->next) printf("%s ", cur->name);
    printf("\n---------------------\n");
}

Player* selectPlayer(Queue *F, Queue *F1, Queue *F3) {
    if (F1->head != NULL) return dequeue(F1);
    if (F->head != NULL) return dequeue(F);
    if (F3->head != NULL) return dequeue(F3);
    return NULL;
}

void playRoundPart1(Player *p1, Player *p2, Queue *F, Queue *F1, Queue *F3) {
    int score1 = 0, score2 = 0, valuesGenerated = 0;
    getchar(); // clear buffer
    while (abs(score1 - score2) < 3 && valuesGenerated < 12) {
        printf("%s, press Enter to generate a number...\n", (valuesGenerated % 2 == 0) ? p1->name : p2->name);
        getchar();
        int val = rand() % 1000000;
        printf("%s generated number: %d\n", (valuesGenerated % 2 == 0) ? p1->name : p2->name, val);
        if (sumDigits(val) % 5 == 0) {
            if (valuesGenerated % 2 == 0) score1++;
            else score2++;
        }
        valuesGenerated++;
    }
    printf("Result (Part I): %s(%d) vs %s(%d)\n", p1->name, score1, p2->name, score2);
    p1->score += score1; p2->score += score2;
    if (score1 > score2) {
        p1->wins++; p1->consecutiveWins++; p1->consecutiveLosses = 0;
        p2->losses++; p2->consecutiveLosses++; p2->consecutiveWins = 0;
        if (p1->wins >= 5) insertLG(p1);
        else if (p1->consecutiveWins >= 3) enqueue(F1, p1);
        else enqueue(F, p1);
        if (p2->losses >= 5) insertLP(p2);
        else if (p2->consecutiveLosses >= 3) enqueue(F3, p2);
        else enqueue(F, p2);
    } else if (score2 > score1) {
        p2->wins++; p2->consecutiveWins++; p2->consecutiveLosses = 0;
        p1->losses++; p1->consecutiveLosses++; p1->consecutiveWins = 0;
        if (p2->wins >= 5) insertLG(p2);
        else if (p2->consecutiveWins >= 3) enqueue(F1, p2);
        else enqueue(F, p2);
        if (p1->losses >= 5) insertLP(p1);
        else if (p1->consecutiveLosses >= 3) enqueue(F3, p1);
        else enqueue(F, p1);
    } else {
        enqueue(F, p1);
        enqueue(F, p2);
    }
}

void playRoundPart2(Player *p1, Player *p2, Queue *F, Queue *F1, Queue *F3) {
    int score1 = 0, score2 = 0, valuesGenerated = 0;
    getchar(); // clear buffer
    while (abs(score1 - score2) < 3 && valuesGenerated < 16) {
        printf("%s, press Enter to generate two numbers...\n", (valuesGenerated % 2 == 0) ? p1->name : p2->name);
        getchar();
        int n1 = rand() % 1000 + 1, n2 = rand() % 1000 + 1;
        printf("%s generated numbers: %d and %d\n", (valuesGenerated % 2 == 0) ? p1->name : p2->name, n1, n2);
        if (checkScore(n1, n2)) {
            if (valuesGenerated % 2 == 0) score1++;
            else score2++;
        }
        valuesGenerated++;
    }
    printf("Result (Part II): %s(%d) vs %s(%d)\n", p1->name, score1, p2->name, score2);
    p1->score += score1; p2->score += score2;
    if (score1 > score2) {
        p1->wins++; p1->consecutiveWins++; p1->consecutiveLosses = 0;
        p2->losses++; p2->consecutiveLosses++; p2->consecutiveWins = 0;
        if (p1->consecutiveWins >= 2) insertLG(p1);
        else enqueue(F1, p1);
        if (p2->losses >= 2) insertLP(p2);
        else enqueue(F3, p2);
    } else if (score2 > score1) {
        p2->wins++; p2->consecutiveWins++; p2->consecutiveLosses = 0;
        p1->losses++;
                p1->consecutiveWins = 0;

        if (p2->consecutiveWins >= 2) insertLG(p2);
        else enqueue(F1, p2);

        if (p1->losses >= 2) insertLP(p1);
        else enqueue(F3, p1);
    } else {
        enqueue(F, p1);
        enqueue(F, p2);
    }
}

void printTopWinners() {
    printf("\n=== TOP 3 WINNERS ===\n");
    Player *cur = LG;
    int count = 0;
    while (cur != NULL && count < 3) {
        printf("%d. %s with %d points\n", count + 1, cur->name, cur->score);
        cur = cur->next;
        count++;
    }
    if (count == 0) printf("No winners recorded.\n");
}

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
        p->score = p->wins = p->losses = p->consecutiveWins = p->consecutiveLosses = 0;
        p->next = NULL;
        enqueue(&F, p);
    }

    int round = 1, maxRoundsPart1 = 3 * numPlayers, maxRoundsPart2 = 2 * numPlayers;

    // ----------- Part I Strategy -----------
    while (round <= maxRoundsPart1 && (F.head || F1.head || F3.head)) {
        Player *p1 = selectPlayer(&F, &F1, &F3);
        Player *p2 = selectPlayer(&F, &F1, &F3);
        if (!p1 || !p2) break;
        printf("\n--- Part I Round %d ---\n", round);
        playRoundPart1(p1, p2, &F, &F1, &F3);
        displayState(&F, &F1, &F3);
        round++;
    }

    // ----------- Part II Strategy -----------
    round = 1;
    while (round <= maxRoundsPart2 && (F.head || F1.head || F3.head)) {
        Player *p1 = selectPlayer(&F, &F1, &F3);
        Player *p2 = selectPlayer(&F, &F1, &F3);
        if (!p1 || !p2) break;
        printf("\n--- Part II Round %d ---\n", round);
        playRoundPart2(p1, p2, &F, &F1, &F3);
        displayState(&F, &F1, &F3);
        round++;
    }

    // ----------- End of Game Forced Placement -----------
    printf("\n--- Forced End of Game Placement ---\n");
    while (F1.head != NULL) {
        Player *p = dequeue(&F1);
        if (p->score == 0) p->score = p->wins; // ensure score preserved
        insertLG(p);
    }
    while (F.head != NULL) {
        Player *p = dequeue(&F);
        insertLP(p);
    }
    while (F3.head != NULL) {
        Player *p = dequeue(&F3);
        insertLP(p);
    }

    printf("\n=== GAME OVER ===\n");
    displayState(&F, &F1, &F3);
    printTopWinners();

    return 0;
}
