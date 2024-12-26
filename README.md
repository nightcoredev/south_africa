Building
-------
1.  **Dependencies:** No external libraries needed
2.  **Compilation (using MinGW as an example):**

    ```bash
    gcc -o sysmon.exe main.c -lpsapi
    ./sysmon.exe
    ```
 
    
Usage
-----

1.  Compile the code as noted in the "Building" section.
2.  Run the executable (`sysmon.exe` for Windows).
3.  Press Enter to start the process. The program will automatically run through each demo, displaying the results to the console

Code Structure
--------------
*   **`systemm.h`:** Contains function prototypes, structure definitions (like `MemoryBlock`), and configuration constants.
*   **`main.c`:** Contains the implementations of the functions declared in `systemm.h`, the `main` function, and the memory tracking data
