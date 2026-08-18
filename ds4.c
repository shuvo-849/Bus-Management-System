#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define SEAT_LIMIT 50
#define FEEDBACK_LIMIT 20
#define ADMIN_PASSWORD "admin123"
#define MAX_LOGIN_TRY 3

#define MAX_MULTI_SEAT 5

#define BUS_FILE       "buses.txt"
#define PASSENGER_FILE "passengers.txt"

struct QueueNode
{
    char passengerName[30];
    struct QueueNode *next;
};

struct Bus
{
    int id;
    char name[20];
    char from[20];
    char to[20];
    char time[30];
    char date[30];
    int distance;
    int seatCount;
    int seatStatus[SEAT_LIMIT];
    int starList[FEEDBACK_LIMIT];
    char noteList[FEEDBACK_LIMIT][200];
    int feedbackCount;
    struct QueueNode *queueFront;
    struct QueueNode *queueRear;
    struct Bus *next;
};

struct Passenger
{
    char email[50];
    char password[30];
    struct Passenger *next;
};

struct Bus *busHead = NULL;
struct Passenger *passengerHead = NULL;
char currentPassengerEmail[50] = "";

void addBus(char name[], int id, char from[], char to[], int distance,
            int seatCount, char time[], char date[]);
struct Bus *searchBus(int id);
void readLine(char *buffer, int size);
void clearInputLine(void);
void enqueuePassenger(struct Bus *bus, char name[]);
int dequeuePassenger(struct Bus *bus, char outName[]);

void showPassengerMenu(void);
void passengerPanel(void);
void searchRoute(void);
void viewSeatMap(void);
void bookMultipleSeats(void);
void cancelMultipleSeats(void);
void addFeedback(void);
void viewFeedback(void);
void viewQueue(void);

int emailExists(char email[]);
void registerPassenger(void);
void loginPassenger(void);
void passengerAuthMenu(void);
void savePassengersToFile(void);
void loadPassengersFromFile(void);

int adminLogin(void);
void showAdminMenu(void);
void adminPanel(void);
void addBusAdmin(void);
void deleteBusAdmin(void);
void updateBusAdmin(void);
void viewAllBusesAdmin(void);

void saveBusesToFile(void);
int  loadBusesFromFile(void);

void addBus(char name[], int id, char from[], char to[], int distance,
            int seatCount, char time[], char date[])
{
    struct Bus *node = (struct Bus *)malloc(sizeof(struct Bus));

    node->id = id;
    strcpy(node->name, name);
    strcpy(node->from, from);
    strcpy(node->to, to);
    strcpy(node->time, time);
    strcpy(node->date, date);
    node->distance = distance;
    node->seatCount = seatCount;
    node->feedbackCount = 0;
    node->queueFront = NULL;
    node->queueRear = NULL;
    node->next = NULL;

    for (int i = 0; i < seatCount; i++)
        node->seatStatus[i] = 0;

    if (busHead == NULL)
    {
        busHead = node;
        return;
    }

    struct Bus *temp = busHead;
    while (temp->next != NULL)
        temp = temp->next;
    temp->next = node;
}

struct Bus *searchBus(int id)
{
    struct Bus *temp = busHead;
    while (temp != NULL)
    {
        if (temp->id == id)
            return temp;
        temp = temp->next;
    }
    return NULL;
}

void readLine(char *buffer, int size)
{
    (void)size;

    scanf(" %[^\n]", buffer);
}

void clearInputLine(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

void enqueuePassenger(struct Bus *bus, char name[])
{
    struct QueueNode *node = (struct QueueNode *)malloc(sizeof(struct QueueNode));
    strcpy(node->passengerName, name);
    node->next = NULL;

    if (bus->queueFront == NULL)
    {
        bus->queueFront = node;
        bus->queueRear = node;
        return;
    }

    bus->queueRear->next = node;
    bus->queueRear = node;
}

int dequeuePassenger(struct Bus *bus, char outName[])
{
    if (bus->queueFront == NULL)
        return 0;

    struct QueueNode *temp = bus->queueFront;
    strcpy(outName, temp->passengerName);

    bus->queueFront = bus->queueFront->next;
    if (bus->queueFront == NULL)
        bus->queueRear = NULL;

    free(temp);
    return 1;
}

void showPassengerMenu(void)
{
    printf("\n===== Passenger Panel (%s) =====\n", currentPassengerEmail);
    printf("1. Search Route\n");
    printf("2. Book Seat(s) [up to %d]\n", MAX_MULTI_SEAT);
    printf("3. Cancel Seat(s) [up to %d]\n", MAX_MULTI_SEAT);
    printf("4. Seat Map\n");
    printf("5. Give Feedback\n");
    printf("6. View Feedback\n");
    printf("7. View Waiting Queue\n");
    printf("8. Logout\n");
    printf("Enter choice: ");
}

void passengerPanel(void)
{
    int choice;

    do
    {
        showPassengerMenu();
        if (scanf("%d", &choice) != 1)
        {
            clearInputLine();
            printf("Invalid input, numbers only.\n");
            continue;
        }

        switch (choice)
        {
        case 1:
            searchRoute();
            break;
        case 2:
            bookMultipleSeats();
            break;
        case 3:
            cancelMultipleSeats();
            break;
        case 4:
            viewSeatMap();
            break;
        case 5:
            addFeedback();
            break;
        case 6:
            viewFeedback();
            break;
        case 7:
            viewQueue();
            break;
        case 8:
            printf("Logging out %s...\n", currentPassengerEmail);
            break;
        default:
            printf("Invalid choice, try again.\n");
        }

    } while (choice != 8);
}

void searchRoute(void)
{
    char from[20], to[20];

    printf("From (CAPITAL letters): ");
    scanf("%s", from);
    printf("To (CAPITAL letters): ");
    scanf("%s", to);

    int found = 0;
    struct Bus *temp = busHead;

    while (temp != NULL)
    {
        if (strcmp(temp->from, from) == 0 && strcmp(temp->to, to) == 0)
        {
            found++;
            int freeSeats = 0;
            for (int s = 0; s < temp->seatCount; s++)
            {
                if (temp->seatStatus[s] == 0)
                    freeSeats++;
            }

            printf("\nBus: %s (ID %d)\n", temp->name, temp->id);
            printf("Route: %s -> %s | %d km\n", temp->from, temp->to, temp->distance);
            printf("Departs: %s on %s\n", temp->time, temp->date);
            printf("Seats: %d total, %d free\n\n", temp->seatCount, freeSeats);
        }
        temp = temp->next;
    }

    if (found == 0)
        printf("No matching route found.\n\n");
}

void viewSeatMap(void)
{
    int id;
    struct Bus *bus;

    while (1)
    {
        printf("Bus ID (0 to go back): ");
        if (scanf("%d", &id) != 1)
        {
            clearInputLine();
            printf("Invalid input, numbers only. Try again.\n");
            continue;
        }
        if (id == 0)
            return;

        bus = searchBus(id);
        if (bus == NULL)
        {
            printf("Bus not found! Try again.\n");
            continue;
        }
        break;
    }

    printf("\n%s (ID %d)\n", bus->name, bus->id);
    for (int s = 0; s < bus->seatCount; s++)
        printf("Seat %2d -> %s\n", s + 1, bus->seatStatus[s] ? "Occupied" : "Free");
}

void bookMultipleSeats(void)
{
    struct Bus *bus = NULL;
    int id;

    while (1)
    {
        printf("\nEnter Bus ID (0 to go back): ");
        if (scanf("%d", &id) != 1)
        {
            clearInputLine();
            printf("Invalid input, numbers only. Try again.\n");
            continue;
        }
        if (id == 0)
        {
            printf("Returning to passenger menu...\n");
            return;
        }

        bus = searchBus(id);
        if (bus == NULL)
        {
            printf("No bus found with ID %d. Try again.\n", id);
            continue;
        }
        break;
    }

    int seatsWanted;
    while (1)
    {
        printf("How many seats do you want to book (1-%d, 0 to go back): ", MAX_MULTI_SEAT);
        if (scanf("%d", &seatsWanted) != 1)
        {
            clearInputLine();
            printf("Invalid input, numbers only. Try again.\n");
            continue;
        }
        if (seatsWanted == 0)
        {
            printf("Returning to passenger menu...\n");
            return;
        }
        if (seatsWanted < 1 || seatsWanted > MAX_MULTI_SEAT)
        {
            printf("You can only book between 1 and %d seats at a time. Try again.\n", MAX_MULTI_SEAT);
            continue;
        }
        break;
    }

    int freeSeats[SEAT_LIMIT];
    int freeCount = 0;
    for (int s = 0; s < bus->seatCount; s++)
        if (bus->seatStatus[s] == 0)
            freeSeats[freeCount++] = s;

    if (freeCount < seatsWanted)
    {
        printf("Only %d seat(s) free on %s, not enough for %d seat(s) requested.\n",
               freeCount, bus->name, seatsWanted);

        int qChoice;
        while (1)
        {
            printf("1. Join waiting queue instead\n2. Go back\nChoice: ");
            if (scanf("%d", &qChoice) != 1)
            {
                clearInputLine();
                printf("Invalid input.\n");
                continue;
            }
            if (qChoice == 1 || qChoice == 2)
                break;
            printf("Invalid input, enter 1 or 2.\n");
        }

        if (qChoice == 2)
        {
            printf("Returning to passenger menu...\n");
            return;
        }

        clearInputLine();
        for (int i = 0; i < seatsWanted; i++)
        {
            char name[30];
            printf("Enter name for waiting-queue passenger %d/%d: ", i + 1, seatsWanted);
            readLine(name, sizeof(name));
            enqueuePassenger(bus, name);
        }
        printf("%d passenger(s) added to the waiting queue for %s.\n", seatsWanted, bus->name);

        return;
    }

    for (int i = 0; i < seatsWanted; i++)
    {
        int pos = freeSeats[i];
        bus->seatStatus[pos] = 1;
        printf("Seat %d BOOKED on %s.\n", pos + 1, bus->name);
    }

    saveBusesToFile();
}

void cancelMultipleSeats(void)
{
    struct Bus *bus = NULL;
    int id;

    while (1)
    {
        printf("\nEnter Bus ID (0 to go back): ");
        if (scanf("%d", &id) != 1)
        {
            clearInputLine();
            printf("Invalid input, numbers only. Try again.\n");
            continue;
        }
        if (id == 0)
        {
            printf("Returning to passenger menu...\n");
            return;
        }

        bus = searchBus(id);
        if (bus == NULL)
        {
            printf("No bus found with ID %d. Try again.\n", id);
            continue;
        }
        break;
    }

    int seatsWanted;
    while (1)
    {
        printf("How many seats do you want to cancel (1-%d, 0 to go back): ", MAX_MULTI_SEAT);
        if (scanf("%d", &seatsWanted) != 1)
        {
            clearInputLine();
            printf("Invalid input, numbers only. Try again.\n");
            continue;
        }
        if (seatsWanted == 0)
        {
            printf("Returning to passenger menu...\n");
            return;
        }
        if (seatsWanted < 1 || seatsWanted > MAX_MULTI_SEAT)
        {
            printf("You can only cancel between 1 and %d seats at a time. Try again.\n", MAX_MULTI_SEAT);
            continue;
        }
        break;
    }

    int seatNumbers[MAX_MULTI_SEAT];

    for (int i = 0; i < seatsWanted; i++)
    {
        while (1)
        {
            int sn;
            printf("Seat number %d/%d to cancel (1-%d, 0 to go back): ", i + 1, seatsWanted, bus->seatCount);
            if (scanf("%d", &sn) != 1)
            {
                clearInputLine();
                printf("Invalid input, numbers only. Try again.\n");
                continue;
            }
            if (sn == 0)
            {
                printf("Cancelled the whole operation, no seats changed. Returning...\n");
                return;
            }
            if (sn < 1 || sn > bus->seatCount)
            {
                printf("Seat number out of range (1-%d). Try again.\n", bus->seatCount);
                continue;
            }
            if (bus->seatStatus[sn - 1] == 0)
            {
                printf("Seat %d is already free. Pick a booked seat.\n", sn);
                continue;
            }

            int duplicate = 0;
            for (int k = 0; k < i; k++)
            {
                if (seatNumbers[k] == sn)
                {
                    duplicate = 1;
                    break;
                }
            }
            if (duplicate)
            {
                printf("You already picked seat %d. Choose a different seat.\n", sn);
                continue;
            }

            seatNumbers[i] = sn;
            break;
        }
    }

    for (int i = 0; i < seatsWanted; i++)
    {
        int pos = seatNumbers[i] - 1;
        bus->seatStatus[pos] = 0;
        printf("Seat %d is now FREE on %s.\n", seatNumbers[i], bus->name);

        char nextName[30];
        if (dequeuePassenger(bus, nextName))
        {
            bus->seatStatus[pos] = 1;
            printf("%s was waiting in queue, auto-booked to seat %d.\n", nextName, seatNumbers[i]);
        }
    }

    saveBusesToFile();
}

void addFeedback(void)
{
    int id, stars;
    char note[200];
    struct Bus *bus;

    while (1)
    {
        printf("Bus ID (0 to go back): ");
        if (scanf("%d", &id) != 1)
        {
            clearInputLine();
            printf("Invalid input, numbers only. Try again.\n");
            continue;
        }
        if (id == 0)
            return;

        bus = searchBus(id);
        if (bus == NULL)
        {
            printf("Bus not found! Try again.\n");
            continue;
        }
        if (bus->feedbackCount >= FEEDBACK_LIMIT)
        {
            printf("Feedback limit reached for this bus.\n\n");
            return;
        }
        break;
    }

    while (1)
    {
        printf("Stars for %s (1-5): ", bus->name);
        if (scanf("%d", &stars) != 1)
        {
            clearInputLine();
            printf("Invalid input, numbers only. Try again.\n");
            continue;
        }
        if (stars < 1 || stars > 5)
        {
            printf("Stars must be between 1 and 5. Try again.\n");
            continue;
        }
        break;
    }
    getchar();
    printf("Comment: ");
    readLine(note, sizeof(note));

    int r = bus->feedbackCount;
    bus->starList[r] = stars;
    strcpy(bus->noteList[r], note);
    bus->feedbackCount++;

    printf("Feedback saved. Thanks!\n\n");
}

void viewFeedback(void)
{
    int id;
    struct Bus *bus;

    while (1)
    {
        printf("Bus ID (0 to go back): ");
        if (scanf("%d", &id) != 1)
        {
            clearInputLine();
            printf("Invalid input, numbers only. Try again.\n");
            continue;
        }
        if (id == 0)
            return;

        bus = searchBus(id);
        if (bus == NULL)
        {
            printf("Bus not found! Try again.\n");
            continue;
        }
        break;
    }

    printf("\n---- Feedback: %s ----\n", bus->name);

    if (bus->feedbackCount == 0)
    {
        printf("No feedback yet.\n\n");
        return;
    }

    for (int r = 0; r < bus->feedbackCount; r++)
        printf("[%d] %d/5 stars - %s\n", r + 1, bus->starList[r], bus->noteList[r]);
    printf("\n");
}

void viewQueue(void)
{
    int id;
    struct Bus *bus;

    while (1)
    {
        printf("Bus ID (0 to go back): ");
        if (scanf("%d", &id) != 1)
        {
            clearInputLine();
            printf("Invalid input, numbers only. Try again.\n");
            continue;
        }
        if (id == 0)
            return;

        bus = searchBus(id);
        if (bus == NULL)
        {
            printf("Bus not found! Try again.\n");
            continue;
        }
        break;
    }

    printf("\n---- Waiting Queue: %s ----\n", bus->name);

    if (bus->queueFront == NULL)
    {
        printf("Queue is empty.\n\n");
        return;
    }

    int position = 1;
    struct QueueNode *temp = bus->queueFront;
    while (temp != NULL)
    {
        printf("%d. %s\n", position, temp->passengerName);
        position++;
        temp = temp->next;
    }
    printf("\n");
}

int emailExists(char email[])
{
    struct Passenger *t = passengerHead;
    while (t != NULL)
    {
        if (strcmp(t->email, email) == 0)
            return 1;
        t = t->next;
    }
    return 0;
}

void registerPassenger(void)
{
    char email[50], pass[30], pass2[30];

    clearInputLine();

    while (1)
    {
        printf("Enter email to register (0 to go back): ");
        readLine(email, sizeof(email));

        if (strcmp(email, "0") == 0)
        {
            printf("Returning...\n");
            return;
        }
        if (strchr(email, '@') == NULL)
        {
            printf("Invalid email, it must contain '@'. Try again.\n");
            continue;
        }
        if (emailExists(email))
        {
            printf("This email is already registered. Try Login instead.\n");
            continue;
        }
        break;
    }

    while (1)
    {
        printf("Create a password (min 4 characters): ");
        readLine(pass, sizeof(pass));

        if (strlen(pass) < 4)
        {
            printf("Password too short, must be at least 4 characters.\n");
            continue;
        }

        printf("Confirm password: ");
        readLine(pass2, sizeof(pass2));

        if (strcmp(pass, pass2) != 0)
        {
            printf("Passwords do not match. Try again.\n");
            continue;
        }
        break;
    }

    struct Passenger *p = (struct Passenger *)malloc(sizeof(struct Passenger));
    strcpy(p->email, email);
    strcpy(p->password, pass);
    p->next = passengerHead;
    passengerHead = p;

    savePassengersToFile();

    strcpy(currentPassengerEmail, email);
    printf("Registration successful! Logged in as %s.\n", email);
}

void loginPassenger(void)
{
    char email[50], pass[30];
    int tries = 0;

    clearInputLine();

    while (tries < MAX_LOGIN_TRY)
    {
        printf("\nEmail: ");
        readLine(email, sizeof(email));
        printf("Password: ");
        readLine(pass, sizeof(pass));

        struct Passenger *t = passengerHead;
        while (t != NULL)
        {
            if (strcmp(t->email, email) == 0 && strcmp(t->password, pass) == 0)
            {
                strcpy(currentPassengerEmail, email);
                printf("Login successful! Welcome, %s.\n", email);
                return;
            }
            t = t->next;
        }

        tries++;
        printf("Wrong email or password! (%d/%d tries used)\n", tries, MAX_LOGIN_TRY);
    }

    printf("Too many wrong attempts. Returning to menu.\n");
}

void passengerAuthMenu(void)
{
    int choice;

    while (1)
    {
        printf("\n===== Passenger Login =====\n");
        printf("1. Login\n");
        printf("2. Register\n");
        printf("3. Back to Main Menu\n");
        printf("Enter choice: ");

        if (scanf("%d", &choice) != 1)
        {
            clearInputLine();
            printf("Invalid input, numbers only.\n");
            continue;
        }

        if (choice == 1)
        {
            loginPassenger();
            if (strlen(currentPassengerEmail) > 0)
            {
                passengerPanel();
                currentPassengerEmail[0] = '\0';
            }
        }
        else if (choice == 2)
        {
            registerPassenger();
            if (strlen(currentPassengerEmail) > 0)
            {
                passengerPanel();
                currentPassengerEmail[0] = '\0';
            }
        }
        else if (choice == 3)
        {
            printf("Returning to main menu...\n");
            return;
        }
        else
        {
            printf("Invalid choice, try again.\n");
        }
    }
}

int adminLogin(void)
{
    char inputPass[30];
    int tries = 0;

    while (tries < MAX_LOGIN_TRY)
    {
        printf("\nEnter Admin Password: ");
        readLine(inputPass, sizeof(inputPass));

        if (strcmp(inputPass, ADMIN_PASSWORD) == 0)
        {
            printf("Login successful! Welcome, Admin.\n");
            return 1;
        }

        tries++;
        printf("Wrong password! (%d/%d tries used)\n", tries, MAX_LOGIN_TRY);
    }

    printf("Too many wrong attempts. Access denied.\n");
    return 0;
}

void showAdminMenu(void)
{
    printf("\n===== Admin Panel =====\n");
    printf("1. Add Bus\n");
    printf("2. Delete Bus\n");
    printf("3. Update Bus Info\n");
    printf("4. View All Buses (Seat Status)\n");
    printf("5. View Feedback (any bus)\n");
    printf("6. View Waiting Queue (any bus)\n");
    printf("7. Back to Main Menu\n");
    printf("Enter choice: ");
}

void adminPanel(void)
{
    int choice;

    do
    {
        showAdminMenu();
        if (scanf("%d", &choice) != 1)
        {
            clearInputLine();
            printf("Invalid input, numbers only.\n");
            continue;
        }

        switch (choice)
        {
        case 1:
            addBusAdmin();
            break;
        case 2:
            deleteBusAdmin();
            break;
        case 3:
            updateBusAdmin();
            break;
        case 4:
            viewAllBusesAdmin();
            break;
        case 5:
            viewFeedback();
            break;
        case 6:
            viewQueue();
            break;
        case 7:
            printf("Logging out of Admin Panel...\n");
            break;
        default:
            printf("Invalid choice, try again.\n");
        }

    } while (choice != 7);
}

void addBusAdmin(void)
{
    char name[20], from[20], to[20], time[30], date[30];
    int id, distance, seatCount;

    while (1)
    {
        printf("Bus ID (number, 0 to go back): ");
        if (scanf("%d", &id) != 1)
        {
            clearInputLine();
            printf("Invalid input, numbers only. Try again.\n");
            continue;
        }
        if (id == 0)
            return;
        if (searchBus(id) != NULL)
        {
            printf("A bus with this ID already exists! Try again.\n");
            continue;
        }
        break;
    }

    getchar();
    printf("Bus Name: ");
    readLine(name, sizeof(name));

    printf("From (CAPITAL letters): ");
    readLine(from, sizeof(from));

    printf("To (CAPITAL letters): ");
    readLine(to, sizeof(to));

    while (1)
    {
        printf("Distance (km): ");
        if (scanf("%d", &distance) != 1)
        {
            clearInputLine();
            printf("Invalid input, numbers only. Try again.\n");
            continue;
        }
        break;
    }

    while (1)
    {
        printf("Seat Count (max %d): ", SEAT_LIMIT);
        if (scanf("%d", &seatCount) != 1)
        {
            clearInputLine();
            printf("Invalid input, numbers only. Try again.\n");
            continue;
        }
        if (seatCount <= 0 || seatCount > SEAT_LIMIT)
        {
            printf("Invalid seat count, must be 1-%d. Try again.\n", SEAT_LIMIT);
            continue;
        }
        break;
    }

    getchar();
    printf("Departure Time (e.g. 10:00 AM): ");
    readLine(time, sizeof(time));

    printf("Date (e.g. 24-07-2026): ");
    readLine(date, sizeof(date));

    addBus(name, id, from, to, distance, seatCount, time, date);
    printf("Bus '%s' (ID %d) added successfully!\n\n", name, id);

    saveBusesToFile();
}

void deleteBusAdmin(void)
{
    int id;
    struct Bus *temp, *prev;

    while (1)
    {
        printf("Bus ID to delete (0 to go back): ");
        if (scanf("%d", &id) != 1)
        {
            clearInputLine();
            printf("Invalid input, numbers only. Try again.\n");
            continue;
        }
        if (id == 0)
            return;

        temp = busHead;
        prev = NULL;
        while (temp != NULL && temp->id != id)
        {
            prev = temp;
            temp = temp->next;
        }

        if (temp == NULL)
        {
            printf("Bus not found! Try again.\n");
            continue;
        }
        break;
    }

    struct QueueNode *q = temp->queueFront;
    while (q != NULL)
    {
        struct QueueNode *nextQ = q->next;
        free(q);
        q = nextQ;
    }

    if (prev == NULL)
        busHead = temp->next;
    else
        prev->next = temp->next;

    printf("Bus '%s' (ID %d) deleted.\n\n", temp->name, temp->id);
    free(temp);

    saveBusesToFile();
}

void updateBusAdmin(void)
{
    int id;
    struct Bus *bus;

    while (1)
    {
        printf("Bus ID to update (0 to go back): ");
        if (scanf("%d", &id) != 1)
        {
            clearInputLine();
            printf("Invalid input, numbers only. Try again.\n");
            continue;
        }
        if (id == 0)
            return;

        bus = searchBus(id);
        if (bus == NULL)
        {
            printf("Bus not found! Try again.\n");
            continue;
        }
        break;
    }

    getchar();
    printf("New Bus Name (%s): ", bus->name);
    readLine(bus->name, sizeof(bus->name));

    printf("New From (%s): ", bus->from);
    readLine(bus->from, sizeof(bus->from));

    printf("New To (%s): ", bus->to);
    readLine(bus->to, sizeof(bus->to));

    printf("New Departure Time (%s): ", bus->time);
    readLine(bus->time, sizeof(bus->time));

    printf("New Date (%s): ", bus->date);
    readLine(bus->date, sizeof(bus->date));

    printf("Bus (ID %d) updated.\n\n", bus->id);

    saveBusesToFile();
}

void viewAllBusesAdmin(void)
{
    if (busHead == NULL)
    {
        printf("No buses in the system.\n\n");
        return;
    }

    printf("\n%-4s %-14s %-10s %-10s %-8s %-6s %-6s\n",
           "ID", "Name", "From", "To", "Total", "Book", "Free");
    printf("---------------------------------------------------------------\n");

    struct Bus *temp = busHead;
    while (temp != NULL)
    {
        int booked = 0;
        for (int s = 0; s < temp->seatCount; s++)
        {
            if (temp->seatStatus[s] == 1)
                booked++;
        }
        int free_ = temp->seatCount - booked;

        printf("%-4d %-14s %-10s %-10s %-8d %-6d %-6d\n",
               temp->id, temp->name, temp->from, temp->to,
               temp->seatCount, booked, free_);

        temp = temp->next;
    }
    printf("\n");
}

void saveBusesToFile(void)
{
    FILE *fp = fopen(BUS_FILE, "w");
    if (fp == NULL)
    {
        printf("Warning: could not open %s for saving buses.\n", BUS_FILE);
        return;
    }

    struct Bus *temp = busHead;
    while (temp != NULL)
    {
        char seatBits[SEAT_LIMIT + 1];
        for (int s = 0; s < temp->seatCount; s++)
            seatBits[s] = temp->seatStatus[s] ? '1' : '0';
        seatBits[temp->seatCount] = '\0';

        fprintf(fp, "%d|%s|%s|%s|%s|%s|%d|%d|%s\n",
                temp->id, temp->name, temp->from, temp->to,
                temp->time, temp->date, temp->distance,
                temp->seatCount, seatBits);

        temp = temp->next;
    }

    fclose(fp);
}

int loadBusesFromFile(void)
{
    FILE *fp = fopen(BUS_FILE, "r");
    if (fp == NULL)
        return 0;

    char line[400];
    int count = 0;

    while (fgets(line, sizeof(line), fp) != NULL)
    {
        int id, distance, seatCount;
        char name[20], from[20], to[20], time[30], date[30];
        char seatBits[SEAT_LIMIT + 1];

        int fields = sscanf(line, "%d|%19[^|]|%19[^|]|%19[^|]|%29[^|]|%29[^|]|%d|%d|%50[^\n]",
                             &id, name, from, to, time, date,
                             &distance, &seatCount, seatBits);

        if (fields != 9)
            continue;

        addBus(name, id, from, to, distance, seatCount, time, date);

        struct Bus *bus = searchBus(id);
        if (bus != NULL)
        {
            for (int s = 0; s < seatCount; s++)
                bus->seatStatus[s] = (seatBits[s] == '1') ? 1 : 0;
        }

        count++;
    }

    fclose(fp);
    return count;
}

void savePassengersToFile(void)
{
    FILE *fp = fopen(PASSENGER_FILE, "w");
    if (fp == NULL)
    {
        printf("Warning: could not open %s for saving passengers.\n", PASSENGER_FILE);
        return;
    }

    struct Passenger *temp = passengerHead;
    while (temp != NULL)
    {
        fprintf(fp, "%s|%s\n", temp->email, temp->password);
        temp = temp->next;
    }

    fclose(fp);
}

void loadPassengersFromFile(void)
{
    FILE *fp = fopen(PASSENGER_FILE, "r");
    if (fp == NULL)
        return;

    char line[100];

    while (fgets(line, sizeof(line), fp) != NULL)
    {
        char email[50], pass[30];

        int fields = sscanf(line, "%49[^|]|%29[^\n]", email, pass);
        if (fields != 2)
            continue;

        struct Passenger *p = (struct Passenger *)malloc(sizeof(struct Passenger));
        strcpy(p->email, email);
        strcpy(p->password, pass);
        p->next = passengerHead;
        passengerHead = p;
    }

    fclose(fp);
}

void showMainMenu(void)
{
    printf("\n===== Bus Management System =====\n");
    printf("1. Passenger Panel\n");
    printf("2. Admin Panel\n");
    printf("3. Exit\n");
    printf("Enter choice: ");
}

int main(void)
{
    int loadedCount = loadBusesFromFile();

    if (loadedCount == 0)
    {
        addBus("Green Line", 101, "DHAKA", "CHITTAGONG", 250, 30, "10:00 AM", "24-07-2026");
        addBus("Shohagh", 102, "DHAKA", "SYLHET", 210, 25, "02:00 PM", "24-07-2026");
        addBus("Ena Transport", 103, "DHAKA", "RANGPUR", 300, 35, "08:00 PM", "24-07-2026");
        saveBusesToFile();
    }

    loadPassengersFromFile();

    int choice;

    do
    {
        showMainMenu();
        if (scanf("%d", &choice) != 1)
        {
            clearInputLine();
            printf("Invalid input, numbers only.\n");
            continue;
        }

        switch (choice)
        {
        case 1:
            passengerAuthMenu();
            break;
        case 2:
            if (adminLogin())
                adminPanel();
            break;
        case 3:
            printf("Goodbye!\n");
            break;
        default:
            printf("Invalid choice, try again.\n");
        }

    } while (choice != 3);

    return 0;
}