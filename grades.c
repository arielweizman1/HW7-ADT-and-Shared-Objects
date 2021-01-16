#include "grades.h"
#include "stdio.h"
#include "linked-list.h"
#include "malloc.h"
#include "string.h"

struct grades {
    struct list *students;
};


struct student {
    int id;
    char *name;
    struct list *courses;
};

struct course {
    int grade;
    char *course_name;
};
/**
 * @brief clone "course" to “output”.
 * @param course -the course needed to be copied.
 * @param output -the cloned course.
 * @returns 0 on success.
 */
int clone_course(void *course, void **output) {
    struct course *cloned_course = malloc(sizeof(*cloned_course));
    if (!cloned_course) {
        return -1;
    }
    struct course *orig_course = course;
    cloned_course->grade = orig_course->grade;
    cloned_course->course_name = malloc(strlen(orig_course->course_name) + 1);
    if (!cloned_course->course_name) {
        free(cloned_course);
        return -1;
    }
    strcpy(cloned_course->course_name, orig_course->course_name);
    *output = cloned_course;
    return 0;
}

/**
 * @brief Destroy “generic_course”.
 * @param generic_course -the course needed to be destroyed.
 */

void destroy_course(void *generic_course) {
    struct course *destroyed_course = generic_course;
    free(destroyed_course->course_name);
    free(destroyed_course);
}

/**
 * @brief clone "student" to “output”.
 * @param student -the student needed to be copied.
 * @param output -the cloned student.
 * @returns 0 on success.
 */
int clone_student(void *student, void **output) {
    struct student *cloned_student = malloc(sizeof(*cloned_student));
    if (!cloned_student) {
        return -1;
    }
    struct student *original_student = student;
    cloned_student->id = original_student->id;
    cloned_student->name = malloc(strlen(original_student->name) + 1);
    if (!cloned_student->name) {
        free(cloned_student);
        return -1;
    }
    strcpy(cloned_student->name, original_student->name);

    cloned_student->courses = list_init(clone_course, destroy_course);
    if (!cloned_student->courses) {
        free(cloned_student->name);
        free(cloned_student);
        return -1;
    }
    for (struct node *iter = list_begin(original_student->courses);
         iter != NULL;
         iter = list_next(iter)) {
        if (list_push_back(cloned_student->courses, list_get(iter)) != 0) {
            list_destroy(cloned_student->courses);
            free(cloned_student->name);
            free(cloned_student);
            return -1;
        }
    }
    *output = cloned_student;
    return 0;
}

/**
 * @brief Destroy "generic_student".
 * @param generic_student -the student needed to be destroyed.
 */
void destroy_student(void *generic_student) {
    struct student *destroyed_student = generic_student;
    free(destroyed_student->name);
    list_destroy(destroyed_student->courses);
    free(destroyed_student);
}


struct grades *grades_init() {
    struct grades *new_grades = malloc(sizeof(*new_grades));
    if (new_grades == NULL) {
        return NULL;
    }
    new_grades->students = list_init(clone_student, destroy_student);
    if (!new_grades->students) {
        free(new_grades);
        return NULL;
    }
    return new_grades;
}

int grades_add_grade(struct grades *grades, const char *name,
                     int id, int grade) {
    if (grades == NULL || grade > 100 || grade < 0) {
        return -1;
    }
    for (struct node *iter = list_begin(grades->students);
         iter != NULL; iter = list_next(iter)) {
        struct student *current_student = list_get(iter);
        if (current_student->id == id) {
            for (struct node *iter2 = list_begin(current_student->courses);
                 iter2 != NULL;
                 iter2 = list_next(iter2)) {
                struct course *current_course = list_get(iter2);
                
                if (strcmp(current_course->course_name, name) == 0) {
                    return -1;
                }
            }

            struct course *new_course = malloc(sizeof(*new_course));
            if (!new_course) {
                return -1;
            }
            new_course->grade = grade;
            new_course->course_name = malloc(strlen(name) + 1);
            if (!new_course->course_name) {
                free(new_course);
                return -1;
            }
            strcpy(new_course->course_name, name);
            if (list_push_back(current_student->courses, new_course) != 0) {
                free(new_course->course_name);
                free(new_course);
                return -1;
            }

            free(new_course->course_name);
            free(new_course);
            return 0;
        }
    }
    return -1;
}

int grades_add_student(struct grades *grades, const char *name, int id) {
    if (grades == NULL) {
        return -1;
    }
    for (struct node *iter = list_begin(grades->students);
         iter != NULL; iter = list_next(iter)) {
        struct student *current_student = list_get(iter);
        if (current_student->id == id) {
            return -1;
        }
    }

    struct student *new_student = malloc(sizeof(*new_student));
    if (!new_student) {
        return -1;
    }
    new_student->id = id;
    new_student->name = malloc(strlen(name) + 1);
    if (!new_student->name) {
        free(new_student);
        return -1;
    }
    strcpy(new_student->name, name);
    new_student->courses = list_init(clone_course,destroy_course);
    if (!new_student->courses){
        free(new_student->name);
        free(new_student);
        return -1;
    }
    if (list_push_back(grades->students, new_student) != 0) {
        free(new_student->name);
        list_destroy(new_student->courses);
        free(new_student);
        return -1;
    }
    list_destroy(new_student->courses);
    free(new_student->name);
    free(new_student);
    return 0;
}

void grades_destroy(struct grades *grades) {
    if (grades != NULL) {
        list_destroy(grades->students);
        free(grades);
    }
}


float grades_calc_avg(struct grades *grades, int id, char **out) {
    if (!grades) {
        *out = NULL;
        return -1;
    }
    float sum = 0;
    for (struct node *iter = list_begin(grades->students);
         iter != NULL; iter = list_next(iter)) {
        struct student *curr_student = list_get(iter);
        if (curr_student->id == id) {
            *out = malloc(strlen(curr_student->name) + 1);
            if (!*out) {
                *out = NULL;
                return -1;
            }
            strcpy(*out, curr_student->name);
            if (list_size(curr_student->courses) == 0) {
                return 0;
            }
            for (struct node *iter2 = list_begin(curr_student->courses);
                 iter2 != NULL;
                 iter2 = list_next(iter2)) {
                struct course *current_course = list_get(iter2);
                sum += current_course->grade;
            }
            return sum / list_size(curr_student->courses);
        }
    }
    *out = NULL;
    return -1;
}

int grades_print_student(struct grades *grades, int id) {
    if (grades == NULL) {
        return -1;
    }
    int counter = 1;
    for (struct node *iter = list_begin(grades->students);
         iter != NULL; iter = list_next(iter)) {
        struct student *current_student = list_get(iter);
        if (current_student->id == id) {
            printf("%s %d:", current_student->name, current_student->id);
            for (struct node *iter2 = list_begin(current_student->courses);
                 iter2 != NULL;
                 iter2 = list_next(iter2)) {
                struct course *current_course = list_get(iter2);
                if (counter == list_size(current_student->courses)) {
                    printf(" %s %d", current_course->course_name,
                           current_course->grade);
                } else {
                    printf(" %s %d,", current_course->course_name,
                           current_course->grade);
                    counter++;
                }
            }
            printf("\n");
            return 0;
        }
    }
    return -1;
}

int grades_print_all(struct grades *grades) {
    if (grades == NULL) {
        return -1;
    }
    for (struct node *iter = list_begin(grades->students);
         iter != NULL; iter = list_next(iter)) {
        struct student *current_student = list_get(iter);
        if (grades_print_student(grades, current_student->id) != 0) {
            return -1;
        }
    }
    return 0;
}