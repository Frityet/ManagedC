#include <stdio.h>

#include "managed.h"
#include "mstring.h"

typedef struct Company Company_t;
typedef struct Team Team_t;
typedef struct Employee Employee_t;

enum DevelopmentCategory {
    DEVELOPMENT_CATEGORY_FOOD,
    DEVELOPMENT_CATEGORY_CARS,
    DEVELOPMENT_CATEGORY_COMPUTERS,
    DEVELOPMENT_CATEGORY_TOYS,
};

struct Company {
    string  name;
    int     ranking;

    Employee_t  *ceo;
    Team_t      *teams[];
};

struct Team {
    string name;
    enum DevelopmentCategory category;
    Employee_t *members[];
};

struct Employee {
    string name;
    int salary;
};

Company_t *create_company(string name, Employee_t *ceo, int ranking)
{

}

void add_team(Company_t *company, Team_t *team)
{

}

int main()
{

}
