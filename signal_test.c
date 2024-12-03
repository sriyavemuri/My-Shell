
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

int main() {
    printf("My PID is: %d\n", getpid()); // Print process ID for sending signals

    while (1) {
        // Do some work here
        sleep(1); // Simulate some ongoing activity
    }

    return 0;
}
