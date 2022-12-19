# Language_Translator_to_C
This project was my 1st Homework for the course CMPE 230 (Systems Programming) at Bogazici University. Our task was: Given a language designed by our instructor, implementation of the BNF of the language using recursion, checking syntax using BNF / grammar rules and the translation of the given language code to C language code.
## About the Project
In this project we (me and my group partner) have implemented a translator for a language called matlang, that translates the matlang code to C language code. If there is a syntactic error, translator detects it and gives the error with the regarding line. The detailed information about the designated language, matlang, as well as other information/rules about the project can be found in the description file. Also there is a documentation file that covers our design & approach for the implementation as well as the BNF grammar scheme that we have created for the matlang language.
## Personal Thoughts About the Code
I personally found this project to be hard, since it is basically implementation of almost a compiler. Additional hardness comes from the fact that it was our very first project in C language. Although these complexities, I think our implementation of the BNF recursively is very well designed. (But I accept it is not very self-explanatory.) But in the main part we could not follow good programming practices since it involves many if-else conditions and not very well designed. (Although the design is not very well, it covers nearly all the situations with some exceptions that are given in the 'Final Remarks' part.) This is mainly because the strict timing issues regaring with the project.
## To Run the Code
First, to avoid issues regarding with the line endings, one should be sure about the format of the input file. To be on the safe side, one should install 'dos2unix' in order to make the conversion.
Example brew formulae as follows:

```brew install dos2unix```

Then, make the conversion of all the testcase file with:

```dos2unix example_test_cases/*.mat```

Now, we are ready to test the code. Move into the directory where the code is:

```cd translator_volcaniqueo```

Since it alrady has a 'Makefile' simply run:

```make```

Now we have the executable file called matlang2c. Thus, test the input file with:

```./matlang2c inputfile.mat```

In this stage if the given input does not syntactically comply with the matlang language, you will see a error on the console:

```Error (Line <line_number>)```

Otherwise, if the input is syntactically correct, matlang2c creates a C file named 'inputfile.c'. Now compile this code with:

```gcc inputfile.c -o prog```

Finally run the executable with:

```./prog```

It will print the result of the designated operations on the console.

## Final Remarks
We have forgotten to cover two scenerios: '#' character is not at the beginning of the input file & Allow to re/assign single elements of a matrix/vector. That is why, unfortunately, test cases, testcase7.mat & testcase28.mat, will fail to create the designated C file and give an 'Error' statement on the console.

After either, 'make' command or 'gcc' command you may see warnings about:

```non-void function does not return a value in all control paths``` 

This is because some functions in 'project1.c' and 'functions.c' does not cover return values for the cases that are not possible the execute. (Since the compiler can not know whether it is possible or not to execute such scenerio, it naturally gives a warning.)




