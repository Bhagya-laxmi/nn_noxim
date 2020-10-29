Notice that this is a temporary document, because the simulator is still in the development stage.
Recommended OS: CentOS.
Please source your SystemC before using it.
Detailed description reference"link_library", and type instruction: source link_library !!!

Please compile it in the "bin" folder before you start to use this NN-Noxim.
Compile instruction
% make

****************************************************************************************************************************************
Sample instruction
% ./noxim -dimx 8 -dimy 8 -dimz 1 -NNmodel model.txt -NNweight weight.txt -NNinput input.txt -mapping random -groupsize 800 -sim 10000

" -dimx 8 -dimy 8 -dimz 1 "
Describe the 2D-NoC size. (Since we developed using 3D-Noxim code, "-dimz 1" necessary for now.)

"-NNmodel model.txt"
The flag is to enter the NN model description file.
Detailed description reference"model.txt".

"-NNweight weight.txt"
The flag is to enter the NN weight information file.

"-NNinput input.txt"
The flag is to enter the NN input file, for noce!

"-groupsize 800"
Describe the approximate operations that each PE can perform in units of software NN neurons.

"-mapping random"
The flag is to enter the NoC mapping algorithm, dirx & diry is also bluid-in.
There is also a "mapping.txt", which can also do the dirx algorithm by using instruction "-mapping table mapping.txt".

"-sim 10000"
The flag is to set simulation time in cycle. *** It is important to set it larger than it need. *** 

Notice that it will print the sending and receiving packets situation of each PEs in "PE_X_XX.txt", and the final result of computation in "output.txt".
Other information will output in Terminal.
****************************************************************************************************************************************

To know more information of NN-Noxim, you can use the following instruction.
% ./noxim -help 

****************************************************************************************************************************************
If you have any problem, you can contect with:
(1) kcchen@mail.cse.nsysu.edu.tw, Professor Kun-Chih (Jimmy) Chen
(2) tingyi@cereal.cse.nsysu.edu.tw
(3) andrewyong@cereal.cse.nsysu.edu.tw
****************************************************************************************************************************************