%% CombineAndProcessArraysTX1
% Created by Kalin Norman
% Should be a one time use file that converts all of the raw data into a
% usable form for all of the post-processing for the figures and other
% useful information. 

% As a side note, none of the first transmitter location's data made it
% into the conference paper, but it was collected, so these files still
% remain. 

% Separate files for dB and linear data outputs are available. This one
% will output an array of numbers that are in linear

clear; close all;
%% Load Individual Arrays
for i = 1:26 % Load in data from Camacho's office
    eval(sprintf('load Data/tx1Data/camacho_%d.mat;', i));
end
for i = 1:28 % Load in data from Chamber's office
    eval(sprintf('load Data/tx1Data/chambers_%d.mat;', i));
end
for i = 1:55 % Load in data from Conference room
    eval(sprintf('load Data/tx1Data/conference_main_%d.mat;', i));
end
for i = 1:3 % Load in data from Conference room
    eval(sprintf('load Data/tx1Data/conference_north_%d.mat;', i));
end
for i = 1:3 % Load in data from Conference room
    eval(sprintf('load Data/tx1Data/conference_south_pillar_%d.mat;', i));
end
for i = 1:4 % Load in data from Conference room
    eval(sprintf('load Data/tx1Data/conference_south_wall_%d.mat;', i));
end
for i = 1:300 % Load in data from Hallway
    eval(sprintf('load Data/tx1Data/hallway_%d.mat;', i));
end
for i = 1:29 % Load in data from Harrison's office
    eval(sprintf('load Data/tx1Data/harrison_%d.mat;', i));
end
for i = 1:31 % Load in data from Smalley's office
    eval(sprintf('load Data/tx1Data/smalley_%d.mat;', i));
end

%% Combine Into One Array Per Room

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% This section takes the individual arrays generated from data
% collection and first turns each section or room into a complete
% three dimensional array that is oriented such that it can be used
% for the generation of a heatmap (co-ordinate (1,1) is in the top
% left corner of the array). After each section, or room, has been
% oriented and combined, those arrays are then inserted into one large
% array that represents the entire section where our data was
% collected. Points where we do not have data are represented by NaN's
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Hallway
numbarrays = 300;
numbloops = 23;
hallway_data = nan(2048,numbloops,numbarrays);
for numbtests = 1:numbarrays
    for numbcorrections = 1:numbloops
        obj = 'hallway_' + string(numbtests) + '(:,' + string(numbcorrections) + ')';
        hallway_data(:,24 - numbcorrections,numbtests) = eval(obj);
    end
end
% Camacho's Office
numbarrays = 26;
numbloops = 32;
camacho_data = nan(2048,numbloops,numbarrays);
for numbtests = 1:numbarrays
    for numbcorrections = 1:numbloops
        obj = 'camacho_' + string(numbtests) + '(:,' + string(numbcorrections) + ')';
        camacho_data(:,33 - numbcorrections,numbtests) = eval(obj);
    end
end
% Chambers' Office
num_data_arrays = 28;
num_loops_for_area = 32;
chambers_data = nan(2048,num_loops_for_area,num_data_arrays);
for numtests = 1:num_data_arrays - 2
    obj = 'chambers_' + string(numtests) + '(:,:)';
    chambers_data(:,:,numtests) = eval(obj);
end
for numtests = num_data_arrays - 1:num_data_arrays
    obj = 'chambers_' + string(numtests) + '(:,:)';
    chambers_data(:,1:29,numtests) = eval(obj);
end
% Conference Room
numbarrays = 65;
numbloops = 61;
conference_data = nan(2048,numbarrays,numbloops);
for numbtests = 1:4
    for numbcorrections = 1:29
        obj = 'conference_south_wall_' + string(numbtests) + '(:,' + string(numbcorrections) + ')';
        conference_data(:,5 - numbtests,62 - numbcorrections) = eval(obj);
    end
end
for numbtests = 1:3
    for numbcorrections = 1:58
        obj = 'conference_south_pillar_' + string(numbtests) + '(:,' + string(numbcorrections) + ')';
        conference_data(:,8 - numbtests,62 - numbcorrections) = eval(obj);
    end
end
for numbtests = 1:55
    for numbcorrections = 1:61
        obj = 'conference_main_' + string(numbtests) + '(:,' + string(numbcorrections) + ')';
        conference_data(:,63 - numbtests,62 - numbcorrections) = eval(obj);
    end
end
for numbtests = 1:3
    for numbcorrections = 1:55
        obj = 'conference_north_' + string(numbtests) + '(:,' + string(numbcorrections) + ')';
        conference_data(:,66 - numbtests,59 - numbcorrections) = eval(obj);
    end
end
% Harrison's office
numbarrays = 29;
numbloops = 33;
harrison_data = nan(2048,numbarrays,numbloops);
for numbtests = 1:numbarrays
    obj = 'harrison_' + string(numbtests) + '(:,:)';
    harrison_data(:,30 - numbtests,:) = eval(obj);
end
% Smalley's office
numbarrays = 31;
numbloops = 26;
smalley_data = nan(2048,numbarrays,numbloops);
for numbtests = 1:numbarrays
    for numbcorrections = 1:numbloops
        obj = 'smalley_' + string(numbtests) + '(:,' + string(numbcorrections) + ')';
        smalley_data(:,32 - numbtests,27 - numbcorrections) = eval(obj);
    end
end

%% Pwelch Data And Leave In Linear

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% This section takes the arrays that are created in the above section and 
% runs each of them through a pwelch function in order to obtain 
% understandable data from what was originally gathered.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

Nfft = 2*64;
FF = -0.5:1/Nfft:0.5-1/Nfft;
FF = 20*FF;
% Hallway
clear nanarray;
nanarray = isnan(hallway_data);
xlength = 300;
ylength = 23;
hallway_pwelch = nan(128,ylength,xlength);
for count = 1:xlength
    for loops = 1:ylength
        if nanarray(:,loops,count) == 1
            
        else
            YY = pwelch(hallway_data(:,loops,count),boxcar(Nfft),0,Nfft,'twosided');
            hallway_pwelch(:,loops,count) = fftshift(YY);
        end
    end
end
% Camacho's Office
clear nanarray;
nanarray = isnan(camacho_data);
xlength = 26;
ylength = 32;
camacho_pwelch = nan(128,ylength,xlength);
for count = 1:xlength
    for loops = 1:ylength
        if nanarray(:,loops,count) == 1
            
        else
            YY = pwelch(camacho_data(:,loops,count),boxcar(Nfft),0,Nfft,'twosided');
            camacho_pwelch(:,loops,count) = fftshift(YY);
        end
    end
end
% Chamber's Office
clear nanarray;
nanarray = isnan(chambers_data);
xlength = 28;
ylength = 32;
chambers_pwelch = nan(128,ylength,xlength);
for count = 1:xlength
    for loops = 1:ylength
        if nanarray(:,loops,count) == 1
            
        else
            YY = pwelch(chambers_data(:,loops,count),boxcar(Nfft),0,Nfft,'twosided');
            chambers_pwelch(:,loops,count) = fftshift(YY);
        end
    end
end
% Conference Room
clear nanarray;
nanarray = isnan(conference_data);
xlength = 61;
ylength = 65;
conference_pwelch = nan(128,ylength,xlength);
for count = 1:xlength
    for loops = 1:ylength
        if nanarray(:,loops,count) == 1
            
        else
            YY = pwelch(conference_data(:,loops,count),boxcar(Nfft),0,Nfft,'twosided');
            conference_pwelch(:,loops,count) = fftshift(YY);
        end
    end
end
% Harrison's Office
clear nanarray;
nanarray = isnan(harrison_data);
xlength = 33;
ylength = 29;
harrison_pwelch = nan(128,ylength,xlength);
for count = 1:xlength
    for loops = 1:ylength
        if nanarray(:,loops,count) == 1
            
        else
            YY = pwelch(harrison_data(:,loops,count),boxcar(Nfft),0,Nfft,'twosided');
            harrison_pwelch(:,loops,count) = fftshift(YY);
        end
    end
end
% Smalley's Office
clear nanarray;
nanarray = isnan(smalley_data);
xlength = 26;
ylength = 31;
smalley_pwelch = nan(128,ylength,xlength);
for count = 1:xlength
    for loops = 1:ylength
        if nanarray(:,loops,count) == 1
            
        else
            YY = pwelch(smalley_data(:,loops,count),boxcar(Nfft),0,Nfft,'twosided');
            smalley_pwelch(:,loops,count) = fftshift(YY);
        end
    end
end

%% Piece Together Into One Array

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% This file takes the pwelch data arrays and inserts them in the proper
% locations with respect to one another into one array that represents the
% complete area over which the data was collected
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
tx1_linear = nan(128,91,345);
% Insert Conference room
for temp = 1:65
    for loops = 1:61
        obj = 'conference_pwelch(:,' + string(temp) + ',' + string(loops) + ')';
        tx1_linear(:,temp,loops) = eval(obj);
    end
end
% Insert Harrison's office
for temp = 1:29
    for loops = 1:33
        obj = 'harrison_pwelch(:,' + string(temp) + ',' + string(loops) + ')';
        tx1_linear(:,36+temp,64+loops) = eval(obj);
    end
end
% Insert Smalley's office
for temp = 1:31
    for loops = 1:26
        obj = 'smalley_pwelch(:,' + string(temp) + ',' + string(loops) + ')';
        tx1_linear(:,34+temp,122+loops) = eval(obj);
    end
end
% Insert Camacho's office
for temp = 1:32
    for loops = 1:26
        obj = 'camacho_pwelch(:,' + string(temp) + ',' + string(loops) + ')';
        tx1_linear(:,33+temp,153+loops) = eval(obj);
    end
end
% Insert Chambers' office
for temp = 1:32
    for loops = 1:28
        obj = 'chambers_pwelch(:,' + string(temp) + ',' + string(loops) + ')';
        tx1_linear(:,33+temp,212+loops) = eval(obj);
    end
end
% Insert Hallway
for temp = 1:23
    for loops = 1:300
        obj = 'hallway_pwelch(:,' + string(temp) + ',' + string(loops) + ')';
        tx1_linear(:,68+temp,42+loops) = eval(obj);
    end
end

%% Separate Signal and Noise Carriers

tx1_signal_linear(:,:,:) = tx1_linear(1:2:end,:,:);
tx1_noise_linear(:,:,:) = tx1_linear(2:2:end,:,:);

tx1_signal_lin_gc(:,:,:) = tx1_signal_linear(11:55,:,:);
tx1_noise_lin_gc(:,:,:) = tx1_noise_linear(10:54,:,:);

save Data/tx1_linear.mat tx1_linear;
save Data/tx1_signal_linear.mat tx1_signal_linear;
save Data/tx1_noise_linear.mat tx1_noise_linear;
save Data/tx1_signal_lin_gc.mat tx1_signal_lin_gc;
save Data/tx1_noise_lin_gc.mat tx1_noise_lin_gc;