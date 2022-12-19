# include <stdio.h>
# include <string.h>
# include <ctype.h> 
# include <stdlib.h>

int line_counter = 0;   // Keeps current line number.(Empty lines and lines containing only whitespaces are also considered.)  

/*
Function Declarations
*/
int is_valid_id(char *id); 
int is_declared(char *str);
char **give_type(char *str);
int is_not_empty_str(char *str);
int is_empty_str(char *str);
void space_pass(char **line);
int contains_error(char *str);
char **expr(char *str);
char **term(char *str);
char **factor(char *str);


/*
Arrays containing variable names and dimensions if needed (matrices and vectors). 
Integer variables for storing current number of specified types. 
*/
char * scalar_ids[10000];
int scalar_count = 0;
char *vector_ids[10000][3];
int vector_count = 0;
char *matrix_ids[10000][3];
int matrix_count = 0;

/*
Main function with 2 arguments: 
    1)# variables  
    2)File name  
*/
int main(int argc, char *argv[]) {
    FILE *pf;   // File pointer to input file. 
    FILE *pf_out;   // File pointer to output file.
    char line[256];     // Char array containing characters of the line(Max. of 256 characters is assumed).
    int is_open_loop = 0;   // Variable for proper arrangment of open and closed curly braces within a for loop block. 2:Nested, 1:Basic
    int is_ok = 0;  //Indicates the necessity of continue statement for the while loop that reads the input file line by line. Used in cases where "continue" statement cannot be used directly.    
    if (argc != 2) {
        //Error: You didn't give the filename!
        return(1);
    }
    pf = fopen(argv[1], "r");
    if(pf == NULL) {
        //Error: Cannot open file! 
        return(1);
    }

    /*
    Name of the output file is modified 
    e.g. ex3.mat -> ex3.c 
    */
    char *out_name = strdup(argv[1]);   
    out_name[strlen(out_name) - 2] = '\0';
    out_name[strlen(out_name) - 1] = 'c';
    pf_out = fopen(out_name, "w");

    /*
    Necessary preprocessors commands are written in output file  
    */
    fprintf(pf_out, "#include \"functions.c\"\n");
    fprintf(pf_out, "#include <stdio.h>\n");
    fprintf(pf_out, "#include <stdlib.h>\n");
    fprintf(pf_out, "#include <string.h>\n");
    fprintf(pf_out, "#include <ctype.h>\n");
    fprintf(pf_out, "#include <math.h>\n");
    fprintf(pf_out, "int main() {\n");

    /*  
    Main while loop that reads the input file line-by-line 
    */
    while (fgets(line,256,pf) != NULL) {
        is_ok = 0;
        line_counter++;
        char *line_main = strdup(line); // Duplicate of current input line.
        char *token;    // Pointer to access strsep() return value.
        char *line_dup;     // Current input line without new line character.
        line_dup = strsep(&line_main,"\n");
        space_pass(&line_dup);
        
        /*
        When line is an empty line or contains only whitespaces.
        */
        if (is_empty_str(line_dup)) {
            continue;
        }

        /*
        If statement for checking for loop close statement.
        */
        if (line_dup[0] == '}') {
            if (is_open_loop) {
                line_dup ++;
                space_pass(&line_dup);
                if (is_not_empty_str(line_dup)) {
                    printf("Error (Line %d)\n", line_counter);   // If a line contains } no more characters are allowed.
                    return(1);
                }
                /*For loop to print necessary number of closed curly brace(s).*/
                for (int i = 0; i < is_open_loop; i++) {
                    fprintf(pf_out,"%s","}\n");
                }
                is_open_loop = 0;
                continue;
            }else {
                printf("Error (Line %d)\n", line_counter);   // There is no open loop to be closed.
                return(1);
            }
        }

        /*
        Check if '#' is present.
        If present, checks the location of #.
        */
        if (strchr(line, '#') != NULL) { 
            space_pass(&line_dup);
            if (line_dup[0] != '#') {
                printf("Error (Line %d)\n", line_counter);    // Syntax Error for Comment, '#' not placed in first index.
                return(1);
            }else{
                continue;
            }
        }
        /*
        This conditional block looks for "printsep" keyword or "print" keyword. 
        */
        if(strstr(line_dup, "printsep") != NULL){
            space_pass(&line_dup);
            
            /*rstrip the string.*/
            while(line_dup[strlen(line_dup)-1] == ' '){
                line_dup[strlen(line_dup)-1] = '\0' ;
            }

            /*Looks for open and closed paranthesis.*/
            if(strchr(line_dup, '(') == NULL){
                printf("Error (Line %d): Syntax Error for printsep()!\n", line_counter);
                return(1);
            }
            if(strchr(line_dup, ')') == NULL){
                printf("Error (Line %d): Syntax Error for printsep()!\n", line_counter);
                return(1);
            }

            token = strsep(&line_dup, "(");

            /*rstrip the string.*/
            while(token[strlen(token)-1] == ' '){
                token[strlen(token)-1] = '\0' ;
            }

            if(strcmp(token, "printsep") != 0){
                //Char(s) between "printsep" and "(" are not allowed. 
                printf("Error (Line %d)\n", line_counter);
                return(1);
            }
            space_pass(&line_dup);
            if(strcmp(line_dup, ")") != 0){
                // After "printsep(" there can be only ")".  
                printf("Error (Line %d)\n", line_counter);
                return(1);
            }
            fprintf(pf_out,"printsep();\n");
            continue;

        }else if(strstr(line_dup, "print") != NULL){
            space_pass(&line_dup);
            if(strchr(line_dup, '(') == NULL){
                // Looks for open paranthesis.
                printf("Error (Line %d)\n", line_counter);
                return(1);
            }
            token = strsep(&line_dup, "(");

            /*rstrip the string.*/
            while(token[strlen(token)-1] == ' '){
                token[strlen(token)-1] = '\0' ;
            }

            if(strcmp(token, "print") != 0){
                //Char(s) between "print" and "(" are not allowed. 
                printf("Error (Line %d)\n", line_counter);
                return(1);
            }

            char *last_occ = strrchr(line_dup, ')');
            if(last_occ == NULL){
                // Looks for last occurence of closed paranthesis.
                printf("Error (Line %d)\n", line_counter);
                return 1;
            }
            *last_occ = '\0';
            char **argument_p;  // Pointer used to keep array of strings. Index 0: Modified line translated to c, Index 1: Type, Index 2&3: Dimensions 
            argument_p = expr(line_dup);    
            if(contains_error(argument_p[0])){
                // Invalid Expression
                printf("Error (Line %d)\n", line_counter);
                return 1;
            }

            /*
            Checks the type of expression and writes the necessary print function to output file.
            */
            if(strcmp(argument_p[1], "s") == 0){
                fprintf(pf_out, "print_s(%s);\n", argument_p[0]);
            }else if(strcmp(argument_p[1], "m") == 0) {
                fprintf(pf_out, "print_m(%s,%s,%s);\n", argument_p[0], argument_p[2], argument_p[3]);
            }
            continue;
        }

        /*
        If statement to check assignment statement.
        */
        if(strchr(line_dup, '=') != NULL) {
            token = strsep(&line_dup,"=");  
            space_pass(&token);
            if(is_empty_str(token)) {
                // Left side of assignment is empty.
                printf("Error (Line %d)\n", line_counter);
                return 1;
            }
            char *assign_var;   // Left side of assignment statement.
            if (strchr(token, ' ') != NULL) {
                assign_var = strsep(&token," ");
                space_pass(&token);
                if(is_not_empty_str(token)) {
                    // Multiple words on the left side of assignment statement is not allowed.
                    printf("Error (Line %d)\n", line_counter);
                    return 1;
                }
            }else{
                assign_var = strdup(token);
            }
            if (!is_declared(assign_var)) {
                //Variable is not declared. 
                printf("Error (Line %d)\n", line_counter);
                return 1;
            }
            char **type_l = give_type(assign_var);  // Pointer used to keep array of strings. Index 0: Type, Index 1&2: Dimensions
            char **type_r;  // Pointer used to keep array of strings. Index 0: Modified line translated to c, Index 1: Type, Index 2&3: Dimensions
        
            space_pass(&line_dup);

            /*Detects assignment with curly braces*/
            if (strchr(line_dup, '{') != NULL) {
                if (strcmp(type_l[0], "0") == 0) {
                    // Matrix can not be assigned to a scalar variable.
                    printf("Error (Line %d)\n", line_counter);
                    return 1;
                }
                if (line_dup[0] != '{') {
                    // Line contains { but it is not in the first place.
                    printf("Error (Line %d)\n", line_counter);
                    return 1;
                }else if(strchr(line_dup, '}') == NULL) {
                    // Line contains { but it is not closed
                    printf("Error (Line %d)\n", line_counter);
                    return 1;
                }

                /*rstrip the string.*/
                while(line_dup[strlen(line_dup)-1] == ' '){
                line_dup[strlen(line_dup)-1] = '\0' ;
                }
                line_dup += 1;
                if(strchr(line_dup, '{') != NULL){
                    // Line contains more than one {'s
                    printf("Error (Line %d)\n", line_counter);
                    return 1;
                }
                if(line_dup[strlen(line_dup)-1] != '}'){
                    // Last character is not '}'.
                    printf("Error (Line %d)\n", line_counter);
                    return 1;
                }
                line_dup[strlen(line_dup)-1] = '\0';
                if(strchr(line_dup, '}') != NULL){
                    // Line contains more than one '}'s.
                    printf("Error (Line %d)\n", line_counter);
                    return 1;
                }

                /*
                Dimensions in string converted to int type.
                */
                int dim1 = atoi(type_l[1]);
                int dim2 = atoi(type_l[2]);
                int size = dim1 * dim2;
                space_pass(&line_dup);
                char *elements;   // Keeps elements inside curly braces. 
                elements = (char*) calloc(128,sizeof(char));
                strcat(elements, "{");
                char *elm;  // Keeps single element in string form.
                int count_v = 0;    // Counts # elements.
                if(is_empty_str(line_dup)){
                    // Nothing inside curly braces.
                    printf("Error (Line %d)\n", line_counter);
                    return 1;
                }

                /*Separation of elements.*/
                while((elm = strsep(&line_dup, " ")) != NULL){
                    if (is_empty_str(elm)) {
                        // Multiple spaces between elements is allowed.
                        continue;
                    }
                    if (line_dup != NULL) {
                        space_pass(&line_dup);
                    }
                    char *num = (char*) calloc(16,sizeof(char));
                    int only_dot = 0;   // Control variable for detecting multiple dots in an a scalar.
                    if (elm[0] == '.' || elm[strlen(elm) - 1] == '.') {
                        // Scalar cannot begin or end with a dot.
                        printf("Error (Line %d)\n", line_counter);
                        return 1;
                    }
                    /*
                    Loop concatenating single digits into a string.
                    */
                    for(int i = 0; i < strlen(elm); i++){
                        if (elm[i] == '.') {
                            if (only_dot == 0) {
                                only_dot = 1;
                                num[strlen(num)] = elm[i];
                                continue;
                            }else {
                                // Multiple dots not allowed.
                                printf("Error (Line %d)\n", line_counter);
                                return 1;
                            }
                        }
                        if(isdigit(elm[i]) == 0){
                            // Element cannot have characters other than dot or digit.
                            printf("Error (Line %d)\n", line_counter);
                            return 1;
                        }
                        num[strlen(num)] = elm[i];
                    }
                    count_v += 1;
                    strcat(elements,"(double) ");
                    strcat(elements, num);
                    strcat(elements, ",");
                    free(num);
                }
                int is_not_null = 1;    // Control variable to check whether line_dup is null or not. 
                if (line_dup == NULL) {
                    if (count_v == 0) {
                        return 1;
                    }
                    is_not_null = 0;
                }
                if((is_not_null) && is_not_empty_str(line_dup)){
                    char *num = (char*) calloc(16,sizeof(char));
                    if (line_dup[0] == '.' || line_dup[strlen(line_dup) - 1] == '.') {
                        // Scalar cannot begin or end with a dot.
                        printf("Error (Line %d)\n", line_counter);
                        return 1;
                    }
                    int only_dot = 0;
                    for(int i = 0; i < strlen(line_dup); i++){
                        if (line_dup[i] == '.') {
                            if (only_dot == 0) {
                                only_dot = 1;
                                num[strlen(num)] = line_dup[i];
                                continue;
                            }else {
                                // Multiple dots not allowed.
                                printf("Error (Line %d)\n", line_counter);
                                return 1;
                            }
                        }
                        if(isdigit(line_dup[i]) == 0){
                            // Element cannot have characters other than dot or digit.
                            printf("Error (Line %d)\n", line_counter);
                            return 1;
                        }
                        num[strlen(num)] = line_dup[i];
                    }
                    count_v += 1;
                    strcat(elements,"(double) ");
                    strcat(elements, num);
                    strcat(elements, "}");
                    free(num);
                }else {
                    elements[strlen(elements) - 1] = '}';
                }
                if(count_v != size){
                    // Size of left and right sides don't match.
                    printf("Error (Line %d)\n", line_counter);
                    return 1;
                }
                fprintf(pf_out, "double elms_%d[%d] = %s;\n", line_counter,size,elements);
                fprintf(pf_out, "assign_var(&%s, elms_%d, %d, %d);\n", assign_var, line_counter, dim1, dim2);
                continue;
            }
            type_r = expr(line_dup);
            if (contains_error(type_r[0])) {
                // Invalid Expression.
                printf("Error (Line %d)\n", line_counter);
                return 1;
            }
            if (strcmp(type_l[0], "0") == 0) {
                if (strcmp(type_r[1], "s") != 0) {
                    //Assignment of non-scalar expression to a scalar type is not allowed.
                    printf("Error (Line %d)\n", line_counter);
                    return 1;
                }
            }else{
                if (strcmp(type_r[1], "s") == 0) {
                    //Assignment of scalar expression to a non-scalar type is not allowed.
                    printf("Error (Line %d)\n", line_counter);
                    return 1;
                }
                if( (strcmp(type_l[1], type_r[2]) != 0 || strcmp(type_l[2], type_r[3]) != 0) ){
                    // Dimension of left and right sides don't match.
                    printf("Error (Line %d)\n", line_counter);
                    return 1;
                }
            }
            fprintf(pf_out,"%s = %s;\n", assign_var, type_r[0]);
            continue;
        }
        char * line_dup2 = strdup(line_dup);

        /*
        Loop to process declaration statements. 
        */
        while((token = strsep(&line_dup," ")) != NULL){
            /*Variables to keep dimension(s) of vectors & matrices.*/
            char *dimension_r;
            char *dimension_l;
            char *dimension;

            if(token[0] == '\0'){
                continue;
            }
            if(line_dup == NULL){
                break;
            }

            /*
            Conditional block to identify declaration keyword.
            */
            if(strcmp(token, "scalar") == 0) {
                space_pass(&line_dup);
                token = strsep(&line_dup," ");
                if(is_valid_id(token) == 0){
                    //Invalid ID.
                    printf("Error (Line %d)\n", line_counter);
                    return(1);
                }
                if (is_declared(token)) {
                    //Redeclaration is not allowed.
                    printf("Error (Line %d)\n", line_counter);
                    return(1);
                }
                scalar_ids[scalar_count] = strdup(token);   // Adds Id of scalar.
                scalar_count += 1;
                while((token = strsep(&line_dup," ")) != NULL){
                    if(token[0] == '\0'){
                        continue;
                    }else{
                        //More than 2 tokens on definition type is not allowed.
                        printf("Error (Line %d)\n", line_counter);
                        return(1);
                    }
                }
                fprintf(pf_out,"double %s;\n", scalar_ids[scalar_count-1]);
                is_ok = 1;

            }else if(strcmp(token, "vector") == 0) {
                space_pass(&line_dup);
                if (strchr(line_dup, '[') == NULL) { 
                    //No open bracket for dimension of vector.
                    printf("Error (Line %d)\n", line_counter);
                    return(1);
                }
                token = strsep(&line_dup,"[");
                if(is_valid_id(token) == 0){
                    //Invalid ID.
                    printf("Error (Line %d)\n", line_counter);
                    return(1);
                }
                if (is_declared(token)) {
                    //Redeclaration is not allowed.
                    printf("Error (Line %d)\n", line_counter);
                    return(1);
                }
                vector_ids[vector_count][0] = strdup(token);    // Adds Id of vector.
                vector_count += 1;
                if (strchr(line_dup, ']') == NULL) {
                    //No closed bracket for dimension of vector.
                    printf("Error (Line %d)\n", line_counter);
                    return(1);
                }
                token = strsep(&line_dup,"]");
                space_pass(&token);
                if(is_empty_str(token)){
                    // Size of vector should be specified.
                    printf("Error (Line %d)\n", line_counter);
                    return 1;
                }
                dimension = (char*) calloc(16,1);   // String to be concatenated with digit characters of dimension.
                while (token[0] != ' ' && token[0] != '\0') {
                    if (isdigit(token[0]) == 0) {
                        //Non-digit character is not allowed for dimension.
                        printf("Error (Line %d)\n", line_counter);
                        return(1);
                    }
                    dimension[strlen(dimension)] = token[0];
                    token += 1;
                }
                /*Adds dimension information.*/
                vector_ids[vector_count - 1][1] = strdup(dimension);
                vector_ids[vector_count - 1][2] = "1";
                free(dimension);
                
                space_pass(&token);
                //Last Checks.
                if(is_not_empty_str(token)){
                    printf("Error (Line %d)\n", line_counter);
                    return 1;
                }
                space_pass(&line_dup);
                if(is_not_empty_str(line_dup)){
                    printf("Error (Line %d)\n", line_counter);
                    return 1;
                }
                fprintf(pf_out, "double **%s;\n",  vector_ids[vector_count - 1][0]);
                fprintf(pf_out, "declare_var(&%s, %s, 1);\n", vector_ids[vector_count - 1][0], vector_ids[vector_count - 1][1]);
                is_ok = 1;
            }else if(strcmp(token, "matrix") == 0) {
                space_pass(&line_dup);
                if (strchr(line_dup, '[') == NULL) { 
                    //No open bracket for dimension of matrix.
                    printf("Error (Line %d)\n", line_counter);
                    return(1);
                }
                token = strsep(&line_dup,"[");
            
                if(is_valid_id(token) == 0){
                    //Invalid ID.
                    printf("Error (Line %d)\n", line_counter);
                    return(1);
                }
                if (is_declared(token)) {
                    //Redeclaration is not allowed.
                    printf("Error (Line %d)\n", line_counter);
                    return(1);
                }
                matrix_ids[matrix_count][0] = strdup(token);     // Adds Id of matrix.
                matrix_count += 1;
                if (strchr(line_dup, ']') == NULL) {
                    //No closed bracket for dimension of matrix.
                    printf("Error (Line %d)\n", line_counter);
                    return(1);
                }
                token = strsep(&line_dup,"]");
                space_pass(&token);
                if(is_empty_str(token)){
                    // Dimensions of matrix should be specified.
                    printf("Error (Line %d)\n", line_counter);
                    return 1;
                }
                if (strchr(token, ',') == NULL) {
                    // Dimensions must be separeted by comma. 
                    printf("Error (Line %d)\n", line_counter);
                    return(1);
                }
                char *token_l;
                token_l = strsep(&token,",");
                dimension_l = (char*) calloc(16, 1);    // String to be concatenated with digit characters of first dimension of matrix.
                while (token_l[0] != ' ' && token_l[0] != '\0') {
                    if (isdigit(token_l[0]) == 0) {
                        //Non-digit character is not allowed for dimension.
                        printf("Error (Line %d)\n", line_counter);
                        return(1);
                    }
                    dimension_l[strlen(dimension_l)] = token_l[0];
                    token_l += 1;
                }
                matrix_ids[matrix_count - 1][1] = strdup(dimension_l);  //Adds first dimension information.
                free(dimension_l);
                space_pass(&token_l);
                //Double Check.
                if(is_not_empty_str(token_l)){
                    printf("Error (Line %d)\n", line_counter);
                    return 1;
                }
                space_pass(&token);
                if(is_empty_str(token)){
                    // Second dimension info is not given.
                    printf("Error (Line %d)\n", line_counter);
                    return 1;
                }
                dimension_r = (char*) calloc(16,1);   // String to be concatenated with digit characters of second dimension of matrix.
                while (token[0] != ' ' && token[0] != '\0') {
                    if (isdigit(token[0]) == 0) {
                        //Non-digit character is not allowed for dimension.
                        printf("Error (Line %d)\n", line_counter);
                        return(1);
                    }
                    dimension_r[strlen(dimension_r)] = token[0];
                    token += 1;
                }
                matrix_ids[matrix_count - 1][2] = strdup(dimension_r);  //Adds second dimension information.
                free(dimension_r);
                space_pass(&token);
                //Double Check
                if(is_not_empty_str(token)){
                    printf("Error (Line %d)\n", line_counter);
                    return 1;
                }
                space_pass(&line_dup);
                if(is_not_empty_str(line_dup)){
                    printf("Error (Line %d)\n", line_counter);
                    return 1;
                }
                fprintf(pf_out, "double **%s;\n", matrix_ids[matrix_count - 1][0]);
                fprintf(pf_out, "declare_var(&%s, %s, %s);\n", matrix_ids[matrix_count - 1][0], matrix_ids[matrix_count - 1][1],  matrix_ids[matrix_count - 1][2]);
                is_ok = 1;
            }
        }
        space_pass(&line_dup2);
        // If statement for detecting for loop with open paranthesis.
        if(strchr(line_dup2, '(') != NULL){
            token = strsep(&line_dup2, "(");
            //rstrip the string.
            while(token[strlen(token)-1] == ' '){
                token[strlen(token)-1] = '\0' ;
            }
            if(strcmp(token, "for") == 0){
                if(is_open_loop == 1){
                    return 1;
                }
                space_pass(&line_dup2);
                char *control_str = strdup(line_dup2);  // Checking proper arrangement of for loop header.  
                char *index_of_str = strstr(control_str, " in "); 
                if(index_of_str != NULL){
                    *index_of_str = '\0';
                }
                if(strchr(control_str, ',') != NULL){
                    //Nested for loop
                    char *var1_for;    // First id of nested for loop.
                    var1_for = strsep(&line_dup2, ",");
                    if(is_empty_str(var1_for) || (is_declared(var1_for) == 0) ){
                        //ID should be declared.
                        printf("Error (Line %d)\n", line_counter);
                        return(1);
                    }
                    if ( strcmp(give_type(var1_for)[0], "0") != 0){
                        //ID must be a scalar.
                        printf("Error (Line %d)\n", line_counter);
                        return(1);
                    }
                    space_pass(&line_dup2);
                    char *var2_for;    // Second id of nested for loop.
                    var2_for = strsep(&line_dup2, " ");
                    if(is_empty_str(var2_for) || (is_declared(var2_for) == 0) ){                    
                        //ID should be declared.
                        printf("Error (Line %d)\n", line_counter);
                        return(1);
                    }
                    if ( strcmp(give_type(var2_for)[0], "0") != 0){
                        //ID must be a scalar.
                        printf("Error (Line %d)\n", line_counter);
                        return(1);
                    }
                    space_pass(&line_dup2);
                    if( (line_dup2[0] != 'i') || (line_dup2[1] != 'n') ){
                        // After two variables "in" keyword must be present.  
                        printf("Error (Line %d)\n", line_counter);
                        return(1);
                    }
                    line_dup2 += 2;
                    space_pass(&line_dup2);
                    if(strchr(line_dup2, ':') == NULL){
                        // Expressions must be separeted with ':'.
                        printf("Error (Line %d)\n", line_counter);
                        return(1);
                    }
                    char *exp1_1;   //Expression regarding of initial value of first id.
                    exp1_1 = strsep(&line_dup2, ":");
                    char **checker1_1;  // Pointer used to keep array of strings. Index 0: Modified line translated to c, Index 1: Type, Index 2&3: Dimension 
                    checker1_1 = expr(exp1_1);
                    if( (strcmp(checker1_1[1], "s") != 0) || (contains_error(checker1_1[0])) ){
                        // Expression must be scalar and should not contain any errors.
                        printf("Error (Line %d)\n", line_counter);
                        return(1);
                    }

                    if(strchr(line_dup2, ':') == NULL){
                        // Expressions must be separeted with ':'.
                        printf("Error (Line %d)\n", line_counter);
                        return(1);
                    }
                    char *exp2_1;   //Expression regarding of boundary value of first id.
                    exp2_1 = strsep(&line_dup2, ":");
                    char **checker2_1;  // Pointer used to keep array of strings. Index 0: Modified line translated to c, Index 1: Type, Index 2&3: Dimension 
                    checker2_1 = expr(exp2_1);
                    if( (strcmp(checker2_1[1], "s") != 0) || (contains_error(checker2_1[0])) ){
                        // Expression must be scalar and should not contain any errors.
                        printf("Error (Line %d)\n", line_counter);
                        return(1);
                    }
                    if(strchr(line_dup2, ',') == NULL){
                        // Nested for loops' expressions must be separeted by comma.
                        printf("Error (Line %d)\n", line_counter);
                        return(1);
                    }
                    char *exp3_1;   //Expression regarding of iterated incremention value of first id.
                    exp3_1 = strsep(&line_dup2, ",");
                    char **checker3_1;  // Pointer used to keep array of strings. Index 0: Modified line translated to c, Index 1: Type, Index 2&3: Dimension 
                    checker3_1 = expr(exp3_1);
                    if( (strcmp(checker3_1[1], "s") != 0) || (contains_error(checker3_1[0])) ){
                        // Expression must be scalar and should not contain any errors.
                        printf("Error (Line %d)\n", line_counter);
                        return(1);
                    }
                    //VAR2
                    char *exp1_2;    //Expression regarding of initial value of second id.
                    exp1_2 = strsep(&line_dup2, ":");   
                    char **checker1_2;  // Pointer used to keep array of strings. Index 0: Modified line translated to c, Index 1: Type, Index 2&3: Dimension 
                    checker1_2 = expr(exp1_2);
                    if( (strcmp(checker1_2[1], "s") != 0) || (contains_error(checker1_2[0])) ){
                        // Expression must be scalar and should not contain any errors.
                        printf("Error (Line %d)\n", line_counter);
                        return(1);
                    }
                    if(strchr(line_dup2, ':') == NULL){
                        // Expressions must be separeted with ':'.
                        printf("Error (Line %d)\n", line_counter);
                        return(1);
                    }
                    char *exp2_2;   //Expression regarding of boundary value of second id.
                    exp2_2 = strsep(&line_dup2, ":");
                    char **checker2_2;  // Pointer used to keep array of strings. Index 0: Modified line translated to c, Index 1: Type, Index 2&3: Dimension 
                    checker2_2 = expr(exp2_2);
                    if( (strcmp(checker2_2[1], "s") != 0) || (contains_error(checker2_2[0])) ){
                        // Expression must be scalar and should not contain any errors.
                        printf("Error (Line %d)\n", line_counter);
                        return(1);
                    }
                    if(strchr(line_dup2, ')') == NULL){
                        // For loop header must contain ')'.
                        printf("Error (Line %d)\n", line_counter);
                        return(1);
                    }
                    char *exp3_2;   //Expression regarding of iterated incremention value of second id.
                    exp3_2 = strsep(&line_dup2, ")");
                    char **checker3_2;  // Pointer used to keep array of strings. Index 0: Modified line translated to c, Index 1: Type, Index 2&3: Dimension 
                    checker3_2 = expr(exp3_2);
                    if( (strcmp(checker3_2[1], "s") != 0) || (contains_error(checker3_2[0])) ){
                        // Expression must be scalar and should not contain any errors.
                        printf("Error (Line %d)\n", line_counter);
                        return(1);
                    }
                    space_pass(&line_dup2);
                    if(line_dup2[0] != '{'){
                        // Last character of line containing for loop header must be '{'.
                        printf("Error (Line %d)\n", line_counter);
                        return(1);
                    }
                    line_dup2 += 1;
                    space_pass(&line_dup2);
                    if(is_not_empty_str(line_dup2)){
                        //No more characters are allowed after '{'.
                        printf("Error (Line %d)\n", line_counter);
                        return(1);
                    }
                    is_open_loop = 2;
                    fprintf(pf_out, "for(%s = %s; %s <= %s; %s += %s){\n", var1_for, checker1_1[0], var1_for, checker2_1[0],var1_for, checker3_1[0] );
                    fprintf(pf_out, "for(%s = %s; %s <= %s; %s += %s){\n", var2_for, checker1_2[0], var2_for, checker2_2[0],var2_for, checker3_2[0] );

                }else{
                    //Basic for loop
                    char *var_for;  // Id of for loop.
                    var_for = strsep(&line_dup2, " ");
                    if(is_empty_str(var_for) || (is_declared(var_for) == 0) ){
                        //ID should be declared.
                        printf("Error (Line %d)\n", line_counter);
                        return(1);
                    }
                    if ( strcmp(give_type(var_for)[0], "0") != 0){
                        //ID must be a scalar.
                        printf("Error (Line %d)\n", line_counter);
                        return(1);
                    }
                    space_pass(&line_dup2);
                    if( (line_dup2[0] != 'i') || (line_dup2[1] != 'n') ){
                        // After two variables "in" keyword must be present.  
                        printf("Error (Line %d)\n", line_counter);
                        return(1);
                    }
                    line_dup2 += 2;
                    space_pass(&line_dup2);
                    if(strchr(line_dup2, ':') == NULL){
                        // Expressions must be separeted with ':'.
                        printf("Error (Line %d)\n", line_counter);
                        return(1);
                    }
                    char *exp1;    //Expression regarding of initial value of id.
                    exp1 = strsep(&line_dup2, ":");
                    char **checker1;    // Pointer used to keep array of strings. Index 0: Modified line translated to c, Index 1: Type, Index 2&3: Dimension 
                    checker1 = expr(exp1);
                    if( (strcmp(checker1[1], "s") != 0) || (contains_error(checker1[0])) ){
                        // Expression must be scalar and should not contain any errors.
                        printf("Error (Line %d)\n", line_counter);
                        return(1);
                    }

                    if(strchr(line_dup2, ':') == NULL){
                        // Expressions must be separeted with ':'.
                        printf("Error (Line %d)\n", line_counter);
                        return(1);
                    }
                    char *exp2;    //Expression regarding of boundary value of id.
                    exp2 = strsep(&line_dup2, ":");
                    char **checker2;    // Pointer used to keep array of strings. Index 0: Modified line translated to c, Index 1: Type, Index 2&3: Dimension    
                    checker2 = expr(exp2);
                    if( (strcmp(checker2[1], "s") != 0) || (contains_error(checker2[0])) ){
                        // Expression must be scalar and should not contain any errors.
                        printf("Error (Line %d)\n", line_counter);
                        return(1);
                    }

                    if(strchr(line_dup2, ')') == NULL){
                        // For loop header must contain ')'.
                        printf("Error (Line %d)\n", line_counter);
                        return(1);
                    }
                    char *exp3;    //Expression regarding of iterated incremention value of id.
                    exp3 = strsep(&line_dup2, ")");
                    char **checker3;    // Pointer used to keep array of strings. Index 0: Modified line translated to c, Index 1: Type, Index 2&3: Dimension 
                    checker3 = expr(exp3);
                    if( (strcmp(checker3[1], "s") != 0) || (contains_error(checker3[0])) ){
                        // Expression must be scalar and should not contain any errors.
                        printf("Error (Line %d)\n", line_counter);
                        return(1);
                    }
                    space_pass(&line_dup2);
                    if(line_dup2[0] != '{'){
                        // Last character of line containing for loop header must be '{'.
                        printf("Error (Line %d)\n", line_counter);
                        return(1);
                    }
                    line_dup2 += 1;
                    space_pass(&line_dup2);
                    if(is_not_empty_str(line_dup2)){
                        //No more characters are allowed after '{'.
                        printf("Error (Line %d)\n", line_counter);
                        return(1);
                    }
                    is_open_loop = 1;
                    fprintf(pf_out, "for(%s = %s; %s <= %s; %s += %s){\n", var_for, checker1[0], var_for, checker2[0],var_for, checker3[0] );
                }
            }else{
                //Line contains '(' and cannot be matched with for loop call. 
                printf("Error (Line %d)\n", line_counter);
                return(1);
            }
            is_ok = 1;
        }
        if (is_ok == 0) {
            // Line cannot be matched with any syntatic behavior.
            printf("Error (Line %d)\n", line_counter);
            return(1);
        }
    }
    if(is_open_loop){
        //For loop block remains open even after the last line.
        printf("Error (Line %d)\n", line_counter);
        return 1;
    }
    fprintf(pf_out, "return 0;\n");
    fprintf(pf_out, "}");
    fclose(pf);
    fclose(pf_out);
}

/*
Function checks whether given string is a valid id.
Args: String to be checked.
Returns: int 1,If valid 
         int 0,If invalid
*/
int is_valid_id(char *idnt){
    
    char *id;
    if(idnt == NULL){
        // NULL cannot be a valid ID.
        return 0;
    }
    if(strchr(idnt, ' ') != NULL){
        id = strsep(&idnt," ");
        while(idnt[0] == ' '){
            idnt += 1;
        }
        if(idnt[0] != '\0'){
            //Two separete words cannot refer to an ID. 
            return 0;
        }
    }else{
        id = idnt;
    }
    char *keywords[] = {"scalar", "vector", "matrix", "for", "print", "tr", "sqrt", "in",
     "printsep", "choose"};
     for (int i = 0; i < 10; i++) {
         if (strcmp(id, keywords[i]) == 0) {
            //Keywords cannot refer to an ID. 
            return(0);
         }
    }
    if(isalpha(id[0]) == 0){
        // First character should be alphabetical.
        return(0);
    }
    char *str_dup = strdup(id);
    for(int i = 1; i < strlen(str_dup); i++){
        if(isalpha(str_dup[i]) != 0 || isdigit(str_dup[i]) != 0) {
            continue;
        }
        // Characters must be alphanumeric.
        return 0;
    }
    //If all conditions satisfied it's a valid ID.
    return(1);
}

/*
Function lstrips the string.
Args: Address of the string to be modified.
Returns: void type
*/
void space_pass(char **line) {
    while( (((char*)(*line))[0] == ' ') || (((char*)(*line))[0] == '\t') ){
        *line = *line + 1;
    }
}

/*
Function checks whether given string is not empty.
Args: String to be checked.
Returns: int 1, If not empty
         int 0, If empty
*/
int is_not_empty_str(char *str){
    if (str[0] != '\0') {
        return(1);
    }
    return 0;
}

/*
Function checks whether given string is empty.
Args: String to be checked.
Returns: int 1, If empty
         int 0, If not empty
*/
int is_empty_str(char *str){
    if (str[0] == '\0') {
        return(1);
    }
    return 0;
}

/*
Function checks whether given ID as string is declared by iterating over all types of arrays.
Args: String to be checked.
Returns: int 1, If already declared
         int 0, If not declared
*/
int is_declared(char *str){
    for (int i = 0; i < scalar_count; i++) {
        if (strcmp(str, scalar_ids[i]) == 0) {
            return 1;
        }
    }
    for(int i = 0; i < vector_count; i++ ){
         if (strcmp(str, vector_ids[i][0]) == 0) {
            return 1;
        }
    }
    for (int i = 0; i < matrix_count; i++) {
         if (strcmp(str, matrix_ids[i][0]) == 0) {
            return 1;
        }
    }
    return 0;
}

/*
Function checks whether given string is syntactically incorrect. 
IMPORTANT NOTE: A string contains '!' if it has an error according to our implementation of grammar functions.
Args: String to be checked.
Returns: int 1, If contains error.
         int 0, If does not contain error.
*/
int contains_error(char * str) {
    if (strchr(str, '!') != NULL) {
        return 1;
    }else {
        return 0;
    }
}

/*
Function returns necessary type information of argument. 
Iterates through all array of types, finds necessary information about the variable.
Args: String to be inspected.
Returns: Array[3] of strings. 
    Index 0: Type. (O If scalar, 1 If vector, 2 If matrix)
    Index 1&2: Dimensions If vector or matrix, If scalar still returns 0,0 but never used. 
*/
char **give_type(char *str){
    char ** to_return;  // Pointer used to keep array of strings. Index 0: Type, Index 1&2: Dimensions
    to_return = (char **) calloc(8, sizeof(char *));
    to_return[0] = (char *) calloc(4, sizeof(char));
    to_return[1] = (char *) calloc(16, sizeof(char));
    to_return[2] = (char *) calloc(16, sizeof(char));
    for (int i = 0; i < scalar_count; i++) {
        if (strcmp(str, scalar_ids[i]) == 0) {
            to_return[0] = "0";
            return to_return;
        }
    }
    for(int i = 0; i < vector_count; i++ ){
         if (strcmp(str, vector_ids[i][0]) == 0) {
            to_return[0] = "1";
            to_return[1] = vector_ids[i][1];
            to_return[2] = vector_ids[i][2];
            return to_return;
        }
    }
    for (int i = 0; i < matrix_count; i++) {
         if (strcmp(str, matrix_ids[i][0]) == 0) {
            to_return[0] = "2";
            to_return[1] = matrix_ids[i][1];
            to_return[2] = matrix_ids[i][2];
            return to_return;
        }
    }
}

/*
Recursive function implements BNF of the grammar. 
Detailed explanation of algorithm is in documentation.
IMPORTANT NOTE: 0th index of return value (Modified line translated to c) contains '!' if syntactically incorrect. Safe because character is not a valid one in matlang language. 
*/
char ** expr(char *str){ 
    char ** to_return;  // Pointer used to keep array of strings. Index 0: Modified line translated to c, Index 1: Type, Index 2&3: Dimensions
    to_return = (char **) calloc(8, sizeof(char *));    
    to_return[0] = (char *) calloc(128, sizeof(char));
    to_return[1] = (char *) calloc(4, sizeof(char));
    to_return[2] = (char *) calloc(16, sizeof(char));
    to_return[3] = (char *) calloc(16, sizeof(char));
    space_pass(&str);
    char *plus;
    char *minus;
    plus = strchr(str, '+');
    minus = strchr(str, '-');
    char **trm;
    char **exp;

    if (plus == NULL && minus == NULL) {
        trm = term(str);
        if (contains_error(trm[0]) == 1) {
            // Return value of term is syntactically incorrect.
            to_return[0] = "!";
            return to_return;
        }
        strcat(to_return[0], trm[0]);
        to_return[1] = trm[1];
        to_return[2] = trm[2];
        to_return[3] = trm[3];
        return to_return;
    }else{
        int swtch_p = 0;
        int swtch_b = 0;
        char * term1 = (char *) calloc(256, sizeof(char));
        int oper = -1; // 1 if plus, 0 if minus.
        for (int i = 0; i < strlen(str); i++) {
            if (swtch_p < 0 || swtch_b < 0) {
                // Arrangement of paranteses or brackets are syntactically incorrect. e.g. Unmatched parantheses 
                to_return[0] = "!";
                return to_return;
            }
            char ch = str[i];
            char * chs = (char *) calloc(2, sizeof(char));
            chs[0] = str[i];
            if (ch == '+' && swtch_p == 0 && swtch_b == 0) {
                str += (i + 1);
                oper = 1;
                break;
            }else if (ch == '-' && swtch_p == 0 && swtch_b == 0) {
                str += (i + 1);
                oper = 0;
                break;
            }
            if (ch == '(') {
                swtch_p ++;
            }else if (ch == ')'){
                swtch_p --;
            }else if (ch == '[') {
                swtch_b ++;
            }else if (ch == ']') {
                swtch_b --;
            }
            strcat(term1, chs);
        }
        if (oper == 1) {
            trm = term(term1);
            if (contains_error(trm[0]) == 1) {
                // Return value of term is syntactically incorrect.
                to_return[0] = "!";
                return to_return;
            }
            exp = expr(str);
            if (contains_error(exp[0]) == 1) {
                // Return value of expr is syntactically incorrect.
                to_return[0] = "!";
                return to_return;
            }
            if (strcmp(trm[1], "s") == 0 && strcmp(exp[1], "s") == 0) {
                strcat(to_return[0], trm[0]);
                strcat(to_return[0], "+");
                strcat(to_return[0], exp[0]);
                to_return[1] = "s";
                return to_return;
            }else if ( (strcmp(trm[1], "s") == 0 && strcmp(exp[1], "m") == 0) || (strcmp(trm[1], "m") == 0 && strcmp(exp[1], "s") == 0) ) {
                // Addition of scalar with matrix type is not allowed.
                to_return[0] = "!";
                return to_return;  
            }else if (strcmp(trm[1], "m") == 0 && strcmp(exp[1], "m") == 0){
                if((strcmp(trm[2], exp[2]) == 0) && (strcmp(trm[3], exp[3]) == 0)){
                    strcat(to_return[0], "mm_add(");  
                    strcat(to_return[0], trm[0]);
                    strcat(to_return[0], ",");
                    strcat(to_return[0], exp[0]);
                    strcat(to_return[0], ",");
                    strcat(to_return[0], trm[2]);
                    strcat(to_return[0], ",");
                    strcat(to_return[0], trm[3]);
                    strcat(to_return[0], ")");
                    to_return[1] = "m";
                    to_return[2] = trm[2];
                    to_return[3] = trm[3]; 
                    return to_return;                      
                }else{
                    // Dimension of matrices to be summed doesn't match.
                    to_return[0] = "!";
                    return to_return;
                }  
            }
        }else if(oper == 0){
            trm = term(term1);
            if (contains_error(trm[0]) == 1) {
                // Return value of term is syntactically incorrect.
                to_return[0] = "!";
                return to_return;
            }
            exp = expr(str);
            if (contains_error(exp[0]) == 1) {
                // Return value of expr is syntactically incorrect.
                to_return[0] = "!";
                return to_return;
            }
            if (strcmp(trm[1], "s") == 0 && strcmp(exp[1], "s") == 0) {
                strcat(to_return[0], trm[0]);
                strcat(to_return[0], "-");
                strcat(to_return[0], exp[0]);
                to_return[1] = "s";
                return to_return;
            }else if ( (strcmp(trm[1], "s") == 0 && strcmp(exp[1], "m") == 0) || (strcmp(trm[1], "m") == 0 && strcmp(exp[1], "s") == 0) ) {
                // Substraction of scalar with matrix type is not allowed.
                to_return[0] = "!";
                return to_return;  
            }else if (strcmp(trm[1], "m") == 0 && strcmp(exp[1], "m") == 0){
                if((strcmp(trm[2], exp[2]) == 0) && (strcmp(trm[3], exp[3]) == 0)){
                    strcat(to_return[0], "mm_sub(");  
                    strcat(to_return[0], trm[0]);
                    strcat(to_return[0], ",");
                    strcat(to_return[0], exp[0]);
                    strcat(to_return[0], ",");
                    strcat(to_return[0], trm[2]);
                    strcat(to_return[0], ",");
                    strcat(to_return[0], trm[3]);
                    strcat(to_return[0], ")");
                    to_return[1] = "m";
                    to_return[2] = trm[2];
                    to_return[3] = trm[3]; 
                    return to_return;                      
                }else{
                    // Dimension of matrices to be substracted doesn't match.
                    to_return[0] = "!";
                    return to_return;
                }  
            }
        }else{
            trm = term(term1);
            if (contains_error(trm[0]) == 1) {
                // Return value of term is syntactically incorrect.
                to_return[0] = "!";
                return to_return;
            }
            strcat(to_return[0], trm[0]);
            to_return[1] = trm[1];
            to_return[2] = trm[2];
            to_return[3] = trm[3];
            return to_return;

        }
    }
}

/*
Recursive function implements BNF of the grammar. 
Detailed explanation of algorithm is in documentation.
IMPORTANT NOTE: 0th index of return value (Modified line translated to c) contains '!' if syntactically incorrect. Safe because character is not a valid one in matlang language. 
*/
char **term(char *str) {
    char ** to_return;  // Pointer used to keep array of strings. Index 0: Modified line translated to c, Index 1: Type, Index 2&3: Dimensions
    to_return = (char **) calloc(8, sizeof(char *));
    to_return[0] = (char *) calloc(128, sizeof(char));
    to_return[1] = (char *) calloc(4, sizeof(char));
    to_return[2] = (char *) calloc(16, sizeof(char));
    to_return[3] = (char *) calloc(16, sizeof(char));
    char ** fct;
    char ** trm;
    space_pass(&str);
    char *ast;
    ast = strchr(str, '*');
    if (ast != NULL) {
        int swtch_p = 0;
        int swtch_b = 0;
        char * factor1 = (char *) calloc(256, sizeof(char));
        int check_for = 0;  // To determine whether term can be separeted by an asterix or not.
        for (int i = 0; i < strlen(str); i++) {
            if (swtch_p < 0 || swtch_b < 0) {
                // Arrangement of paranteses or brackets are syntactically incorrect. e.g. Unmatched parantheses. 
                to_return[0] = "!";
                return to_return;
            }
            char ch = str[i];
            char * chs = (char *) calloc(2, sizeof(char));
            chs[0] = str[i];
            if (ch == '*' && swtch_p == 0 && swtch_b == 0) {
                str += (i + 1);
                check_for = 1;
                break;
            }
            if (ch == '(') {
                swtch_p ++;
            }else if (ch == ')'){
                swtch_p --;
            }else if (ch == '[') {
                swtch_b ++;
            }else if (ch == ']') {
                swtch_b --;
            }
            strcat(factor1, chs);
        }
        if (check_for) {
            fct = factor(factor1);
            if (contains_error(fct[0]) == 1) {
                // Return value of factor is syntactically incorrect.
                to_return[0] = "!";
                return to_return;
            }
            trm = term(str);
            if (contains_error(trm[0]) == 1) {
                // Return value of term is syntactically incorrect.
                to_return[0] = "!";
                return to_return;
            }
            if (strcmp(fct[1], "s") == 0 && strcmp(trm[1], "s") == 0) {
                strcat(to_return[0], fct[0]);
                strcat(to_return[0], "*");
                strcat(to_return[0], trm[0]);
                to_return[1] = "s";
                return to_return;
            }else if (strcmp(fct[1], "s") == 0 && strcmp(trm[1], "m") == 0) {
                strcat(to_return[0], "ms_mup(");
                strcat(to_return[0], trm[0]);
                strcat(to_return[0], ",");
                strcat(to_return[0], fct[0]);
                strcat(to_return[0], ",");
                strcat(to_return[0], trm[2]);
                strcat(to_return[0], ",");
                strcat(to_return[0], trm[3]);
                strcat(to_return[0], ")");
                to_return[1] = "m";
                to_return[2] = trm[2];
                to_return[3] = trm[3];
                return to_return;
            }else if (strcmp(fct[1], "m") == 0 && strcmp(trm[1], "s") == 0) {
                strcat(to_return[0], "ms_mup(");
                strcat(to_return[0], fct[0]);
                strcat(to_return[0], ",");
                strcat(to_return[0], trm[0]);
                strcat(to_return[0], ",");
                strcat(to_return[0], fct[2]);
                 strcat(to_return[0], ",");
                strcat(to_return[0], fct[3]);
                strcat(to_return[0], ")");
                to_return[1] = "m";
                to_return[2] = fct[2];
                to_return[3] = fct[3];
                return to_return;
            }else if (strcmp(fct[1], "m") == 0 && strcmp(trm[1], "m") == 0) {
                if (strcmp(fct[3], trm[2]) == 0) {
                    strcat(to_return[0], "mm_mup(");
                    strcat(to_return[0], fct[0]);
                    strcat(to_return[0], ",");
                    strcat(to_return[0], trm[0]);
                    strcat(to_return[0], ",");
                    strcat(to_return[0], fct[2]);
                    strcat(to_return[0], ",");
                    strcat(to_return[0], fct[3]);
                    strcat(to_return[0], ",");
                    strcat(to_return[0], trm[3]);
                    strcat(to_return[0], ")");
                    to_return[1] = "m";
                    to_return[2] = fct[2];
                    to_return[3] = trm[3];
                    return to_return;
                }else{
                    // Dimension of matrices to be multiplied doesn't match.
                    to_return[0] = "!";
                    return to_return;
                }
            }
        }else{
            fct = factor(factor1);
            if (contains_error(fct[0]) == 1) {
                // Return value of factor is syntactically incorrect.
                to_return[0] = "!";
                return to_return;
            }
            strcat(to_return[0], fct[0]);
            to_return[1] = fct[1];
            to_return[2] = fct[2];
            to_return[3] = fct[3];
            return to_return;
        }
    }else{
        fct = factor(str);
        if (contains_error(fct[0]) == 1) {
            // Return value of factor is syntactically incorrect.
            to_return[0] = "!";
            return to_return;
        }
        strcat(to_return[0], fct[0]);
        to_return[1] = fct[1];
        to_return[2] = fct[2];
        to_return[3] = fct[3];
        return to_return;
    }
}

/*
Recursive function implements BNF of the grammar. 
Detailed explanation of algorithm is in documentation.
IMPORTANT NOTE: 0th index of return value (Modified line translated to c) contains '!' if syntactically incorrect. Safe because character is not a valid one in matlang language. 
*/
char ** factor(char* str) {
    char ** to_return;   // Pointer used to keep array of strings. Index 0: Modified line translated to c, Index 1: Type, Index 2&3: Dimensions
    to_return = (char **) calloc(8, sizeof(char *));
    to_return[0] = (char *) calloc(128, sizeof(char));
    to_return[1] = (char *) calloc(4, sizeof(char));
    to_return[2] = (char *) calloc(16, sizeof(char));
    to_return[3] = (char *) calloc(16, sizeof(char));
    space_pass(&str);
    if (is_empty_str(str)) {
        // Factor cannot be null.
        to_return[0] = "!";
        return to_return;
    }
    char *key;
    key = (char*) calloc(256,sizeof(char));
    while(str[0] != '\0' && str[0] != '(' && str[0] != '['){
        key[strlen(key)] = str[0];
        str += 1;
    }
    if(str[0] == '('){
        //rstrips the string.
        while(key[strlen(key)-1] == ' '){
            key[strlen(key)-1] = '\0' ;
        }
        char *last_occ = strrchr(str, ')');
        if(last_occ == NULL){
            // Parantheses unmatched.
            to_return[0] = "!";
            return to_return;
        }
        *last_occ = '\0';
        str += 1;

        if(key[0] == '\0'){
            char ** returned_str = expr(str);
            if (contains_error(returned_str[0])) {
                to_return[0] = "!";
                // Return value of expr is syntactically incorrect.
                return to_return;
            }
            strcat(to_return[0], "(");
            strcat(to_return[0], returned_str[0]);
            strcat(to_return[0], ")");
            to_return[1] = returned_str[1];
            to_return[2] = returned_str[2];
            to_return[3] = returned_str[3];
            return to_return;
        }else if(strcmp(key, "sqrt") == 0){
            char ** returned_str = expr(str);
            if (contains_error(returned_str[0])) {
                // Return value of expr is syntactically incorrect.
                to_return[0] = "!";
                return to_return;
            }
            if (strcmp(returned_str[1], "m") == 0) {
                // Function sqrt() cannot type matrix type expression as argument. 
                to_return[0] = "!";
                return to_return;
            }
            strcat(to_return[0], "sqrt_c(");
            strcat(to_return[0], returned_str[0]);
            strcat(to_return[0], ")");
            to_return[1] = "s";
            return to_return; 
        }else if(strcmp(key, "tr") == 0){
            char ** returned_str = expr(str);
            if (contains_error(returned_str[0])) {
                // Return value of expr is syntactically incorrect.
                to_return[0] = "!";
                return to_return;
            }
            if (strcmp(returned_str[1], "s") == 0) {
                to_return[1] = "s";
            }else if(strcmp(returned_str[1], "m") == 0){
                to_return[1] = "m";
            }
            strcat(to_return[0], "tr(");
            strcat(to_return[0], returned_str[0]);
            strcat(to_return[0], ",");
            strcat(to_return[0], returned_str[2]);
            strcat(to_return[0], ",");
            strcat(to_return[0], returned_str[3]);
            strcat(to_return[0], ")");
            to_return[2] = returned_str[3];
            to_return[3] = returned_str[2];
            // expr(str) must return valid expr(all types allowed.). 
            return to_return;
        }else if(strcmp(key, "choose") == 0){  
            strcat(to_return[0], "choose(");
            for(int i = 0; i < 3; i++){
                if(strchr(str, ',') == NULL ){
                    // There should be three commas for separting four expressions.
                    to_return[0] = "!";
                    return to_return;
                }else{
                    int swtch_p = 0;
                    int swtch_b = 0;
                    char * l_comma = (char *) calloc(256, sizeof(char));
                    int check_for = 0;
                    for (int i = 0; i < strlen(str); i++) {
                        if (swtch_p < 0 || swtch_b < 0) {
                            // Arrangement of paranteses or brackets are syntactically incorrect. e.g. Unmatched parantheses.
                            to_return[0] = "!";
                            return to_return;
                        }
                        char ch = str[i];
                        char * chs = (char *) calloc(2, sizeof(char));
                        chs[0] = str[i];
                        if (ch == ',' && swtch_p == 0 && swtch_b == 0) {
                            str += (i + 1);
                            check_for = 1;
                            break;
                        }
                        if (ch == '(') {
                            swtch_p ++;
                        }else if (ch == ')'){
                            swtch_p --;
                        }else if (ch == '[') {
                            swtch_b ++;
                        }else if (ch == ']') {
                            swtch_b --;
                        }
                        strcat(l_comma, chs);
                    }
                    if (check_for == 0) {
                        // Comma doesn't separate the expressions properly.
                        to_return[0] = "!";
                        return to_return;
                    }
                    char ** returned_l_comma = expr(l_comma);
                    if (contains_error(returned_l_comma[0])) {
                        // Return value of expr is syntactically incorrect.
                        to_return[0] = "!";
                        return to_return;
                    }
                    if(strcmp(returned_l_comma[1], "s") == 0) {
                        strcat(to_return[0], returned_l_comma[0]);
                        strcat(to_return[0], ",");
                    }else {
                        // Expressions in choose function must be of scalar type.
                        to_return[0] = "!";
                        return to_return;
                    }
                }                        
            }
            char ** returned_r_comma = expr(str);
            if (contains_error(returned_r_comma[0])) {
                // Return value of expr is syntactically incorrect.
                to_return[0] = "!";
                return to_return;
            }
            if(strcmp(returned_r_comma[1], "s") == 0) {
                strcat(to_return[0], returned_r_comma[0]);
                strcat(to_return[0], ")");
            }else {
                // Expressions in choose function must be of scalar type.
                to_return[0] = "!";
                return to_return;
            }
            to_return[1] = "s";
            return to_return;

        }
        //Invalid keyword for function call.
        to_return[0] = "!";
        return to_return;

    }else if(str[0] == '[') {

        //rstrip the string.
        while(key[strlen(key)-1] == ' '){
            key[strlen(key)-1] = '\0' ;
        }
        if(is_declared(key)){
            char *type = give_type(key)[0];
            if(strcmp(type, "0") == 0){
                // Scalar type cannot be indexed.
                to_return[0] = "!";
                return to_return;
            }else if(strcmp(type, "1") == 0){
                //Analysis of Vector.
                char *last_occ = strrchr(str, ']');
                if(last_occ == NULL){
                    // Brackets unmatched.
                    to_return[0] = "!";
                    return to_return;
                }
                *last_occ = '\0';
                str += 1;
                char ** returned_str = expr(str);
                if (contains_error(returned_str[0])) {
                    // Return value of expr is syntactically incorrect.
                    to_return[0] = "!";
                    return to_return;
                }
                if(strcmp(returned_str[1], "s") != 0) {
                    // Indexing takes scalar type as argument.
                    to_return[0] = "!";
                    return to_return;
                }
                strcat(to_return[0], key);
                strcat(to_return[0], "[");
                strcat(to_return[0], "(int) ");
                strcat(to_return[0], returned_str[0]);
                strcat(to_return[0], "-1");
                strcat(to_return[0], "]");
                strcat(to_return[0], "[");
                strcat(to_return[0], "0");
                strcat(to_return[0], "]");
                to_return[1] = "s";
                to_return[2] = returned_str[2];
                to_return[3] = returned_str[3];
                return to_return;
            }else if(strcmp(type, "2") == 0){
                //Analysis of Matrix.
                char *last_occ = strrchr(str, ']');
                if(last_occ == NULL){
                    //Brackets unmatched.
                    to_return[0] = "!";
                    return to_return;
                }
                *last_occ = '\0';
                str += 1;
                if(strchr(str, ',') == NULL){
                    // No comma between matrix expressions.
                    to_return[0] = "!";
                    return to_return;
                }
                int swtch_p = 0;
                int swtch_b = 0;
                char * expr_l = (char *) calloc(256, sizeof(char));
                int check_for = 0;
                for (int i = 0; i < strlen(str); i++) {
                    if (swtch_p < 0 || swtch_b < 0) {
                        // Arrangement of paranteses or brackets are syntactically incorrect. e.g. Unmatched parantheses.
                        to_return[0] = "!";
                        return to_return;
                    }
                    char ch = str[i];
                    char * chs = (char *) calloc(2, sizeof(char));
                    chs[0] = str[i];
                    if (ch == ',' && swtch_p == 0 && swtch_b == 0) {
                        str += (i + 1);
                        check_for = 1;
                        break;
                    }
                    if (ch == '(') {
                        swtch_p ++;
                    }else if (ch == ')'){
                        swtch_p --;
                    }else if (ch == '[') {
                        swtch_b ++;
                    }else if (ch == ']') {
                        swtch_b --;
                    }
                    strcat(expr_l, chs);
                }
                if (check_for) {
                    char ** returned_l_str = expr(expr_l);
                    if (contains_error(returned_l_str[0])) {
                        // Return value of expr is syntactically incorrect.
                        to_return[0] = "!";
                        return to_return;
                    }
                    if(strcmp(returned_l_str[1], "s") != 0) {
                        // Indexing takes scalar type as argument.
                        to_return[0] = "!";
                        return to_return;
                    }
    
                    char ** returned_r_str = expr(str);
                    if (contains_error(returned_r_str[0])) {
                        // Return value of expr is syntactically incorrect.
                        to_return[0] = "!";
                        return to_return;
                    }
                    if(strcmp(returned_r_str[1], "s") != 0) {
                        // Indexing takes scalar type as argument.
                        to_return[0] = "!";
                        return to_return;
                    }
    
                    strcat(to_return[0], key);
                    strcat(to_return[0], "[");
                    strcat(to_return[0], "(int) ");
                    strcat(to_return[0], returned_l_str[0]);
                    strcat(to_return[0], "-1");
                    strcat(to_return[0], "][");
                    strcat(to_return[0], "(int) ");
                    strcat(to_return[0], returned_r_str[0]);
                    strcat(to_return[0], "-1");
                    strcat(to_return[0], "]");
                    to_return[1] = "s";
                    return to_return;
                }else {
                    // Comma doesn't separate indices properly.
                    to_return[0] = "!";
                    return to_return;
                }
            }   
        }else{
            // Variable name not declared.
            to_return[0] = "!";
            return to_return;
        }
    }else if (str[0] == '\0') {
        //rstrip the string.
        while(key[strlen(key)-1] == ' '){
            key[strlen(key)-1] = '\0' ;
        }
        if(is_declared(key)){
            char **type = give_type(key);    
            if (strcmp(type[0], "0") == 0) {
                to_return[1] = "s";
                strcat(to_return[0], key);
                return to_return;
            }else {
                to_return[1] = "m";
                strcat(to_return[0], key);
                to_return[2] = type[1];
                to_return[3] = type[2];
                return to_return;
            }
        }
        int only_dot = 0;
        if (key[0] == '.' || key[strlen(key) - 1] == '.') {
            // Number cannot begin or end with a dot.
            to_return[0] = "!";
            return to_return;
        }
        for (int i = 0; i < strlen(key); i++){
            if (key[i] == '.') {
                if (only_dot == 0) {
                    only_dot = 1;
                    continue;
                }else {
                    // Number cannot have more than one dots.
                    to_return[0] = "!";
                    return to_return;
                }
            }
            if(isdigit(key[i]) == 0){
                // Number can have characters, one dot or digit(s).
                to_return[0] = "!";
                return to_return;
            }
        }
        to_return[1] = "s";
        strcat(to_return[0], key);
        return to_return;
    }else{
        // Doesn't match any syntactically correct expression to evaluate in factor.
        to_return[0] = "!";
        return to_return;
    }
}



