#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME 50
#define MAX_FOOD 500
#define MAX_LOC 50
#define MAX_STATUS 20
#define MAX_REVIEW 1000
#define KITCHENS 3

struct food_item
{
    char name[MAX_NAME];
    int price;
    int prep_time;
};

struct food_item menu[] = {
    {"Dosa", 50, 4}, {"Idly", 40, 3}, {"Coffee", 30, 2}, {"Biryani", 120, 8},
    {"Noodles", 90, 7}, {"Paneer Butter Masala", 150, 10}, {"Chicken Curry", 180, 12},
    {"Sandwich", 70, 4}, {"French Fries", 60, 3}, {"Ice Cream", 50, 2},
    {"Fresh Juice", 40, 2}, {"Pasta", 110, 7}, {"Burger", 100, 6},
    {"Momos", 80, 5}, {"Soup", 60, 4}
};

#define MENU_COUNT (sizeof(menu)/sizeof(menu[0]))

struct order {
    int order_id;
    char name[MAX_NAME];
    char loc[MAX_LOC];
    char food[MAX_FOOD];
    int price;
    int prep_time;
    int del_time;
    int kitchen_id;
    char status[MAX_STATUS];
    char review[MAX_REVIEW];
    struct order* next;
};

struct order* front = NULL;
struct order* rear = NULL;
struct order* served_front = NULL;
int orderCounter = 1;
int kitchen_time[KITCHENS] = {0};

void trim(char* str)
{
    char* end;
    while (*str == ' ') str++;
    end = str + strlen(str) - 1;
    while (end > str && *end == ' ') end--;
    *(end + 1) = '\0';
}

void list_menu()
{
    printf("\nAvailable Food Items:\n");
    for (int i = 0; i < MENU_COUNT; i++)
    {
        printf("%2d. %-20s RS.%d\n", i + 1, menu[i].name, menu[i].price);
    }
}

int assign_kitchen(int prep_time)
{
    int min_idx = 0;
    for (int i = 1; i < KITCHENS; i++)
    {
        if (kitchen_time[i] < kitchen_time[min_idx]) min_idx = i;
    }
    kitchen_time[min_idx] += prep_time;
    return min_idx + 1;
}

int get_prep_time(const char* food)
{
    for (int i = 0; i < MENU_COUNT; i++)
    {
        if (strcmp(food, menu[i].name) == 0) return menu[i].prep_time;
    }
    return 5;
}

int get_food_price(const char* food)
{
    for (int i = 0; i < MENU_COUNT; i++)
    {
        if (strcmp(food, menu[i].name) == 0) return menu[i].price;
    }
    return 50;
}

void add_order(const char* name, const char* food_list, const char* loc)
{
    struct order* neworder = malloc(sizeof(struct order));
    if (!neworder)
    {
        printf("Memory allocation failed!\n");
        return;
    }
    neworder->order_id = orderCounter++;
    strcpy(neworder->name, name);
    strcpy(neworder->loc, loc);
    strcpy(neworder->food, food_list);

    int total_price = 0, max_prep = 0;
    char temp_list[MAX_FOOD];
    strcpy(temp_list, food_list);
    char* token = strtok(temp_list, ",");

    while (token)
    {
        trim(token);
        total_price += get_food_price(token);
        int item_prep = get_prep_time(token);
        if (item_prep > max_prep) max_prep = item_prep;
        token = strtok(NULL, ",");
    }

    neworder->prep_time = max_prep;
    neworder->price = total_price;
    neworder->del_time = strlen(loc) % 10 + 5 + max_prep;
    neworder->kitchen_id = assign_kitchen(max_prep);
    strcpy(neworder->status, "Pending");
    strcpy(neworder->review, "Pending");
    neworder->next = NULL;

    if (!rear)
    {
        front = rear = neworder;
    }
    else
    {
        rear->next = neworder;
        rear = neworder;
    }

    printf("\nOrder Added Successfully!\nOrder ID: %d\nTotal Price: RS.%d\n", neworder->order_id, neworder->price);
}

void serve_order()
{
    if (!front)
    {
        printf("\nNo orders to serve!\n");
        return;
    }
    struct order* temp = front;
    front = front->next;
    if (!front) rear = NULL;

    printf("\nServing Order ID: %d\nCustomer: %s\nFood: %s\nTotal Bill: RS.%d\n", temp->order_id, temp->name, temp->food, temp->price);
    printf("Enter customer review: ");
    fgets(temp->review, sizeof(temp->review), stdin);
    temp->review[strcspn(temp->review, "\n")] = 0;
    strcpy(temp->status, "Served");

    temp->next = served_front;
    served_front = temp;

    printf("Order Served with Review: %s\n", temp->review);
}

void display_orders(struct order* list, const char* title)
{
    if (!list)
    {
        printf("\nNo %s orders.\n", title);
        return;
    }
    printf("\n%s Orders:\n", title);
    struct order* curr = list;
    while (curr)
    {
        printf("----------------------------------------\n");
        printf("Order ID: %d\nCustomer: %s\nLocation: %s\nFood Items: %s\nTotal Price: RS.%d\nPrep Time: %d\nDelivery Time: %d\nKitchen: %d\nStatus: %s\nReview: %s\n",
               curr->order_id, curr->name, curr->loc, curr->food, curr->price, curr->prep_time, curr->del_time, curr->kitchen_id, curr->status, curr->review);
        curr = curr->next;
    }
    printf("----------------------------------------\n");
}

void search_order()
{
    int id;
    printf("\nEnter Order ID to search: ");
    scanf("%d", &id); getchar();

    struct order* curr = front;
    while (curr) {
        if (curr->order_id == id)
        {
            printf("Order Found in Pending:\nOrder ID: %d, Customer: %s, Food: %s, Status: %s\n", curr->order_id, curr->name, curr->food, curr->status);
            return;
        }
        curr = curr->next;
    }

    curr = served_front;
    while (curr)
    {
        if (curr->order_id == id)
        {
            printf("Order Found in Served:\nOrder ID: %d, Customer: %s, Food: %s, Status: %s\n", curr->order_id, curr->name, curr->food, curr->status);
            return;
        }
        curr = curr->next;
    }

    printf("Order ID %d not found.\n", id);
}

void cancel_order()
{
    int id;
    printf("\nEnter Order ID to cancel: ");
    scanf("%d", &id); getchar();

    struct order *curr = front, *prev = NULL;
    while (curr) {
        if (curr->order_id == id)
        {
            if (!prev) front = curr->next;
            else prev->next = curr->next;
            if (curr == rear) rear = prev;
            printf("Order ID %d cancelled successfully.\n", id);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }

    printf("Order ID %d not found in pending orders.\n", id);
}

void update_order()
{
    int id;
    printf("\nEnter Order ID to add more food items: ");
    scanf("%d", &id); getchar();

    struct order* curr = front;
    while (curr)
    {
        if (curr->order_id == id)
        {
            printf("Current Food Items: %s\n", curr->food);
            printf("Enter additional food items (comma-separated): ");
            char new_food[MAX_FOOD];
            fgets(new_food, sizeof(new_food), stdin);
            new_food[strcspn(new_food, "\n")] = 0;

            strcat(curr->food, ",");
            strcat(curr->food, new_food);

            int total_price = 0, max_prep = 0;
            char temp_list[MAX_FOOD];
            strcpy(temp_list, curr->food);
            char* token = strtok(temp_list, ",");

            while (token)
            {
                trim(token);
                total_price += get_food_price(token);
                int item_prep = get_prep_time(token);
                if (item_prep > max_prep) max_prep = item_prep;
                token = strtok(NULL, ",");
            }

            curr->price = total_price;
            curr->prep_time = max_prep;
            curr->del_time = strlen(curr->loc) % 10 + 5 + max_prep;

            printf("Order ID %d updated successfully.\n", id);
            printf("New Food List: %s\n", curr->food);
            printf("Updated Total Price: RS.%d\n", curr->price);
            return;
        }
        curr = curr->next;
    }

    printf("Order ID %d not found in pending orders.\n", id);
}

void display_stats()
{
    int pending = 0, served = 0;
    struct order* curr = front;
    while (curr)
    {
        pending++;
        curr = curr->next;
    }
    curr = served_front;
    while (curr)
    {
        served++;
        curr = curr->next;
    }
    printf("\nOrder Statistics:\nPending Orders: %d\nServed Orders: %d\n", pending, served);
}

int main()
{
    int choice;
    char name[MAX_NAME], food[MAX_FOOD], loc[MAX_LOC];

    while (1)
    {
        printf("\n========== Restaurant Order System =========="
               "\n1. List Menu\n2. Add Order\n3. Serve Order\n4. Display Pending Orders\n5. View Served Orders"
               "\n6. Search Order\n7. Cancel Order\n8. Update Order\n9. Display Stats\n10. Exit"
               "\nEnter your choice: ");
        scanf("%d", &choice); getchar();

        switch (choice)
        {
            case 1: list_menu(); break;
            case 2:
                printf("Enter Customer Name: ");
                fgets(name, sizeof(name), stdin);
                name[strcspn(name, "\n")] = 0;
                list_menu();
                printf("Enter Food Items (comma-separated, e.g., Dosa,Idly,Coffee): ");
                fgets(food, sizeof(food), stdin);
                food[strcspn(food, "\n")] = 0;
                printf("Enter Delivery Location(Delivery Free!!): ");
                fgets(loc, sizeof(loc), stdin);
                loc[strcspn(loc, "\n")] = 0;
                add_order(name, food, loc);
                break;
            case 3: serve_order(); break;
            case 4: display_orders(front, "Pending"); break;
            case 5: display_orders(served_front, "Served"); break;
            case 6: search_order(); break;
            case 7: cancel_order(); break;
            case 8: update_order(); break;
            case 9: display_stats(); break;
            case 10: printf("Exiting...\n"); exit(0);
            default: printf("Invalid choice!\n");
        }
    }
    return 0;
}
