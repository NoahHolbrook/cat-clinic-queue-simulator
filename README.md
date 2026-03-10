# Cat Clinic Queue Simulator

A C program that simulates a cat clinic scheduling system using sorted linked lists.

## About

This program reads cat patient records from a file and schedules them across two doctors (Doctor Uno and Doctor Dos) based on arrival time. It tracks which cats get treated, which get rejected due to time constraints, and which cats may have been exposed to other patients seen by Doctor Dos.

## How It Works

- Cats arrive at the clinic with a name, arrival time, and treatment duration
- They are inserted into a sorted queue by arrival time
- Two doctors are available, starting free at minute 0
- The doctor who becomes free first treats the next cat (ties go to Doctor Uno)
- If a treatment would run past minute 480 (8 hours), the cat is rejected
- Cats treated by Doctor Dos are tracked for an exposure report

## Features

- Sorted linked list insertion (no sorting algorithm — each node placed in position immediately)
- Dynamic memory allocation for cat names
- Separate rejection and exposure tracking lists
- Full memory cleanup on exit

## Usage

Compile and run:
```bash
gcc pa3.c -o clinic
./clinic
```

Requires a `logs.txt` file in the same directory with the format:
```
<arrival> <name> <duration>
...
-1
```

## Author

Noah Holbrook
