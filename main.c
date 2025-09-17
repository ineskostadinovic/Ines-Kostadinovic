#include "Header.h"

int main(void) {
    int choice = 0;
    char buf[32];
    while (1) {
        showMenu();
        printf("Odaberi opciju: ");
        if (!fgets(buf, sizeof(buf), stdin)) {
            clearerr(stdin);
            continue;
        }
        trim_newline(buf);
        if (buf[0] == '\0') continue;
        choice = (int)strtol(buf, NULL, 10);

        switch (choice) {
        case ADD: addStudent(); break;
        case VIEW: viewStudents(); break;
        case UPDATE: updateStudent(); break;
        case DELETE: deleteStudent(); break;
        case SEARCH: searchStudentByJMBAG(); break;
        case VIEW_SORTED: viewSortedStudents(); break;
        case STATS: showStatistics(); break;
        case EXPORT_CSV: exportToCSV(); break;
        case EXIT: printf("Izlazak iz programa.\n"); return 0;
        default: printf("Nepostojeća opcija.\n");
        }
    }
    return 0;
}
