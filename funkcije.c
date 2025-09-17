#include "Header.h"

/* --- pomoćne funkcije --- */
void trim_newline(char* s) {
    if (!s) return;
    s[strcspn(s, "\r\n")] = '\0';
}

int readIntPrompt(const char* prompt) {
    char buf[64];
    char* endptr;
    long val;
    while (1) {
        printf("%s", prompt);
        if (!fgets(buf, sizeof(buf), stdin)) {
            clearerr(stdin);
            continue;
        }
        trim_newline(buf);
        if (buf[0] == '\0') {
            printf("Unos ne smije biti prazan. Pokušaj ponovno.\n");
            continue;
        }
        val = strtol(buf, &endptr, 10);
        if (endptr == buf || *endptr != '\0') {
            printf("Neispravan broj. Pokušaj ponovno.\n");
            continue;
        }
        return (int)val;
    }
}

float readFloatPrompt(const char* prompt) {
    char buf[64];
    char* endptr;
    float val;
    while (1) {
        printf("%s", prompt);
        if (!fgets(buf, sizeof(buf), stdin)) {
            clearerr(stdin);
            continue;
        }
        trim_newline(buf);
        if (buf[0] == '\0') {
            printf("Unos ne smije biti prazan. Pokušaj ponovno.\n");
            continue;
        }
        val = strtof(buf, &endptr);
        if (endptr == buf || *endptr != '\0') {
            printf("Neispravan decimalni broj. Pokušaj ponovno.\n");
            continue;
        }
        return val;
    }
}

void readStringPrompt(const char* prompt, char* dest, size_t size) {
    char buf[512];
    while (1) {
        printf("%s", prompt);
        if (!fgets(buf, (int)sizeof(buf), stdin)) {
            clearerr(stdin);
            continue;
        }
        trim_newline(buf);
        if (buf[0] == '\0') {
            printf("Unos ne smije biti prazan. Pokušaj ponovno.\n");
            continue;
        }
        strncpy(dest, buf, size - 1);
        dest[size - 1] = '\0';
        return;
    }
}

/* --- implementacije funkcija programa --- */
int compareByLastName(const void* a, const void* b) {
    return strcmp(((const Student*)a)->lastName, ((const Student*)b)->lastName);
}

void showMenu() {
    printf("\n--- Evidencija studenata ---\n");
    printf("1. Dodaj studenta\n");
    printf("2. Prikaži sve studente\n");
    printf("3. Ažuriraj podatke studenta\n");
    printf("4. Obriši studenta\n");
    printf("5. Pretraži studenta po JMBAG-u\n");
    printf("6. Prikaži sortirane studente po prezimenu\n");
    printf("7. Statistika (prosjek dolaznosti i ocjena)\n");
    printf("8. Izvoz u CSV datoteku\n");
    printf("9. Izlaz\n");
}

void addStudent() {
    FILE* file = fopen(FILE_NAME, "ab");
    if (!file) {
        perror("Otvaranje datoteke");
        return;
    }

    Student s;
    readStringPrompt("Unesi ime: ", s.firstName, MAX_NAME);
    readStringPrompt("Unesi prezime: ", s.lastName, MAX_NAME);

    while (1) {
        char tmp[MAX_JMBAG * 2];
        printf("Unesi JMBAG: ");
        if (!fgets(tmp, sizeof(tmp), stdin)) { clearerr(stdin); continue; }
        trim_newline(tmp);
        if (tmp[0] == '\0') { printf("JMBAG ne smije biti prazan.\n"); continue; }
        if (strlen(tmp) >= MAX_JMBAG) { printf("JMBAG predug. Maksimalno %d znakova.\n", MAX_JMBAG - 1); continue; }
        strcpy(s.jmbag, tmp);
        break;
    }

    s.attendance = readIntPrompt("Unesi broj dolazaka: ");
    s.grade = readFloatPrompt("Unesi ocjenu (npr. 4.00): ");

    if (fwrite(&s, sizeof(Student), 1, file) != 1) {
        perror("Upis u datoteku");
    }
    else {
        printf("Student dodan.\n");
    }
    fclose(file);
}

void viewStudents() {
    FILE* file = fopen(FILE_NAME, "rb");
    if (!file) {
        perror("Otvaranje datoteke");
        return;
    }

    Student s;
    int any = 0;
    printf("\n--- Popis studenata ---\n");
    while (fread(&s, sizeof(Student), 1, file) == 1) {
        printf("Ime: %s\nPrezime: %s\nJMBAG: %s\nDolaznost: %d\nOcjena: %.2f\n\n",
            s.firstName, s.lastName, s.jmbag, s.attendance, s.grade);
        any = 1;
    }
    if (!any) printf("Nema unesenih studenata.\n");
    fclose(file);
}

void updateStudent() {
    FILE* file = fopen(FILE_NAME, "r+b");
    if (!file) {
        perror("Otvaranje datoteke");
        return;
    }

    char jmbag[MAX_JMBAG];
    printf("Unesi JMBAG studenta za ažuriranje: ");
    if (!fgets(jmbag, sizeof(jmbag), stdin)) { fclose(file); return; }
    trim_newline(jmbag);
    if (jmbag[0] == '\0') { printf("JMBAG prazan.\n"); fclose(file); return; }

    Student s;
    int found = 0;

    while (fread(&s, sizeof(Student), 1, file) == 1) {
        if (strcmp(s.jmbag, jmbag) == 0) {
            found = 1;
            printf("Trenutni podaci:\nIme: %s\nPrezime: %s\nJMBAG: %s\nDolaznost: %d\nOcjena: %.2f\n\n",
                s.firstName, s.lastName, s.jmbag, s.attendance, s.grade);

            readStringPrompt("Novo ime: ", s.firstName, MAX_NAME);
            readStringPrompt("Novo prezime: ", s.lastName, MAX_NAME);

            s.attendance = readIntPrompt("Nova dolaznost: ");
            s.grade = readFloatPrompt("Nova ocjena (npr. 4.00): ");

            if (fseek(file, -(long)sizeof(Student), SEEK_CUR) != 0) {
                perror("fseek error");
                break;
            }
            if (fwrite(&s, sizeof(Student), 1, file) != 1) {
                perror("Upis u datoteku");
            }
            else {
                printf("Podaci ažurirani.\n");
            }
            break;
        }
    }

    if (!found) {
        printf("Student nije pronađen.\n");
    }

    fclose(file);
}

void deleteStudent() {
    FILE* file = fopen(FILE_NAME, "rb");
    if (!file) {
        perror("Otvaranje datoteke");
        return;
    }

    FILE* temp = fopen("temp.dat", "wb");
    if (!temp) {
        perror("Kreiranje privremene datoteke");
        fclose(file);
        return;
    }

    char jmbag[MAX_JMBAG];
    printf("Unesi JMBAG za brisanje: ");
    if (!fgets(jmbag, sizeof(jmbag), stdin)) { fclose(file); fclose(temp); return; }
    trim_newline(jmbag);

    Student s;
    int found = 0;
    while (fread(&s, sizeof(Student), 1, file) == 1) {
        if (strcmp(s.jmbag, jmbag) == 0) {
            found = 1;
            continue;
        }
        if (fwrite(&s, sizeof(Student), 1, temp) != 1) {
            perror("Upis u temp datoteku");
            fclose(file);
            fclose(temp);
            return;
        }
    }

    fclose(file);
    fclose(temp);

    if (found) {
        if (remove(FILE_NAME) != 0) perror("Greška pri brisanju datoteke");
        if (rename("temp.dat", FILE_NAME) != 0) perror("Greška pri preimenovanju datoteke");
        printf("Student obrisan.\n");
    }
    else {
        remove("temp.dat");
        printf("Student nije pronađen.\n");
    }
}

void searchStudentByJMBAG() {
    FILE* file = fopen(FILE_NAME, "rb");
    if (!file) {
        perror("Otvaranje datoteke");
        return;
    }

    char jmbag[MAX_JMBAG];
    printf("Unesi JMBAG za pretragu: ");
    if (!fgets(jmbag, sizeof(jmbag), stdin)) { fclose(file); return; }
    trim_newline(jmbag);
    if (jmbag[0] == '\0') { printf("JMBAG prazan.\n"); fclose(file); return; }

    Student s;
    int found = 0;
    while (fread(&s, sizeof(Student), 1, file) == 1) {
        if (strcmp(s.jmbag, jmbag) == 0) {
            printf("Ime: %s\nPrezime: %s\nJMBAG: %s\nDolaznost: %d\nOcjena: %.2f\n",
                s.firstName, s.lastName, s.jmbag, s.attendance, s.grade);
            found = 1;
            break;
        }
    }

    if (!found) printf("Student nije pronađen.\n");
    fclose(file);
}

void viewSortedStudents() {
    FILE* file = fopen(FILE_NAME, "rb");
    if (!file) {
        perror("Otvaranje datoteke");
        return;
    }

    if (fseek(file, 0, SEEK_END) != 0) { perror("fseek error"); fclose(file); return; }
    long size = ftell(file);
    if (size < 0) { perror("ftell error"); fclose(file); return; }
    rewind(file);

    int count = (int)(size / sizeof(Student));
    if (count == 0) {
        printf("Nema studenata za prikaz.\n");
        fclose(file);
        return;
    }

    Student* students = malloc((size_t)count * sizeof(Student));
    if (!students) { perror("Alokacija memorije"); fclose(file); return; }

    if (fread(students, sizeof(Student), (size_t)count, file) != (size_t)count) {
        perror("Čitanje studenata");
        free(students);
        fclose(file);
        return;
    }
    fclose(file);

    qsort(students, (size_t)count, sizeof(Student), compareByLastName);

    printf("\n--- Studenti sortirani po prezimenu ---\n");
    for (int i = 0; i < count; ++i) {
        printf("Prezime: %s, Ime: %s, JMBAG: %s, Dolaznost: %d, Ocjena: %.2f\n",
            students[i].lastName, students[i].firstName, students[i].jmbag,
            students[i].attendance, students[i].grade);
    }

    free(students);
}

void showStatistics() {
    FILE* file = fopen(FILE_NAME, "rb");
    if (!file) {
        perror("Otvaranje datoteke");
        return;
    }

    Student s;
    int total = 0;
    long sumAttendance = 0;
    double sumGrade = 0.0;

    while (fread(&s, sizeof(Student), 1, file) == 1) {
        sumAttendance += s.attendance;
        sumGrade += s.grade;
        total++;
    }
    fclose(file);

    if (total == 0) {
        printf("Nema podataka za statistiku.\n");
        return;
    }

    printf("\n--- Statistika ---\n");
    printf("Prosječna dolaznost: %.2f\n", (double)sumAttendance / total);
    printf("Prosječna ocjena: %.2f\n", sumGrade / total);
}

void exportToCSV() {
    FILE* file = fopen(FILE_NAME, "rb");
    if (!file) { perror("Otvaranje datoteke"); return; }

    FILE* csv = fopen("students_export.csv", "w");
    if (!csv) { perror("Kreiranje CSV datoteke"); fclose(file); return; }

    fprintf(csv, "Ime,Prezime,JMBAG,Dolaznost,Ocjena\n");

    Student s;
    while (fread(&s, sizeof(Student), 1, file) == 1) {
        if (fprintf(csv, "%s,%s,%s,%d,%.2f\n",
            s.firstName, s.lastName, s.jmbag, s.attendance, s.grade) < 0) {
            perror("Pisanje u CSV");
            break;
        }
    }

    fclose(file);
    fclose(csv);
    printf("Podaci su izvezeni u 'students_export.csv'.\n");
}
