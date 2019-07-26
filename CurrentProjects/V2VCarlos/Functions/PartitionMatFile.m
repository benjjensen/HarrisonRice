function PartitionMatFile(matFileName)
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here


myfile = matfile(matFileName); % Gets the original file

%% Create file with everything but the data
matName = [matFileName(1:end-4) '_other.mat']; % Creates the name for the new file
mat = matfile(matName,'Writable',true); % Creates the new file and allows it to be written to
% The remainder of this section takes each of the variables stored in the
% original file and creates a copy of them in the new file, except for the
% data variable (the variables were found by using the whos command and
% then hard coded into this section). These variables are never large
% enough to cause problems and so are all left together.
mat.altitudes = myfile.altitudes;
mat.date = myfile.date;
mat.distances = myfile.distances;
mat.duration = myfile.duration;
mat.latitudes = myfile.latitudes;
mat.longitudes = myfile.longitudes;
mat.name = myfile.name;
mat.notes = myfile.notes;
mat.samplesStoredBeforeEachGPSFix = myfile.samplesStoredBeforeEachGPSFix;
mat.time = myfile.time;

%% Partition the data into multiple files
info = whos(myfile, 'data'); % isolates the data variable
size = info.size; % Gets the size of the data variable (it is stored in one column, so the size returns [#, 1]
dataindex = 1; % Keeps track of how far into the old file we are (initializes as one to set it to the first row)
addperrun = 5e8; % The number of rows that will be transferred to the new file per loop iteration (this can't be too large as it will use up all of the ram, [MAY THROW AN ERROR, I'M NOT SURE YET, WE COULD TEST IT WITH A LARGER NUMBER IF DESIRED])
breakpoint = 9e9; % The number of elements that will be stored in each file before creating another (where it will create a partition) (1e9 corresponds to approximately 2.2 GB)
numPartitions = 1; % Keeps track of which file number it is on, initialized at 1 and will count up each time the breakpoint is reached
firstrun = true; % Indicates whether or not it is the first run after a new file is created
endoffile = breakpoint; % Keeps track of the data index at which the next file should be created
count = 0;

while (dataindex < size(1)) % While the dataindex is less than the number of rows in the original data file
    if (dataindex >= endoffile) % If the dataindex has reached where the next file should be created
        numPartitions = numPartitions + 1; % Increase the tracker for the number of partitions
        endoffile = endoffile + breakpoint; % Set the new file endpoint 
        firstrun = true; % Indicate that it will be the first run for the new file
        count = 0;
    end
    if (firstrun) % When its the first run
        matName = [matFileName(1:end-4) '_data' num2str(numPartitions) '.mat']; % Sets the name of the new file
        mat = matfile(matName,'Writable',true); % Creates the new file using the name and allows it to be written to
        mat.data = myfile.data(dataindex:dataindex+addperrun-1,1); % Creates the data variable and assigns the first portion to it
        firstrun = false; % Indicates that the first run is done
    else % When it is not the first run
        getDataVar = whos(mat, 'data');
        newsize = getDataVar.size; % Gets the current size of the new file
        mat.dat(newsize(1)+1:newsize(1)+addperrun,1) = myfile.data(dataindex:dataindex+addperrun-1,1); % Adds the desired number of rows into the new file from the previous file
    end
    count = count + 1;
    time = datetime;
    dispstr = ['Iteration number ' num2str(count) ' of ' num2str(breakpoint/addperrun) ' in file ' num2str(numPartitions) ' @ ' char(time)];
    disp(dispstr);
    dataindex = dataindex + addperrun; % Increments the data index to match what has already been transferred
    if (dataindex+addperrun-1 > size(1)) % Checks to see if the next additionto the new file will exceed the end of the original file
        addperrun = size(1) - dataindex + 1; % Reduces the addperrun variable to then only go to the end of the file
    end
end

