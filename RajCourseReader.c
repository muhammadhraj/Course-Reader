#ifndef _HW2_H
#define _HW2_H

/** 
* Loads student and course data from input file and allows user to query the data.
*
* Completion time: 8 hours
* 
* @author Muhammad Raj, Ruben Acuna
* @version October 18, 2021
*
*/

////////////////////////////////////////////////////////////////////////////////
// INCLUDES
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

////////////////////////////////////////////////////////////////////////////////
// MACROS: CONSTANTS
#define MAX_STUDENT_NAME_LENGTH 20
#define MAX_COURSE_NAME_LENGTH 10
#define MAX_TEACHER_NAME_LENGTH 20
#define MAX_COMMENT_LENGTH 20
#define MAX_ASSIGN_NAME_LENGTH 20
#define MAX_FILENAME_LENGTH 30

////////////////////////////////////////////////////////////////////////////////
//DATA STRUCTURES

typedef enum AcademicLevel {Freshman=1, Sophomore=2, Junior=3, Senior=4} AcademicLevel;
typedef struct ScoreStruct {
                        float score;
                        char* comment;} ScoreStruct;
typedef struct Assign {
                        int id;
                        char* name;} Assign;
typedef struct Student {
                        int id;
                        char* name;
                        AcademicLevel level;} Student;
typedef struct Course {
                        int id;
                        char* name;
                        char* teacher;
                        int num_assigns;
                        Assign* assigns;
                        ScoreStruct** scores;} Course;

////////////////////////////////////////////////////////////////////////////////
//GLOBAL VARIABLES
//place to store student information
Student* students = NULL;
//place to store course information
Course* courses = NULL; 
int numCourses, numStudents;

////////////////////////////////////////////////////////////////////////////////
//FORWARD DECLARATIONS

// the following should be used to read student/course data in from the file 
void readFile(char* filename);
void readString(FILE* file, char** dst, int max_length);
Student* readStudents(FILE* file);
Course* readCourses(FILE* file);
Assign* readAssigns(FILE* file, int numAssigns);
ScoreStruct** readScores(FILE* file, int numAssigns);

// the following should be used to free all heap data allocated during the programs runtime
// and handle dangling pointers
void terminate();
void studentsDestructor();
void coursesDestructor();
void assignsDestructor(Assign** assigns, int numAssign);
void scoresDestructor(ScoreStruct*** scores, int numAssigns);

// the following should be used to cleanly print the data used in the program 
void printStudents();
void printAssigns(Assign* assigns, int numAssigns);
void printGrades(ScoreStruct** scores, int numAssigns);
void printCourse(Course course);

// the following are mostly complete functions that define and 
// control the CLI menu associated with the program
void mainMenu();
void mainMenuBranch(int option);
void subMenu(Course course);
void subMenuBranch(int option, Course course);

// these are the 'special' functions that you are being asked to implement
void studentMenu(Course course);
void getStudentScores(Course course, int studentNo);
void assignmentMenu(Course course);
void getAssignmentScore(Course course, int assignmentNo);

// this is an optional function that parses and executes instructions defined in an input file
void performInstructions(char* iFile);

// this is a utility function to be called when input filenames are invalid
void printUsage();

/////////////////////////////////////////////////////////////////////////////////
//IMPLEMENTATION

/**
* Loads data from student/course data file
* @param filename is the name of the file 
*/
void readFile(char* filename){
  FILE* file = fopen(filename, "r");
  //FOR YOU TO IMPLEMENT
  if(file){
      fscanf(file, "%d\n", &numStudents);
      students = readStudents(file);
      //read in number of courses
      fscanf(file, "%d\n", &numCourses);
      //read courses into courses struct
      courses =readCourses(file);
  }else{
      perror("Unable to open input file.");
      exit(1);
  }
  fclose(file);
}

Student* readStudents(FILE* file){
    Student* pStudents = (Student*) malloc(sizeof(Student)*numStudents);
    for(int i=0; i<numStudents; i++){
        fscanf(file, "%d\n", &pStudents[i].id);
        readString(file, &pStudents[i].name, MAX_STUDENT_NAME_LENGTH);
        fscanf(file, "%d\n", &pStudents[i].level);
    }
    return pStudents;
}

void readString(FILE* file, char** dst, int max_length){
    char buffer[max_length];
    fscanf(file, "%s", buffer);
    int length = (int) strlen(buffer);
    char* string = (char*) malloc(sizeof(char)*length);
    strcpy(string, buffer);
    *dst = string;
}

Course* readCourses(FILE* file){
    Course* pCourses = (Course*)malloc(sizeof(Course)*numCourses);
    for(int i=0; i<numCourses; i++){
        fscanf(file, "%d\n", &pCourses[i].id);
        readString(file, &pCourses[i].name, MAX_COURSE_NAME_LENGTH);
        readString(file, &pCourses[i].teacher, MAX_TEACHER_NAME_LENGTH);
        fscanf(file, "%d\n", &pCourses[i].num_assigns);
        pCourses[i].assigns = readAssigns(file, pCourses[i].num_assigns);
        pCourses[i].scores = readScores(file, pCourses[i].num_assigns);
    }
    return pCourses;
}

Assign* readAssigns(FILE* file, int numAssigns){
    Assign* pAssigns = (Assign*)malloc(sizeof(Assign)*numAssigns);
    for(int i=0; i<numAssigns; i++){
        fscanf(file, "%d\n", &pAssigns[i].id);
        readString(file, &pAssigns[i].name, MAX_ASSIGN_NAME_LENGTH);
    }
    return pAssigns;
}

ScoreStruct** readScores(FILE* file, int numAssigns){
    ScoreStruct** scores = (ScoreStruct**)malloc(sizeof(ScoreStruct*)*numAssigns);
    for(int i=0; i<numAssigns; i++){
        scores[i] = (ScoreStruct*)malloc(sizeof(ScoreStruct)*numStudents);
    }
    int num_scores = numAssigns * numStudents;
    for(int i=0; i<num_scores; i++){
        int assign_id;
        int student_id;
        float score;
        fscanf(file, "%d\n", &assign_id);
        fscanf(file, "%d\n", &student_id);
        fscanf(file, "%f\n", &score);
        scores[assign_id-1][student_id-1].score = score;
        readString(file, &scores[assign_id-1][student_id-1].comment, MAX_COMMENT_LENGTH);
    }
    return scores;
}


/**
* Implements main menu functionality, which allows user to select a course to interact with
*/
void mainMenu(){
  int input_buffer;
  printf("Course Searcher");  
  do {
	printf("\n-----------------------------------\n");
    printf("Course Options");
	printf("\n-----------------------------------\n");
	int i;
	for(i = 0; i < numCourses; i++){
            //FOR YOU TO IMPLEMENT
            printf("   %d %s\n", courses[i].id, courses[i].name);
    }
	printf("   0 REPEAT OPTIONS\n");
    printf("  -1 TERMINATE PROGRAM\n");
	printf("Please enter your choice ---> ");
	scanf(" %d", &input_buffer);
    mainMenuBranch(input_buffer);
  } while (1);
}

/**
* Handles menu options of main menu
* @param option is the chosen operation's option #
*/
void mainMenuBranch(int option){
  if (option < -1 || option > numCourses){
    printf("Invalid input. Please try again...\n");; 
    while(option < -1 || option > numCourses){
  	  printf("Please enter a valid option ---> ");
      scanf(" %d", &option);
    }
  }
  if(option == -1){
	printf("Terminating program...\n");
    terminate();
  } else if (option == 0) {
	printf("Repeating options...\n");
  } else {
    Course course = courses[option - 1];
	//FOR YOU TO IMPLEMENT
        printf("Course with name %s selected.\n", course.name);
	subMenu(course);
  }
}

/**
* Implements sub menu functionality, which allows users to select how they want to interact with course
* @course is the course to be queried
*/ 

void subMenu(Course course){
  int input_buffer;
  do {
    printf("\n-----------------------------------\n");
    printf("Menu Options");
	printf("\n-----------------------------------\n");
    printf("   1 Get a student's final grades in the course\n");
	printf("   2 Get the average grade of an assignment in the course\n");
    printf("   3 Print all course data\n");
	printf("   0 REPEAT OPTIONS\n");
	printf("  -1 RETURN TO PREVIOUS MENU\n");
    printf("  -2 TERMINATE PROGRAM\n");	
	printf("Please enter your choice ---> ");
	scanf(" %d", &input_buffer);
    subMenuBranch(input_buffer, course);
  } while (input_buffer != -1);
}

/**
* Handles menu options of submenu
* @param option is the chosen operation's option #
* @param course is the course struct to be queried
*/
void subMenuBranch(int option, Course course){
  if (option < -2 || option > 3){
    printf("Invalid input. Please try again...\n");; 
    while(option < -2 || option > 3){
  	  printf("Please enter a valid option ---> ");
      scanf(" %d", &option);
    }
  }
  if(option == -2){
    printf("Terminating program...\n");
	terminate();
  } else if (option == -1){
	printf("Returning to previous menu...\n");
        mainMenu();
  } else if (option == 0){
    printf("Repeating options...\n");
    subMenu(course);
  } else if (option == 1){
    //FOR YOU TO IMPLEMENT
      studentMenu(course);
  } else if (option == 2){
    //FOR YOU TO IMPLEMENT
      assignmentMenu(course);
  } else if (option == 3){
    //FOR YOU TO IMPLEMENT
    printCourse(course);
  } 
}

void studentMenu(Course course){
    printf("Please choose from the following students: \n");
    printStudents();
    printf("0 RETURN TO PREVIOUS MENU\n");
    int student_choice;
    scanf("%d", &student_choice);
    if(student_choice == 0){
        subMenu(course);
    }
    getStudentScores(course, student_choice);
}
void getStudentScores(Course course, int studentNo){
    printf("Assign Name \t Score \t       Comment\n");
    printf("------------\t-------\t       --------\n");
    float avg=0;
    for(int i=0; i<course.num_assigns; i++){
        printf("%s   \t %f \t%s    \n", course.assigns[i].name, course.scores[i][studentNo-1].score, course.scores[i][studentNo-1].comment);
        avg += course.scores[i][studentNo-1].score;
    }
    avg = avg/course.num_assigns;
    printf("%s's final grade was %f", students[studentNo-1].name, avg);
}
void assignmentMenu(Course course){
    printf("Please choose from the following assignments: \n");
    printAssigns(course.assigns, course.num_assigns);
    printf("0 RETURN TO PREVIOUS MENU\n");
    int assign_choice;
    scanf("%d", &assign_choice);
    if(assign_choice == 0){
        subMenu(course);
    }
    getAssignmentScore(course, assign_choice);
}
void getAssignmentScore(Course course, int assignmentNo){
    float avg=0;
    for(int j=0; j<course.num_assigns; j++){
        avg += course.scores[assignmentNo-1][j].score;
    }
    avg = avg/numStudents;
    printf("The average grade on %s was %f", course.assigns[assignmentNo-1].name, avg);
}

void printStudents(){
    for(int i=0; i<numStudents; i++){
        printf("%d %s\n", students[i].id, students[i].name);
    }
}
void printAssigns(Assign* assigns, int numAssigns){
    for(int i=0; i<numAssigns; i++){
        printf("%d %s\n", assigns[i].id, assigns[i].name);
    }
}
void printGrades(ScoreStruct** scores, int numAssigns);
void printCourse(Course course){
    printf("Course ID: %d\n", course.id);
    printf("Course Name: %s\n", course.name);
    printf("Teacher: %s\n", course.teacher);
    printf("Assigns: \n");
    for(int i=0; i<course.num_assigns; i++){
        printf("  %d  %s\n", course.assigns[i].id, course.assigns[i].name);
    }
    printf("Grade data: \n");
    for(int i=0; i<(numStudents*course.num_assigns); i++){
        for(int j=0; j<numStudents; j++){
            printf("%d %d %f %s\n", i+1, j+1, course.scores[i][j].score, course.scores[i][j].comment);
        }
    }
}

/**
* Prints basic usage instructions for the program to the command line
*/
void print_usage(){
  printf("USAGE:\n./LastNameCourseReader -d <data_file_name(char*)> -c <instruction_file_name(char*)>\n");
  printf("-d refers to the required input data file containing student & course information; this must be a valid .txt file\n");
  printf("-i refers to the optionally supported 'instruction file' that provides directions for how the program should execute without CLI input; \n\t must be a valid .txt.file\n");
}

void terminate(){
    studentDestructor();
    coursesDestructor();
  
    exit(1);
}

void studentDestructor(){
    for(int i=0; i<numStudents; i++){
        free(students[i].name);
    }
    free(students);
    students = NULL;
}

void coursesDestructor(){
    for(int i=0; i<numCourses; i++){
        free(courses[i].name);
        free(courses[i].teacher);
        assignsDestructor(courses[i].assigns, courses[i].num_assigns);
        scoresDestructor(courses[i].scores, courses[i].num_assigns);
    }
    free(courses);
    courses = NULL;
}
void assignsDestructor(Assign** assigns, int numAssign){
    for(int i=0; i<numAssign; i++){
        free(assigns[i]->name);
    }
    free(assigns);
    assigns = NULL;
}
void scoresDestructor(ScoreStruct*** scores, int numAssigns){
    for(int i=0; i<numAssigns; i++){
        for(int j=0; j<numStudents; j++){
            free(scores[i][j]->comment);
        }
    }
    for(int i=0; i<numAssigns; i++){
        for(int j=0; j<numAssigns; j++){
            free(scores[i][j]);
        }
        free(scores[i]);
    }
    free(scores);
    scores = NULL;
}

int main(int argc, char* argv[]){
  char* datafile;
  char* instructionfile;
  int opt;
  while((opt = getopt(argc, argv, ":d:i:")) != -1){
    switch(opt){
      case 'd':
        datafile = optarg;
        break;  
      case 'i':
        instructionfile = optarg;
        break;
      case ':':
		printf("option needs a value\n");
		break;
      case '?':
        printf("unknown option: %c\n", optopt);
        break;
    }
  }
  for (; optind < argc; optind++){
    printf("Given extra arguments: %s\n", argv[optind]);
  }  

  int dflen;
  if(datafile != NULL){
    dflen = strlen(datafile);
    if(dflen >= 5
		&& (strcmp(&datafile[dflen-4], ".txt") == 0)
        && (access(datafile, F_OK) != -1)){
      printf("Importing data from %s\n\n", datafile);
	  readFile(datafile);
    } else {
	  printf("Data file has an invalid name or does not exist.\n");
      print_usage();
	  exit(1);   
    }
  } else {
    printf("No data file name provided. This is a required field.\n");
    print_usage();
	exit(1);
  }

  int iflen;
  int ifval;
  if(instructionfile != NULL){
    iflen = strlen(instructionfile);
	if(iflen >= 5 
		&& (strcmp(&instructionfile[iflen-4], ".txt") == 0)
		&& (access(instructionfile, F_OK) != -1)){
	  printf("Performing instructions defined in %s:\n", instructionfile);
	  //uncomment below if doing this optional part of the assignment
	  //performInstructions(instructionfile);  	  
	} else {
      printf("Instruction file has an invalid name or does not exist.\n");
	  print_usage();
	  exit(1);
    }
  } else {
	printf("No instruction file provided. Using CLI:\n");
    mainMenu();
  }
  return 0;
}

#endif

