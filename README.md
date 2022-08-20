##### Troque para [Português Brasileiro](./README.pt.md) 🇧🇷.

## :fountain_pen: About

**Ribabari** is an operating system simulator focused on hardware resource management among different processes. Our operating system simulator provides management for these following resources, **CPU**, **Memory**, **I/O** and **Disk**.

## 🎓 A college Project
**Ribabari** operating system simulator project is a requirement for the **Operating Systems** course.

## :book: Brief
Our simulator provides a interative terminal for users run its processes and see the processes execution among different views, being each view for a different resource.

#### :question: **Have you not understood what is a view?**
A view is basically a window in the simulator terminal that shows under the hood the operating system managing a hardware resource. In each view, a different hardware resource is being managed (e.g., there are views showing the *OS* managing the disk and semaphores).

#### :question: **How can I execute my own process?**
Oh, wait a minute, following sections is going to detail how to write a **synthetic program file** :spiral_notepad: in which specifies the process instruction, the segment size, semaphores to be used, and so more.

## :hammer_and_wrench: Compiling
First of all, it is a good way to start to compile the simulator first. Therefore, we suggest to follow the next steps.

#### 1. Installing the gcc.
Install the *gcc* the **C** compiler executing the following line.
<p align="center"><i>sudo apt install gcc</i></p>

:bell: **Note**: If you already have the *gcc* installed, then you can jump to the next step.

#### 2. Installing the make.
Install the *make* easies the compilation of our simulator enabling compile the entire project using just one command.

<p align="center"><i>sudo apt install make</i></p>

:bell: **Note**: If you already have the *make* installed, then you can jump to the next step.

#### 3. Installing the ncurses.
Install the *ncurses* is the library used by our simulator for create the terminal interface.

<p align="center"><i>sudo apt install libncurses-dev</i></p>

:bell: **Note**: If you already have the *ncurses* installed, then you are **ready**. Hence, go to the next section.

## :rocket: Running
If you already followed the steps provided in the previous section, thne you are **ready** to run our operating system simulator, for that run the following line **(in the same directory in which the Makefile is)**.

<p align="center"><i>make run</i></p>

After that, take a good taste of our simulator!

## :mag_right: Terminal Menu Options
If is this the first time you are running our simulator, then is essential that you read this section. The terminal menu provides three options, being them

#### 1. Process Create
<p align="center"><i>Allow the user to create a process and execute its instructions. Pressing it, opens a window on the center of the window asking for the synthetic program file path.</i></p>

#### 2. Toggle Resource Acquisition View
<p align="center"><i>It shows/hide the resource acquisition view.</i></p>

#### 3. Exit
<p align="center"><i>It closes the operating system simulator.</i></p>

## :clapper: How can I run my own process?
All of the process executed by the operating system simulato are inputted using a *special file* called **synthetic program file**. This file has a specific format that specifies process metadata and the process' instructions that the *CPU* is going to execute.

### :scroll: Synthetic Program File
The file format is given blow

```vim
program name
segment id
process start priority
segment size
semaphore list (each semaphore name separated by a blank space)
<blank line>
sequence of instructions separated by line
```

Each file specifier is going to be detailed below.

- **program name**: The process name. There is no need to be unique.
- **segment id**: The memory segment id that is going to bee allocate to the process the segment identifier **MUST BE** unique. Otherwise, unexpected behvaior may occur along the simulation.
- **process start priority**: It indicates in which CPU scheduling queue the process must start, being *0* for **highest priority** and *1* for **lowest priority**.
- **segment size**: It indicates the segment size that is going to be allocated to the process.
- **semaphore list**: The semaphore list that may be used by the process along the simulation.

An example of such synthetic program file is

```vim
process1
50
0
24
s

exec 20
exec 80
P(s)
read 110
print 50
exec 40
write 150
V(s)
exec 30
```

:bell: **Note 1**: The synthetic program file given above specifies a process with the name *process1*, with segment identifier *50*, having initial cpu priority set to *0 (**highest priority**)*, with segment size set to *24 kbytes* and *s* as the semaphore to be used.

:bell: **Note 2**: The synthetic program file does not need to have any file extesion or any special name. Therefore, you can give any name you would like to.

:stop_sign: **Important**: The synthetic program file **MUST NOT** have a blank line at the end of the file (look at the file format given above again if needed), and the synthetic program file **MUST BE** in the **LR** format.

:question: **Wait a minute, what are the available instructions?** Of course, we already know you'd ask that. Please, go to the next subsection where we present the available CPU instructions.

#### :desktop_computer: Available CPU Instructions
The *available instructions* listed below are all instructions that can be insert into the sequence of instructions part of the synthetic program file. Therefore, the available instructions are

- **exec k**: Indicate that the program is going to execute (something) for **k** units of time.
- **read k**: Indicate that the program is going to read (something) from the **k** disk's track.
- **write k**: Indicate that the progra mis going to write (something) to the **k** disk's track.
- **print t**: Indicate that the program is going to print (something) for **k** units of time.
- **P(s)**: Indicate that the program is going to access the critical region bounded by the semaphore **s**.
- **V(s)**: Indicate that the program is going to release the critical region bounded by the semaphore **s**.

After that, have fun!
