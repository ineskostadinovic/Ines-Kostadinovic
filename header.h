#ifndef HEADER_H
#define HEADER_H

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME 50
#define MAX_JMBAG 12
#define FILE_NAME "students.dat"

typedef struct {
    char firstName[MAX_NAME];
    char lastName[MAX_NAME];
    char jmbag[MAX_JMBAG];
    int attendance;
    float grade;
} Student;

typedef enum {
    ADD = 1,
    VIEW,
    UPDATE,
    DELETE,
    SEARCH,
    VIEW_SORTED,
    STATS,
    EXPORT_CSV,
    EXIT
} MenuOption;

/* --- deklaracije funkcija --- */
void showMenu();
void addStudent();
void viewStudents();
void updateStudent();
void deleteStudent();
void searchStudentByJMBAG();
void viewSortedStudents();
void showStatistics();
void exportToCSV();

/* pomoćne funkcije */
void trim_newline(char* s);
int readIntPrompt(const char* prompt);
float readFloatPrompt(const char* prompt);
void readStringPrompt(const char* prompt, char* dest, size_t size);

int compareByLastName(const void* a, const void* b);

#endif
