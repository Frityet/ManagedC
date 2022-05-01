#include <stdio.h>

#include "managed.h"
#include "mstring.h"

typedef struct Company Company_t;
typedef struct Team Team_t;
typedef struct Employee Employee_t;

enum DevelopmentCategory {
    DEVELOPMENT_CATEGORY_FOOD       = 10,
    DEVELOPMENT_CATEGORY_CARS       = 100,
    DEVELOPMENT_CATEGORY_COMPUTERS  = 25,
    DEVELOPMENT_CATEGORY_TOYS       = 5,
};

struct Company {
    string  name;
    int     ranking;

    Employee_t  *ceo;
    Team_t      **teams;
};

struct Team {
    string name;
    enum DevelopmentCategory category;
    int maintenance_rate;
    Employee_t **members;
};

struct Employee {
    string name;
    int salary;
};

void bankruptcy(Company_t *company)
{
    mc_release(company->name);
    mc_release(company->ceo);
    mc_release(company->teams);
}

void disband_team(Team_t *team)
{
    mc_release(team->name);
    mc_release(team->members);
}

void fire_employee(Employee_t *employee)
{
    mc_release(employee->name);
}

Employee_t *hire_employee(string name, int salary)
{
    Employee_t *emp = mc_alloc_managed(sizeof(Employee_t), 1, (void *)fire_employee);
    emp->name = mc_reference(name);
    emp->salary = salary;
    return emp;
}

Team_t *create_team(string name, enum DevelopmentCategory category, int employee_count)
{
    Team_t *tm = mc_alloc_managed(sizeof(Team_t), 1, (void *) disband_team);
    tm->name = mc_reference(name);
    tm->members = mc_alloc_managed(sizeof(Employee_t *), employee_count, mc_free_managed);
    tm->category = category;
    tm->maintenance_rate = category * employee_count;
    return tm;
}

Company_t *create_company(string name, Employee_t *ceo, int ranking)
{
    Company_t *cmp = mc_alloc_managed(sizeof(Company_t), 1, (void *)bankruptcy);
    cmp->name = mc_reference(name);
    cmp->ceo = mc_reference(ceo);
    cmp->teams = mc_alloc_managed(sizeof(Team_t *), 1, mc_free_managed);
    cmp->ranking = ranking;
    return cmp;
}

void add_team(Company_t *company, Team_t *team)
{
    int teamcount = mc_countof(company->teams);
//    company->teams
}

int main()
{

}
