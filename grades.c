#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "grades.h"
#include "linked-list.h"

typedef struct course {
    char *name;
    int grade;
} course_t;

typedef struct student {
    char *name;
    int id;
    struct list *courses;
} student_t;

typedef struct grades {
    struct list *students;
} grades_t;

enum Result {
    kSuccess = 0,
    kFail = 1
};

// Auxiliary List Functions:
// ============================================================================

/**
 * @brief Creates a list copy
 * @param source A pointer to a list
 * @param dest A pointer to an empty list
 * @returns Returns 0 on success or 1 on failure
 */
int list_clone(struct list *source, struct list *dest) {
    if ( source == NULL || dest == NULL ) {
        return kFail;
    }
    
    struct node *it = list_begin(source);
    int size = list_size(source);
    for (int i = 0; i < size; i++) {
        list_push_back(dest, list_get(it));
        it = list_next(it);
    }
    return kSuccess;
}

student_t *list_search_student_id(struct list *grades_p, int id) {
    if (grades_p == NULL) {
        return NULL;
    }

    student_t *curr_student_p = NULL;
    struct node* it = list_begin(grades_p);
    int size = list_size(grades_p);
    for (int i = 0; i < size; i++) {
        curr_student_p = (student_t*)list_get(it);
        if (curr_student_p->id == id) {
            return curr_student_p;
        }
        it = list_next(it);
    }
    return NULL;
}

course_t *list_search_course(struct list *courses_p, const char *name) {
    if ( courses_p == NULL || name == NULL ) {
        return NULL;
    }

    course_t *curr_course_p = NULL;
    struct node* it = list_begin(courses_p);
    int size = list_size(courses_p);
    for (int i = 0; i < size; i++) {
        curr_course_p = (course_t*)list_get(it);
        if ( strcmp(curr_course_p->name, name) == 0 ) {
            return curr_course_p;
        }
        it = list_next(it);
    }
    return NULL;
}

// Course Specific Functions:
// ============================================================================

/**
 * @brief Initializes the "course" data-structure
 * @returns A pointer to the data-structure, or NULL in case of an error
 */
course_t* course_new(const char* name, int grade) {
    if (name == NULL || grade < 0 || grade > 100) {
        return NULL;
    }

    course_t* course_p = (course_t*)malloc(sizeof(course_t));
    if (course_p == NULL) {
        return NULL;
    }

    course_p->name = (char*)malloc(strlen(name) + 1);
    if (course_p->name == NULL) {
        free(course_p);
        return NULL;
    }

    strcpy(course_p->name, name);
    course_p->grade = grade;
    return course_p;
}

/**
 * @brief Creates a course copy
 * @param element A pointer to a course struct
 * @param output A pointer to a pointer to a course struct
 * @returns Returns 0 on success or 1 on failure
 */
int course_clone(void *element, void **output) {
    course_t* source = (course_t*)element;
    if (source == NULL) {
        return kFail;
    }

    course_t* clone = course_new(source->name, source->grade);
    if (clone != NULL) {
        *output = clone;
        return kSuccess;
    }
    return kFail;
}

void course_destroy(void *element) {
    course_t *course_p = (course_t*)element;
    if (course_p == NULL) {
        return;
    }

    free(course_p->name);
    free(course_p);
}

// Student Specific Functions:
// ============================================================================

/**
 * @brief Initializes the "student" data-structure
 * @returns A pointer to the data-structure, or NULL in case of an error
 */
student_t* student_new(const char* name, int id) {
    if (name == NULL) {
        return NULL;
    }

    student_t* student_p = (student_t*)malloc(sizeof(student_t));
    if (student_p == NULL) {
        return NULL;
    }

    student_p->name = (char*)malloc(strlen(name) + 1);
    if (student_p->name == NULL) {
        free(student_p);
        return NULL;
    }

    student_p->courses = list_init(course_clone, course_destroy);
    if (student_p->courses == NULL) {
        free(student_p->name);
        free(student_p);
        return NULL;
    }

    strcpy(student_p->name, name);
    student_p->id = id;
    return student_p;
}

/**
 * @brief Creates a student copy
 * @param element A pointer to a student struct
 * @param output A pointer to a pointer to a student struct
 * @returns Returns 0 on success or 1 on failure
 */
int student_clone(void *element, void **output) {
    student_t *source = (student_t*)element;
    if (source == NULL) {
        return kFail;
    }

    student_t *clone = student_new(source->name, source->id);
    if (clone != NULL) {
        if (list_clone(source->courses, clone->courses) == kFail) {
            free(clone);
            return kFail;
        }
        *output = clone;
        return kSuccess;
    }
    return kFail;
}

void student_destroy(void *element) {
    student_t *student_p = (student_t*)element;
    if (student_p == NULL) {
        return;
    }

    list_destroy(student_p->courses);
    free(student_p->name);
    free(student_p);
}

// Grades Specific Functions:
// ============================================================================

struct grades *grades_init() {
    grades_t *grades_p = (grades_t*)malloc(sizeof(grades_t));
    if (grades_p == NULL) {
        return NULL;
    }

    grades_p->students = list_init(student_clone, student_destroy);
    if (grades_p->students == NULL) {
        free(grades_p);
        return NULL;
    }

    return grades_p;
}

void grades_destroy(struct grades *grades) {
    if (grades == NULL) {
        return;
    }

    list_destroy(grades->students);
    free(grades);
}

int grades_add_student(struct grades *grades, const char *name, int id) {
    if ( grades == NULL || name == NULL ) {
        return kFail;
    }
    
    if ( list_search_student_id(grades->students, id) != NULL ) {
        return kFail;
    }

    student_t* student_p = student_new(name, id);
    list_push_back(grades->students, student_p);
    student_destroy(student_p);
    return kSuccess;
}

int grades_add_grade(struct grades *grades,
                     const char* name,
                     int id,
                     int grade) {
    if ( grades == NULL || name == NULL ) {
        return kFail;
    }
    if ( grade < 0 || grade > 100 ) {
        return kFail;
    }

    student_t *student_p = list_search_student_id(grades->students, id);
    if (student_p == NULL) {
        return kFail;
    }

    if ( list_search_course(student_p->courses, name) != NULL ) {
        return kFail;
    }

    course_t *course_p = course_new(name, grade);
    list_push_back(student_p->courses, course_p);
    course_destroy(course_p);
    return kSuccess;
}

float grades_calc_avg(struct grades *grades, int id, char **out) {
    float avg = 0;
    *out = NULL;
    
    if (grades == NULL) {
        return -kFail;
    }

    student_t* student_p = list_search_student_id(grades->students, id);
    if (student_p == NULL) {
        return -kFail;
    }

    course_t* curr_course_p = NULL;
    struct node* it = list_begin(student_p->courses);
    int size = list_size(student_p->courses);

    for (int i = 0; i < size; i++) {
        curr_course_p = (course_t*)list_get(it);
        avg += curr_course_p->grade;
        it = list_next(it);
    }

    char* tmp_out = (char*)malloc(strlen(student_p->name) + 1);
    strcpy(tmp_out, student_p->name);
    *out = tmp_out;
    if (size == 0) {
        return 0.0;
    }
    return avg / size;
}

/**
 * @brief Prints a student struct to stdout
 * @param student_p A pointer to student
 * @returns Returns 0 on success or 1 on failure
 */
void print_student(student_t *student_p) {
    course_t *curr_course_p = NULL;
    struct node* it = list_begin(student_p->courses);
    int size = list_size(student_p->courses);
    if (size == 0) {
        printf("%s %d:\n", student_p->name, student_p->id);
        return;
    }

    printf("%s %d: ", student_p->name, student_p->id);
    for (int i = 0; i < size - 1; i++) {
        curr_course_p = (course_t*)list_get(it);
        printf("%s %d, ", curr_course_p->name, curr_course_p->grade);
        it = list_next(it);
    }
    curr_course_p = (course_t*)list_get(it);
    printf("%s %d\n", curr_course_p->name, curr_course_p->grade);
}

int grades_print_student(struct grades *grades, int id) {
    if (grades == NULL) {
        return kFail;
    }

    student_t* student_p = list_search_student_id(grades->students, id);
    if (student_p == NULL) {
        return kFail;
    }
        
    print_student(student_p);
    return kSuccess;
}

int grades_print_all(struct grades *grades) {
    if (grades == NULL) {
        return kFail;
    }

    student_t *curr_student_p = NULL;
    struct node* it = list_begin(grades->students);
    int size = list_size(grades->students);
    for (int i = 0; i < size; i++) {
        curr_student_p = (student_t*)list_get(it);
        print_student(curr_student_p);
        it = list_next(it);
    }
    return kSuccess;
}
